{
  boost,
  cmake,
  elfutils,
  fetchFromGitHub,
  ffmpeg,
  glib,
  gst_all_1,
  gtest,
  h264bitstream,
  lib,
  libusb,
  openssl,
  openDsh_aasdk,
  pcre2,
  pkg-config,
  protobuf3_20,
  qt5,
  qtGStreamer,
  rtaudio,
  stdenv,
}:
{
  openDsh_openauto = stdenv.mkDerivation rec {
    pname = "openDsh_openauto";
    version = "dev-${src.rev}";

    src = fetchFromGitHub {
      owner = "openDsh";
      repo = "openauto";
      rev = "6496f8e360b54e2c22c55edcb0757f46865294dd";
      hash = "sha256-kEqKJMWC5tI2WC45CF1fGJbr15PjGQlVKgp2YXzyRb4=";
    };

    patches = [
      ../../../patches/openauto_find-h264bitstream.patch
      ../../../patches/openauto_rpath-for-nix.patch
    ];

    # This project build is too heavy to parallel on a Pi4-4GB.
    enableParallelBuilding = false;

    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=Release"
      ''-DAASDK_INCLUDE_DIR="${openDsh_aasdk.dev}/include"''
      "-DGST_BUILD=TRUE" # GST_BUILD or # RPI_BUILD
    ];
    nativeBuildInputs = [
      cmake
      pkg-config
      qt5.wrapQtAppsHook
    ];
    buildInputs = [
      openDsh_aasdk
      boost
      libusb
      openssl
      pcre2
      protobuf3_20
      qt5.full
      rtaudio
      # FIXME: Add option to toggle RPi target.
      # OMX IL Headers exist in libraspberrypi
      # ilclient is not pre-compiled there.
      # Not useful for RPi4, only RPi3.
      h264bitstream
    ];

    propagatedBuildInputs = [
      gst_all_1.gstreamer
      gst_all_1.gst-plugins-base
      gst_all_1.gst-plugins-good
      gst_all_1.gst-plugins-bad
      gst_all_1.gst-plugins-ugly
      gst_all_1.gst-libav
      qtGStreamer
    ];

    postInstall = ''
      qtWrapperArgs+=(--prefix GST_PLUGIN_SYSTEM_PATH_1_0 : "$GST_PLUGIN_SYSTEM_PATH_1_0")
      qtWrapperArgs+=(--prefix 'NIXPKGS_QT5_QML_IMPORT_PATH' ':' '${qtGStreamer}/lib/qt5/qml')
    '';

    meta = with lib; {
      homepage = "https://github.com/openDsh/openauto";
      description = "OpenAuto is an AndroidAuto(tm) headunit emulator based on aasdk library and Qt libraries";
      license = licenses.gpl3;
      platforms = with platforms; intersectLists linux (aarch64 ++ x86_64);
      mainProgram = "autoapp";
    };
  };
}
