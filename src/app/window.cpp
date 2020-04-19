#include <QElapsedTimer>
#include <QtWidgets>
#include <cstdlib>
#include <sstream>

#include <app/tabs/data.hpp>
#include <app/tabs/media.hpp>
#include <app/tabs/launcher.hpp>
#include <app/tabs/settings.hpp>
#include <app/window.hpp>

MainWindow::MainWindow()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->config = Config::get_instance();
    this->setWindowOpacity(this->config->get_brightness() / 255.0);

    this->theme = Theme::get_instance();
    this->theme->set_mode(this->config->get_dark_mode());
    this->theme->set_color(this->config->get_color());

    QFrame *widget = new QFrame(this);
    this->layout = new QStackedLayout(widget);

    this->layout->addWidget(this->window_widget());

    setCentralWidget(widget);
}

QWidget *MainWindow::window_widget()
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(this->tabs_widget());
    layout->addWidget(this->controls_widget());

    return widget;
}

QTabWidget *MainWindow::tabs_widget()
{
    QTabWidget *widget = new QTabWidget(this);
    widget->setTabPosition(QTabWidget::TabPosition::West);
    widget->tabBar()->setIconSize(this->TAB_SIZE);

    widget->addTab(new OpenAutoTab(this), QString());
    this->theme->add_tab_icon("directions_car", 0, Qt::Orientation::Vertical);

    widget->addTab(new MediaTab(this), QString());
    this->theme->add_tab_icon("play_circle_outline", 1, Qt::Orientation::Vertical);

    widget->addTab(new DataTab(this), QString());
    this->theme->add_tab_icon("speed", 2, Qt::Orientation::Vertical);

    widget->addTab(new LauncherTab(this), "");
    this->theme->add_tab_icon("widgets", 3, Qt::Orientation::Vertical);

    widget->addTab(new SettingsTab(this), "");
    this->theme->add_tab_icon("tune", 4, Qt::Orientation::Vertical);

    connect(this->config, &Config::brightness_changed, [this, widget](int position) {
        this->setWindowOpacity(position / 255.0);
        if (widget->currentIndex() == 0) emit set_openauto_state(position);
    });
    connect(this->theme, &Theme::icons_updated,
            [widget](QList<tab_icon_t> &tab_icons, QList<button_icon_t> &button_icons) {
                for (auto &icon : tab_icons) widget->tabBar()->setTabIcon(icon.first, icon.second);
                for (auto &icon : button_icons) icon.first->setIcon(icon.second);
            });
    connect(widget, &QTabWidget::currentChanged,
            [this](int index) { emit set_openauto_state((index == 0) ? (this->windowOpacity() * 255) : 0); });

    return widget;
}

QWidget *MainWindow::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *tab_spacer = new QWidget(this);
    tab_spacer->setFixedWidth(this->TAB_SIZE.width());

    QPushButton *save_button = new QPushButton(widget);
    save_button->setFlat(true);
    save_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("save", save_button);
    connect(save_button, &QPushButton::clicked, [config = this->config]() { config->save(); });

    QPushButton *shutdown_button = new QPushButton(widget);
    shutdown_button->setFlat(true);
    shutdown_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("power_settings_new", shutdown_button);

    QPushButton *exit_button = new QPushButton(widget);
    exit_button->setFlat(true);
    exit_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("close", exit_button);
    connect(exit_button, &QPushButton::clicked, []() { qApp->exit(); });

    QElapsedTimer *timer = new QElapsedTimer();
    connect(shutdown_button, &QPushButton::pressed, [timer]() { timer->start(); });
    connect(shutdown_button, &QPushButton::released, [timer]() {
        sync();

        std::stringstream cmd;
        cmd << "shutdown -" << (timer->hasExpired(2000) ? 'r' : 'h') << " now";
        if (system(cmd.str().c_str()) < 0) qApp->exit();
    });

    layout->addWidget(tab_spacer);
    layout->addWidget(this->volume_widget());
    layout->addStretch();
    layout->addWidget(save_button);
    layout->addWidget(shutdown_button);
    layout->addWidget(exit_button);

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
    this->theme->add_button_icon("volume_down", lower_button);
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
    emit is_ready();
}
