#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QDebug>
#include <QStringList>
#include <QtDBus/QtDBus>
#include <QtWidgets>
#include <iostream>
#include <regex>

#include "app/config.hpp"
#include "app/tabs.hpp"
#include "app/theme.hpp"
#include "app/window.hpp"

#include <app/tabs/open_auto.hpp>
#include <app/tabs/media.hpp>
#include <app/tabs/data.hpp>

namespace aasdk = f1x::aasdk;
namespace autoapp = f1x::openauto::autoapp;
using ThreadPool = std::vector<std::thread>;

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent)
{
    QFontDatabase::addApplicationFont(":/Titillium_Web/TitilliumWeb-Regular.ttf");
    QFontDatabase::addApplicationFont(":/Montserrat/Montserrat-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/Montserrat/Montserrat-Regular.ttf");

    Config *config = Config::get_instance();

    QTimer *config_timer = new QTimer;
    connect(config_timer, &QTimer::timeout, [config]() { config->save(); });
    config_timer->start(10000);

    this->brightness = config->get_brightness();
    setWindowOpacity(this->brightness / 255.0);

    this->theme = Theme::get_instance();
    this->theme->set_mode(config->get_dark_mode());
    this->theme->set_color(config->get_color());

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    this->tabs = new QTabWidget();
    this->tabs->setTabPosition(QTabWidget::TabPosition::West);

    OpenAutoTab *open_auto_tab = new OpenAutoTab(this);
    open_auto_tab->setFocus();
    this->tabs->addTab(open_auto_tab, "");
    this->tabs->addTab(new BluetoothPlayerTab(this), "");
    DataTab *data_tab = new DataTab(this);
    this->tabs->addTab(data_tab, "");
    this->tabs->addTab(new QWidget(this), "");
    this->tabs->setTabEnabled(3, false);
    SettingsTab *settings_tab = new SettingsTab(this);
    // should the signal really be from the settings tab
    connect(settings_tab, SIGNAL(brightness_updated(int)), this, SLOT(update_brightness(int)));
    connect(settings_tab, &SettingsTab::si_units_changed, [data_tab](bool si) { data_tab->convert_gauges(si); });
    this->tabs->addTab(settings_tab, "");
    this->tabs->tabBar()->setIconSize(QSize(this->TAB_SIZE, this->TAB_SIZE));

    std::vector<QString> icons = {"directions_car", "play_circle_outline", "speed", "report_problem", "tune"};
    for (unsigned long i = 0; i < icons.size(); i++) this->theme->add_tab_icon(icons[i], i, 90);

    connect(this->theme, SIGNAL(icons_updated(QList<QPair<int, QIcon>> &, QList<QPair<QPushButton *, QIcon>> &)), this,
            SLOT(update_icons(QList<QPair<int, QIcon>> &, QList<QPair<QPushButton *, QIcon>> &)));

    connect(this->tabs, &QTabWidget::currentChanged, [this](int index) {
        emit this->toggle_open_auto((index == 0) ? this->brightness : 0);

        emit this->data_tab_toggle(index == 2);
    });

    QHBoxLayout *bottom = new QHBoxLayout;
    this->volume_control = new QSlider(Qt::Orientation::Horizontal);
    this->volume_control->setRange(0, 100);
    this->volume_control->setSliderPosition(config->get_volume());
    this->update_system_volume(this->volume_control->sliderPosition());

    connect(this->volume_control, &QSlider::valueChanged,
            [this, config](int position) { config->set_volume(position); });

    connect(this->volume_control, &QSlider::sliderMoved, [this](int position) { update_system_volume(position); });

    QLabel *connectivity = new QLabel();
    connectivity->setFixedSize(640 * RESOLUTION, 48 * RESOLUTION);
    bottom->addWidget(connectivity);
    QPushButton *volume_down = new QPushButton();
    volume_down->setFlat(true);
    this->theme->add_button_icon("volume_mute", volume_down);
    volume_down->setIconSize(QSize(36 * RESOLUTION, 36 * RESOLUTION));
    connect(volume_down, &QPushButton::clicked, [this]() {
        int position = this->volume_control->sliderPosition() - 10;
        this->volume_control->setSliderPosition(position);
        update_system_volume(position);
    });
    bottom->addWidget(volume_down);
    bottom->addWidget(this->volume_control);
    QPushButton *volume_up = new QPushButton;
    volume_up->setFlat(true);
    this->theme->add_button_icon("volume_up", volume_up);
    volume_up->setIconSize(QSize(36 * RESOLUTION, 36 * RESOLUTION));
    connect(volume_up, &QPushButton::clicked, [this]() {
        int position = this->volume_control->sliderPosition() + 10;
        this->volume_control->setSliderPosition(position);
        update_system_volume(position);
    });
    bottom->addWidget(volume_up);

    mainLayout->addWidget(tabs);
    mainLayout->addLayout(bottom);
}

void MainWindow::update_icons(QList<QPair<int, QIcon>> &tab_icons, QList<QPair<QPushButton *, QIcon>> &button_icons)
{
    auto tab_bar = this->tabs->tabBar();
    for (auto &icon : tab_icons) tab_bar->setTabIcon(icon.first, icon.second);

    for (auto &icon : button_icons) icon.first->setIcon(icon.second);
}

void MainWindow::start_open_auto()
{
    static OpenAutoTab *open_auto_tab = qobject_cast<OpenAutoTab *>(this->tabs->widget(0));
    open_auto_tab->start_worker();
}

void MainWindow::update_brightness(int position)
{
    this->brightness = position;
    setWindowOpacity(this->brightness / 255.0);
    if (this->tabs->currentIndex() == 0) emit toggle_open_auto(this->brightness);
}

void MainWindow::update_system_volume(int position)
{
    QProcess *lProc = new QProcess();
    std::string command = "amixer set Master " + std::to_string(position) + "% --quiet";
    lProc->start(QString(command.c_str()));
    lProc->waitForFinished();
}

void MainWindow::update_slider_volume()
{
    QProcess *lProc = new QProcess();
    lProc->start("amixer sget Master");
    lProc->waitForFinished();
    std::string lResult = lProc->readAllStandardOutput().toStdString();
    std::regex rgx("\\[(\\d+)%\\]");
    std::smatch match;
    if (std::regex_search(lResult, match, rgx)) this->volume_control->setSliderPosition(std::stoi(match[1]));
}
