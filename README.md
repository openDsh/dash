# IntelligentAuto

IntelligentAuto is a Qt-based infotainment center for your current Linux OpenAuto installation!

Main features include:

* volume control
* responsive UI
* embedded OpenAuto
* bluetooth media control
* read vehicle OBD-II data
* theming (dark/light mode and accent color)
* brightness (simulated)

![home]
(https://github.com/rsjudka/intelligent-auto/tree/master/docs/imgs/home.png)

![openauto_maps]
(https://github.com/rsjudka/intelligent-auto/tree/master/docs/imgs/openauto_maps.png)

![openauto_spotify]
(https://github.com/rsjudka/intelligent-auto/tree/master/docs/imgs/openauto_spotify.png)

![media]
(https://github.com/rsjudka/intelligent-auto/tree/master/docs/imgs/media.png)

![data]
(https://github.com/rsjudka/intelligent-auto/tree/master/docs/imgs/data.png)

![settings]
(https://github.com/rsjudka/intelligent-auto/tree/master/docs/imgs/settings.png)

## Getting Started

The build procedure is currently a bit tediuos because it requires multiple libraries to be be built. In future versions, I plan on consolidating everything into a single build procedure.

### Prerequisites

To make things easier, I'm going to include the instrutions for building everything (aasdk, OpenAuto, dash) here.

The following packages are required:

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

For example, you can install all the packages like this:

```
sudo apt-get install -y alsa-utils cmake libboost-all-dev libusb-1.0.0-dev libssl-dev libprotobuf-dev protobuf-c-compiler protobuf-compiler libqt5multimedia5 libqt5multimedia5-plugins libqt5multimediawidgets5 qtmultimedia5-dev libqt5bluetooth5 libqt5bluetooth5-bin qtconnectivity5-dev pulseaudio librtaudio-dev librtaudio6 libkf5bluezqt-dev libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
```

### Building

It is assumed you have cloned this repo with all submodules and are in its root directory at the beginning of each build.

#### aasdk

```
mkdir aasdk_build
cd aasdk_build
cmake -DCMAKE_BUILD_TYPE=Release ../aasdk
make
```

#### OpenAuto

```
mkdir openauto_build
cd openauto_build
cmake -DCMAKE_BUILD_TYPE=Release -DAASDK_INCLUDE_DIRS="`pwd`/../aasdk/include" -DAASDK_LIBRARIES="`pwd`/../aasdk/lib/libaasdk.so" -DAASDK_PROTO_INCLUDE_DIRS="`pwd`/../aasdk_build" -DAASDK_PROTO_LIBRARIES="`pwd`/../aasdk/lib/libaasdk_proto.so" ../openauto
make
```

If building this for a Raspberry Pi, add `-DRPI_BUILD=TRUE` to the `cmake` command.

#### dash

```
mkdir dash_build
cd dash_build
cmake -DCMAKE_BUILD_TYPE=Release -DAASDK_INCLUDE_DIRS="`pwd`/../aasdk/include" -DAASDK_LIBRARIES="`pwd`/../aasdk/lib/libaasdk.so" -DAASDK_PROTO_INCLUDE_DIRS="`pwd`/../aasdk_build" -DAASDK_PROTO_LIBRARIES="`pwd`/../aasdk/lib/libaasdk_proto.so" -DOPENAUTO_INCLUDE_DIRS="`pwd`/../openauto/include" -DOPENAUTO_LIBRARIES="`pwd`/../openauto/lib/libauto.so" ../dash
make
```

If building this for a Raspberry Pi, add `-DRPI_BUILD=TRUE` to the `cmake` command.

### Running

Building dash will create the `dash` binary in `dash/bin`. Depending on what you're running it on, you may need to make some adjustments to your system.

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

### Future Features

- [ ] add radio player (UI elements exist, just haven't had anything to interface with yet)
- [ ] support bluetooth OBD-II adapter
- [ ] control screen brightness
- [ ] incorporate wireless OpenAuto
- [ ] wireless hotspot controls
- [ ] add modular OBD-II data tabs
- [ ] add OBD-II error codes tab
- [ ] add dashcam video tab
