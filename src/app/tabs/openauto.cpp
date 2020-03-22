#include <app/config.hpp>
#include <app/tabs/openauto.hpp>
#include <app/theme.hpp>
#include <app/window.hpp>

OpenAutoWorker::OpenAutoWorker(std::function<void(bool)> callback, QWidget *parent)
    : io_service(),
      work(io_service),
      configuration(Config::get_instance()->openauto_config),
      tcp_wrapper(),
      usb_wrapper((libusb_init(&usb_context), usb_context)),
      query_factory(usb_wrapper, io_service),
      query_chain_factory(usb_wrapper, io_service, query_factory),
      service_factory(io_service, configuration, parent, callback),
      android_auto_entity_factory(io_service, configuration, service_factory),
      usb_hub(std::make_shared<aasdk::usb::USBHub>(usb_wrapper, io_service, query_chain_factory)),
      connected_accessories_enumerator(
          std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(usb_wrapper, io_service, query_chain_factory)),
      app(std::make_shared<autoapp::App>(io_service, usb_wrapper, tcp_wrapper, android_auto_entity_factory, usb_hub,
                                         connected_accessories_enumerator))
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

OpenAutoTab::OpenAutoTab(QWidget *parent) : QWidget(parent)
{
    MainWindow *window = qobject_cast<MainWindow *>(parent);
    connect(window, &MainWindow::set_openauto_state, [this](unsigned int alpha) {
        if (this->worker != nullptr) this->worker->set_opacity(alpha);
        if (alpha > 0) this->setFocus();
    });

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    OpenAutoFrame *frame = new OpenAutoFrame(this);
    connect(frame, &OpenAutoFrame::toggle, [=](bool enable) {
        if (!enable && frame->is_fullscreen()) {
            window->unset_widget();
            window->remove_widget(frame);
            layout->addWidget(frame);
            layout->setCurrentIndex(1);
            frame->toggle_fullscreen();
            if (this->worker != nullptr) this->worker->resize();
        }
        layout->setCurrentIndex(enable ? 1 : 0);
    });
    connect(frame, &OpenAutoFrame::double_clicked, [=](bool fullscreen) {
        if (fullscreen) {
            layout->setCurrentIndex(0);
            layout->removeWidget(frame);
            window->add_widget(frame);
            window->set_widget();
        }
        else {
            window->unset_widget();
            window->remove_widget(frame);
            layout->addWidget(frame);
            layout->setCurrentIndex(1);
        }
        if (this->worker != nullptr) this->worker->resize();
        frame->setFocus();
    });

    layout->addWidget(this->msg_widget());
    layout->addWidget(frame);

    connect(window, &MainWindow::is_ready, [this, frame, opacity = window->windowOpacity()]() {
        frame->resize(this->size());
        auto callback = [frame](bool is_active) {
            frame->toggle(is_active);
            frame->setFocus();
        };
        if (this->worker == nullptr) this->worker = new OpenAutoWorker(callback, frame);
        this->worker->set_opacity(opacity * 255);

        this->worker->start();
    });
}

QWidget *OpenAutoTab::msg_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *top_msg = new QLabel("waiting for device...", widget);
    top_msg->setFont(Theme::font_16);
    top_msg->setAlignment(Qt::AlignHCenter);
    layout->addStretch();
    layout->addWidget(top_msg);

    QLabel *bottom_msg = new QLabel("plug in your device to start OpenAuto", widget);
    bottom_msg->setFont(Theme::font_16);
    bottom_msg->setAlignment(Qt::AlignHCenter);
    layout->addWidget(bottom_msg);
    layout->addStretch();

    return widget;
}
