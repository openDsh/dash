# IntelligentAuto

IntelligentAuto is a Qt-based infotainment center for your current Linux OpenAuto installation!

Main features include:

* volume control
* responsive UI (adjusts to any screen size)
* embedded OpenAuto
* bluetooth media control
* read vehicle OBD-II data
* theming (dark/light mode and accent color)
* brightness (simulated)

![home](https://github.com/rsjudka/intelligent-auto/blob/master/docs/imgs/home.png)

![openauto_maps](https://github.com/rsjudka/intelligent-auto/blob/master/docs/imgs/openauto_maps.png)

![openauto_spotify](https://github.com/rsjudka/intelligent-auto/blob/master/docs/imgs/openauto_spotify.png)

![media](https://github.com/rsjudka/intelligent-auto/blob/master/docs/imgs/media.png)

![data](https://github.com/rsjudka/intelligent-auto/blob/master/docs/imgs/data.png)

![settings](https://github.com/rsjudka/intelligent-auto/blob/master/docs/imgs/settings.png)

## Getting Started

### Install.sh automated install (Raspberry Pi only)
First you need to run the executable with the following command:

`./install.sh`

Once installed it will start up, to run afterwards, navigate to bin folder and run:

`./dash`

Video walk through: 

https://youtu.be/CIdEN2JNAzw

## Manual Install

### Prerequisites

The following packages have been used while developing this application (NOTE some things may be missing and others are not actually needed):

* alsa-utils
* cmake
* libboost-all-dev
* libusb-1.0.0-dev
* libssl-dev
* libprotobuf-dev
* protobuf-c-compiler
* protobuf-compiler
* libqt5multimedia5
* libqt5multimedia5-plugins
* libqt5multimediawidgets5
* qtmultimedia5-dev
* libqt5bluetooth5
* libqt5bluetooth5-bin
* qtconnectivity5-dev
* pulseaudio
* librtaudio-dev
* librtaudio6
* libkf5bluezqt-dev
* libtag1-dev

If you plan on using the Qt video library instead of the OMX library (i.e. not using a Raspberry Pi) you'll also most likely want the following packages:

* libgstreamer1.0-0
* gstreamer1.0-plugins-base
* gstreamer1.0-plugins-good
* gstreamer1.0-plugins-bad
* gstreamer1.0-plugins-ugly
* gstreamer1.0-libav
* gstreamer1.0-doc
* gstreamer1.0-tools
* gstreamer1.0-x
* gstreamer1.0-alsa
* gstreamer1.0-gl
* gstreamer1.0-gtk3
* gstreamer1.0-qt5
* gstreamer1.0-pulseaudio

For a Raspberry Pi, you will also need to run the following commands:
```
cd /opt/vc/src/hello_pi/libs/ilclient
make
```

### Building

It is assumed you have cloned this repo with all submodules and are in the root directory.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make
```

If building this for a Raspberry Pi, add `-DRPI_BUILD=TRUE` to the `cmake` command.

### Running

Building dash will create the `dash` binary in `bin/`. Depending on what you're running it on, you may need to make some adjustments to your system.

Some things to consider when configuring your system:

* use a desktop environment that supports transparency (consider using xfce on Raspberry Pi)
* set background color to black and hide any desktop elements (icons, panel, dock, etc.)
* set USB permissions

### Gotchas

Adjusting the brightness does not actually change the screens brightness, it just changes the opacity of the window.

Authentication of bluetooth connections are not handled in the application (i.e. the first time you are connecting a device). To keep things simple, you could install `blueman` which will prompt you for the necessary actions. If you are still having problems, you may need to try manually authenticating the bluetooth connection.

There is currently no option for setting the OBD-II interface. Right now it is assumed you have an adapter connected on `/dev/ttyUSB0`.

Settings are saved periodically every 10 seconds (or anytime the `save` button is clicked).

Not all OpenAuto settings are acessible.

If using GStreamer for your video backend (i.e. not a Raspberry Pi), you may get some black bars around the margins of OpenAuto. I'm still trying to figure out a way for it to ignore the aspect ratio.

### Future Features/Fixes

- [ ] add radio player (UI elements exist, just haven't had anything to interface with yet)
- [ ] support bluetooth OBD-II adapter
- [ ] control screen brightness
- [ ] incorporate wireless OpenAuto
- [ ] add wireless hotspot controls
- [ ] add modular OBD-II data tabs
- [ ] add OBD-II error codes tab
- [ ] add dashcam video tab
- [ ] ignore apsect ratio of OpenAuto for GStreamer backend
- [ ] debug issue for OpenAuto randomly disconnecting
- [ ] add automatic light/dark mode
- [ ] add local media player
- [ ] add audio equalizer
