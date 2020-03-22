#include <app/config.hpp>
#include <app/tabs/open_auto.hpp>
#include <app/theme.hpp>
#include <app/window.hpp>

OpenAutoWorker::OpenAutoWorker(std::function<void(bool)> callback, QWidget *parent)
    : io_service(),
      work(io_service),
      configuration(Config::get_instance()->open_auto_config),
      tcp_wrapper(),
      usb_wrapper((libusb_init(&this->usb_context), usb_context)),
      query_factory(usb_wrapper, io_service),
      query_chain_factory(usb_wrapper, io_service, query_factory),
      service_factory(io_service, configuration, parent, callback),
      android_auto_entity_factory(io_service, configuration, service_factory),
      usb_hub(std::make_shared<aasdk::usb::USBHub>(this->usb_wrapper, this->io_service, this->query_chain_factory)),
      connected_accessories_enumerator(std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(
          this->usb_wrapper, this->io_service, this->query_chain_factory)),
      app(std::make_shared<autoapp::App>(this->io_service, this->usb_wrapper, this->tcp_wrapper,
                                         this->android_auto_entity_factory, usb_hub, connected_accessories_enumerator))
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

OpenAutoTab::OpenAutoTab(QWidget *parent) : QWidget(parent)
{
    MainWindow *window = qobject_cast<MainWindow *>(parent);

    connect(window, &MainWindow::set_open_auto_state, [this](unsigned int alpha) {
        if (this->worker != nullptr) this->worker->set_opacity(alpha);
        if (alpha > 0) this->setFocus();
    });

    QStackedLayout *layout = new QStackedLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->msg_widget());
#ifdef USE_OMX
    QWidget *omx_backdrop = new QWidget(this);
    omx_backdrop->setStyleSheet("background-color: black;");
    layout->addWidget(omx_backdrop);
#endif
}

void OpenAutoTab::start_worker()
{
    QStackedLayout *layout = qobject_cast<QStackedLayout *>(this->layout());
    auto callback = [layout](bool is_active) { layout->setCurrentIndex(is_active ? 1 : 0); };
    if (this->worker == nullptr) this->worker = new OpenAutoWorker(callback, this);

    this->worker->start();
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
