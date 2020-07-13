# OpenDash

The IntelligentAuto dashboard is a Qt-based infotainment center for your new Linux OpenAuto installation!

Main features include:

* volume control
* responsive UI (adjusts to any screen size)
* embedded OpenAuto
* bluetooth media control
* read vehicle OBD-II data
* theming (dark/light mode and accent color)
* brightness (simulated)

![home](https://github.com/robert5974/intelligent-auto/blob/master/docs/imgs/home.png)

![openauto_maps](https://github.com/robert5974/intelligent-auto/blob/master/docs/imgs/openauto_maps.png)

![openauto_spotify](https://github.com/robert5974/intelligent-auto/blob/master/docs/imgs/openauto_spotify.png)

![media](https://github.com/robert5974/intelligent-auto/blob/master/docs/imgs/media.png)

![data](https://github.com/robert5974/intelligent-auto/blob/master/docs/imgs/data.png)

![settings](https://github.com/robert5974/intelligent-auto/blob/master/docs/imgs/settings.png)

## Getting Started


## Manual Install

## Prerequisites

The following packages have been used while developing this application.
> **(NOTE some things may be missing and others are not actually needed)**

**1. Install the following packages:**

```
sudo apt update

sudo apt install alsa-utils cmake libboost-all-dev libusb-1.0.0-dev libssl-dev libprotobuf-dev protobuf-c-compiler protobuf-compiler libqt5multimedia5 libqt5multimedia5-plugins libqt5multimediawidgets5qtmultimedia5-dev libqt5bluetooth5 libqt5bluetooth5-bin qtconnectivity5-dev pulseaudio librtaudio-dev librtaudio6 libkf5bluezqt-dev libtag1-dev qt5-default qml-module-qtquick2 qtdeclarative5-dev qtmultimedia5-dev qml-module-qtquick* qml-module-qtmultimedia qml-module-qt-labs-settings qml-module-qt-labs-folderlistmodel libqt5xmlpatterns5-dev qml-module-qtbluetooth libqt5charts5 qml-module-qtcharts build-essential openssl libglib2.0-dev libboost-dev libudev-dev libgstreamer1.0-dev gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-alsa libgstreamer-plugins-base1.0-dev gstreamer1.0-pulseaudio libfaad-dev libfftw3-dev librtlsdr-dev libusb-1.0-0-dev mesa-common-dev libglu1-mesa-dev zlib1g-dev portaudio19-dev libsndfile1-dev libsamplerate0-dev bluez bluez-obexd pulseaudio-module-bluetooth  qml-module-qtbluetooth qml-module-org-kde-bluezqt qtbase5-private-dev libcanberra-dev libgconf2-dev libpulse-dev libmp3lame-dev libsoapysdr-dev libmpg123-dev
```

_If you plan on using the Qt video library instead of the OMX library navigate to the [qt-gstreamer](https://github.com/robert5974/intelligent-auto/wiki/qt-gstreamer) section of the sidebar and follow the instructions, then return to step 2_

Continue following the steps to build the Dashboard.

For a Raspberry Pi, you will also need to run the following commands:
```
cd /opt/vc/src/hello_pi/libs/ilclient
make
```

### Building

## Clone the repo, Run cmake, and Compile
```
git clone https://github.com/openDsh/dash

cd dash

cmake -DRPI_BUILD=TRUE -DGST_BUILD=TRUE .

make
```



**FUTURE ADDITION WHEN WIKI IS BUILT FOR OPENDASH**
Utilize the wiki to follow building instructions.  The Wiki sidebar has the instructions broken into sections with steps to build for Linux and the Raspberry Pi seperately.  There is another section to follow for using qt-gstreamer for graphics-acceleration.

The Wiki explains the tabs of the UI and the features included in each.  There are helpful tips to ensure each user can get the most out the experience.

Make sure to read through the Running section for the particulars, restrictions, and future enhancements.
