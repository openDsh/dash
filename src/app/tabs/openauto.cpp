#include "app/config.hpp"
#include "app/tabs/openauto.hpp"
#include "app/theme.hpp"
#include "app/widgets/ip_input.hpp"
#include "app/widgets/progress.hpp"
#include "app/window.hpp"

OpenAutoWorker::OpenAutoWorker(std::function<void(bool)> callback, QWidget *parent, bool night_mode)
    : QObject(qApp),
      io_service(),
      work(io_service),
      configuration(Config::get_instance()->openauto_config),
      tcp_wrapper(),
      usb_wrapper((libusb_init(&usb_context), usb_context)),
      query_factory(usb_wrapper, io_service),
      query_chain_factory(usb_wrapper, io_service, query_factory),
      service_factory(io_service, configuration, parent, callback, night_mode),
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
    auto worker = [this]() {
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
    auto worker = [this]() { this->io_service.run(); };

    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
    this->thread_pool.emplace_back(worker);
}

OpenAutoFrame::OpenAutoFrame(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *frame = new QWidget(this);
    frame->setStyleSheet("background-color: rgb(0, 0, 0);");
    layout->addWidget(frame);
}

void OpenAutoFrame::mouseDoubleClickEvent(QMouseEvent *)
{
    this->toggle_fullscreen();
    emit double_clicked(this->fullscreen);
}

// OpenAutoTab::OpenAutoTab(QWidget *parent) : QWidget(parent)
// {
//     this->config = Config::get_instance();

//     this->theme = Theme::get_instance();
//     connect(this->theme, &Theme::mode_updated, [this](bool mode) {
//         if (this->worker != nullptr) this->worker->set_night_mode(mode);
//     });

//     OpenAutoFrame *frame = new OpenAutoFrame(this);
//     MainWindow *window = qobject_cast<MainWindow *>(parent);
//     connect(window, &MainWindow::set_openauto_state, [this, frame](unsigned int alpha) {
//         if (this->worker != nullptr) {
//             this->worker->set_opacity(alpha);
//             this->worker->resize();
//         }
//         if (alpha > 0) frame->setFocus();
//     });

//     QStackedLayout *layout = new QStackedLayout(this);
//     layout->setContentsMargins(0, 0, 0, 0);

//     connect(frame, &OpenAutoFrame::toggle, [=](bool enable) {
//         if (!enable && frame->is_fullscreen()) {
//             window->unset_widget();
//             window->remove_widget(frame);
//             layout->addWidget(frame);
//             layout->setCurrentIndex(1);
//             frame->toggle_fullscreen();
//             if (this->worker != nullptr) this->worker->resize();
//         }
//         layout->setCurrentIndex(enable ? 1 : 0);
//     });
//     connect(frame, &OpenAutoFrame::double_clicked, [=](bool fullscreen) {
//         if (fullscreen) {
//             layout->setCurrentIndex(0);
//             layout->removeWidget(frame);
//             window->add_widget(frame);
//             window->set_widget();
//         }
//         else {
//             window->unset_widget();
//             window->remove_widget(frame);
//             layout->addWidget(frame);
//             layout->setCurrentIndex(1);
//         }
//         if (this->worker != nullptr) this->worker->resize();
//         frame->setFocus();
//     });

//     connect(window, &MainWindow::is_ready, [this, layout, frame]() {
//         frame->resize(this->size());
//         auto callback = [frame](bool is_active) {
//             frame->toggle(is_active);
//             frame->setFocus();
//         };
//         if (this->worker == nullptr) this->worker = new OpenAutoWorker(callback, frame, this->theme->get_mode());
//         BrightnessModule *module = this->config->get_brightness_module();
//         if (module->update_androidauto())
//             this->worker->set_opacity(this->config->get_brightness());

//         layout->addWidget(this->msg_widget());
//         layout->addWidget(frame);

//         this->worker->start();
//     });
// }

OpenAutoTab::OpenAutoTab(QWidget *parent) : QWidget(parent)
{
    this->config = Config::get_instance();
    this->theme = Theme::get_instance();
    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->msg_widget());
}

QWidget *OpenAutoTab::msg_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel("connect device to start Android Auto", widget);
    label->setFont(Theme::font_16);
    label->setAlignment(Qt::AlignCenter);

    QWidget *connection = this->wireless_widget();
    if (!this->config->get_wireless_active()) connection->hide();

    QCheckBox *wireless_button = new QCheckBox("Wireless", widget);
    wireless_button->setFont(Theme::font_14);
    wireless_button->setChecked(this->config->get_wireless_active());
    connect(wireless_button, &QCheckBox::toggled, [config = this->config, connection](bool checked) {
        checked ? connection->show() : connection->hide();
        config->set_wireless_active(checked);
    });

    layout->addStretch();
    layout->addWidget(label, 1);
    layout->addWidget(connection, 1);
    layout->addStretch();
    layout->addWidget(wireless_button, 1, Qt::AlignLeft);

    return widget;
}

QWidget *OpenAutoTab::wireless_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    IpInput *ip_input = new IpInput(this->config->get_wireless_address(), QFont("Titillium Web", 24), widget);
    layout->addWidget(ip_input);

    QPushButton *button = new QPushButton("connect", widget);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button->setFont(Theme::font_14);
    button->setFlat(true);
    button->setIconSize(Theme::icon_36);
    Theme::get_instance()->add_button_icon("wifi", button);
    connect(button, &QPushButton::clicked, [this, widget, ip_input]() {
        widget->setEnabled(false);
        this->worker->connect_wireless(ip_input->active_address());
    });
    layout->addWidget(button, 0, Qt::AlignCenter);

    // connect(this->worker, &OpenAutoWorker::wireless_connection_success, [this, widget, ip_input](QString address) {
    //     widget->setEnabled(true);
    //     this->config->set_wireless_address(address);
    // });
    // connect(this->worker, &OpenAutoWorker::wireless_connection_failure, [widget]() { widget->setEnabled(true); });

    return widget;
}
