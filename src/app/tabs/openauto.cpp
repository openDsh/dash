#include "app/tabs/openauto.hpp"

#include "app/config.hpp"
#include "app/theme.hpp"
#include "app/widgets/ip_input.hpp"
#include "app/widgets/progress.hpp"
#include "app/window.hpp"

OpenAutoWorker::OpenAutoWorker(std::function<void(bool)> callback, bool night_mode, QWidget *frame)
    : QObject(qApp),
      io_service(),
      work(io_service),
      configuration(Config::get_instance()->openauto_config),
      tcp_wrapper(),
      usb_wrapper((libusb_init(&usb_context), usb_context)),
      query_factory(usb_wrapper, io_service),
      query_chain_factory(usb_wrapper, io_service, query_factory),
      service_factory(io_service, configuration, frame, callback, night_mode),
      android_auto_entity_factory(io_service, configuration, service_factory),
      usb_hub(std::make_shared<aasdk::usb::USBHub>(usb_wrapper, io_service, query_chain_factory)),
      connected_accessories_enumerator(
          std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(usb_wrapper, io_service, query_chain_factory)),
      app(std::make_shared<openauto::App>(io_service, usb_wrapper, tcp_wrapper, android_auto_entity_factory, usb_hub,
                                          connected_accessories_enumerator)),
      socket(std::make_shared<boost::asio::ip::tcp::socket>(io_service))
{
    this->create_usb_workers();
    this->create_io_service_workers();
}

OpenAutoWorker::~OpenAutoWorker()
{
    std::for_each(this->thread_pool.begin(), this->thread_pool.end(),
                  std::bind(&std::thread::join, std::placeholders::_1));
    libusb_exit(this->usb_context);
}

void OpenAutoWorker::connect_wireless(QString address)
{
    try {
        this->tcp_wrapper.asyncConnect(*this->socket, address.toStdString(), this->OPENAUTO_PORT,
                                       [this, address](const boost::system::error_code &ec) {
                                           if (!ec) {
                                               this->app->start(this->socket);
                                               emit wireless_connection_success(address);
                                           }
                                           else {
                                               emit wireless_connection_failure();
                                           }
                                       });
    }
    catch (const boost::system::system_error &se) {
        emit wireless_connection_failure();
    }
}

void OpenAutoWorker::create_usb_workers()
{
    std::function<void()> worker = [this]() {
        timeval event_timeout = {180, 0};
        while (!this->io_service.stopped())
            libusb_handle_events_timeout_completed(this->usb_context, &event_timeout, nullptr);
    };

    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
}

void OpenAutoWorker::create_io_service_workers()
{
    std::function<void()> worker = [this]() { this->io_service.run(); };

    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
}

void OpenAutoFrame::mouseDoubleClickEvent(QMouseEvent *)
{
    this->toggle_fullscreen();
    emit double_clicked(this->fullscreen);
}

OpenAutoTab::OpenAutoTab(QWidget *parent) : QStackedWidget(parent)
{
    this->config = Config::get_instance();
    this->theme = Theme::get_instance();

    this->frame = new OpenAutoFrame(this);

    std::function<void(bool)> callback = [frame = this->frame](bool active) { frame->toggle(active); };
    this->worker = new OpenAutoWorker(callback, this->theme->get_mode(), frame);
    this->worker->start();

    connect(this->frame, &OpenAutoFrame::toggle, [this](bool enable) {
        if (!enable && this->frame->is_fullscreen()) {
            this->addWidget(frame);
            this->frame->toggle_fullscreen();
        }
        this->setCurrentIndex(enable ? 1 : 0);
    });
    connect(this->frame, &OpenAutoFrame::double_clicked, [this](bool fullscreen) {
        if (fullscreen) {
            emit toggle_fullscreen(this->frame);
        }
        else {
            this->addWidget(frame);
            this->setCurrentWidget(frame);
        }
        this->worker->update_size();
    });
    connect(this->theme, &Theme::mode_updated, [this](bool mode) { this->worker->set_night_mode(mode); });

    this->addWidget(this->connect_msg());
    this->addWidget(this->frame);
}

void OpenAutoTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    this->frame->resize(this->size());
    this->worker->update_size();
}

QWidget *OpenAutoTab::connect_msg()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect device to start Android Auto", widget);
    label->setFont(Theme::font_16);
    label->setAlignment(Qt::AlignCenter);

    QWidget *connection = this->wireless_config();
    if (!this->config->get_wireless_active())
        connection->hide();

    QCheckBox *wireless_button = new QCheckBox("wireless", widget);
    wireless_button->setFont(Theme::font_14);
    wireless_button->setChecked(this->config->get_wireless_active());
    connect(wireless_button, &QCheckBox::toggled, [config = this->config, connection](bool checked) {
        checked ? connection->show() : connection->hide();
        config->set_wireless_active(checked);
    });

    layout->addWidget(label);
    layout->addWidget(connection);
    layout->addWidget(wireless_button, 0, Qt::AlignLeft);

    return widget;
}

QWidget *OpenAutoTab::wireless_config()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    IpInput *ip_input = new IpInput(this->config->get_wireless_address(), QFont("Titillium Web", 24), widget);
    layout->addWidget(ip_input);

    QLabel *status = new QLabel(widget);
    status->setFont(Theme::font_16);
    status->setAlignment(Qt::AlignCenter);
    layout->addWidget(status);

    QPushButton *button = new QPushButton("connect", widget);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setIconSize(Theme::icon_36);
    button->setIcon(this->theme->add_button_icon2("wifi", button));
    connect(button, &QPushButton::clicked, [this, widget, ip_input, status]() {
        status->setText("connecting...");
        widget->setEnabled(false);
        this->worker->connect_wireless(ip_input->active_address());
    });
    layout->addWidget(button, 0, Qt::AlignCenter);

    connect(this->worker, &OpenAutoWorker::wireless_connection_success,
            [this, widget, ip_input, status](QString address) {
                status->setText(QString());
                widget->setEnabled(true);
                this->config->set_wireless_address(address);
            });
    connect(this->worker, &OpenAutoWorker::wireless_connection_failure, [widget, status]() {
        status->setText("connection failed");
        widget->setEnabled(true);
    });

    return widget;
}
