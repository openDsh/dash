#include <QtWidgets>

#include <app/tabs/data.hpp>
#include <app/tabs/media.hpp>
#include <app/tabs/settings.hpp>
#include <app/window.hpp>

MainWindow::MainWindow()
{
    this->config = Config::get_instance();
    this->setWindowOpacity(this->config->get_brightness() / 255.0);

    this->theme = Theme::get_instance();
    this->theme->set_mode(this->config->get_dark_mode());
    this->theme->set_color(this->config->get_color());

    this->open_auto_tab = new OpenAutoTab(this);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QTabWidget *tabs = new QTabWidget(widget);
    tabs->setTabPosition(QTabWidget::TabPosition::West);
    tabs->tabBar()->setIconSize(Theme::icon_48);
    tabs->addTab(this->open_auto_tab, QString());
    this->theme->add_tab_icon("directions_car", 0, Qt::Orientation::Vertical);
    tabs->addTab(new BluetoothPlayerTab(this), QString());
    this->theme->add_tab_icon("play_circle_outline", 1, Qt::Orientation::Vertical);
    tabs->addTab(new DataTab(this), QString());
    this->theme->add_tab_icon("speed", 2, Qt::Orientation::Vertical);
    tabs->addTab(new SettingsTab(this), "");
    this->theme->add_tab_icon("tune", 3, Qt::Orientation::Vertical);
    connect(this->config, &Config::brightness_changed, [this, tabs](int position) {
        this->setWindowOpacity(position / 255.0);
        if (tabs->currentIndex() == 0) emit set_open_auto_state(position);
    });
    connect(this->theme, &Theme::icons_updated, [tabs](QList<tab_icon_t> &tab_icons, QList<button_icon_t> &button_icons) {
        for (auto &icon : tab_icons) tabs->tabBar()->setTabIcon(icon.first, icon.second);
        for (auto &icon : button_icons) icon.first->setIcon(icon.second);
    });
    connect(tabs, &QTabWidget::currentChanged, [this](int index) {
        emit this->set_open_auto_state((index == 0) ? (windowOpacity() * 255) : 0);
        emit this->set_data_state(index == 2);
    });
    layout->addWidget(tabs);

    layout->addWidget(this->controls_widget());

    setCentralWidget(widget);
}

QWidget *MainWindow::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addStretch(5);
    layout->addWidget(this->volume_widget(), 3);

    return widget;
}

QWidget *MainWindow::volume_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setRange(0, 100);
    slider->setSliderPosition(this->config->get_volume());
    update_system_volume(slider->sliderPosition());
    connect(slider, &QSlider::valueChanged, [config = this->config](int position) {
        config->set_volume(position);
        MainWindow::update_system_volume(position);
    });

    QPushButton *lower_button = new QPushButton(widget);
    lower_button->setFlat(true);
    lower_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_mute", lower_button);
    connect(lower_button, &QPushButton::clicked, [slider]() {
        int position = slider->sliderPosition() - 10;
        slider->setSliderPosition(position);
    });

    QPushButton *raise_button = new QPushButton(widget);
    raise_button->setFlat(true);
    raise_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("volume_up", raise_button);
    connect(raise_button, &QPushButton::clicked, [slider]() {
        int position = slider->sliderPosition() + 10;
        slider->setSliderPosition(position);
    });

    layout->addWidget(lower_button);
    layout->addWidget(slider, 4);
    layout->addWidget(raise_button);

    return widget;
}

void MainWindow::update_system_volume(int position)
{
    QProcess *lProc = new QProcess();
    std::string command = "amixer set Master " + std::to_string(position) + "% --quiet";
    lProc->start(QString(command.c_str()));
    lProc->waitForFinished();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    this->open_auto_tab->start_worker();
    this->open_auto_tab->setFocus();
}
