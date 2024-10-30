{

  boost,
  cmake,
  gst_all_1,
  lib,
  libsForQt5,
  libusb,
  mesa,
  openDsh_aasdk,
  openDsh_openauto,
  openssl,
  pkg-config,
  protobuf3_20,
  qt5,
  qtGStreamer,
  rtaudio,
  stdenv,
  taglib,
  writeScriptBin,
  date,
}:
rec {
  openDsh_dash = stdenv.mkDerivation {
    pname = "openDsh_dash";
    version = "${date}";
    src =
      let
        sourceFilter =
          name: type:
          let
            baseName = baseNameOf (toString name);
          in
          !(
            (baseName == "patches" && type == "directory")
            || (lib.hasSuffix ".nix" baseName)
            || baseName == "flake.lock"
            || baseName == "install.sh"
            || baseName == "rpi.sh"
          );
      in
      lib.cleanSourceWith {
        filter = sourceFilter;
        src = lib.cleanSource ../../../.;
      };

    # This project may consume a significant amount of memory during compilation.
    enableParallelBuilding = false;

    cmakeFlags = [ "-DGST_BUILD=TRUE" ];

    postInstall = ''
      qtWrapperArgs+=(--prefix GST_PLUGIN_SYSTEM_PATH_1_0 : "$GST_PLUGIN_SYSTEM_PATH_1_0")
      qtWrapperArgs+=(--prefix 'NIXPKGS_QT5_QML_IMPORT_PATH' ':' '${qtGStreamer}/lib/qt5/qml')
    '';

    nativeBuildInputs = [
      cmake
      pkg-config
      qt5.wrapQtAppsHook
    ];
    buildInputs = [
      boost
      libusb
      qt5.full
      qt5.qtserialbus
      libsForQt5.bluez-qt
      protobuf3_20
      openssl
      rtaudio
      taglib
      openDsh_aasdk
      openDsh_openauto
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

    meta = with lib; {
      homepage = "https://github.com/openDsh/dash";
      description = "A Qt-based infotainment center for your Linux OpenAuto installation!";
      license = licenses.gpl3;
      platforms = with platforms; intersectLists linux (aarch64 ++ x86_64);
      mainProgram = "dash";
    };
  };

  openDsh_dash_nonnixos = writeScriptBin "dash" ''
    # Wrapper for dash for starting dash on Non-NixOS systems, which lack
    # GPU drivers existing at /run/opengl-driver.
    #
    # Dash requires OpenGL drivers or "White Screen" will occur.
    # "qt.qpa.wayland: EGL not available"
    #
    # This solution is a hack, but is the best hack for the situation.  The
    # alternative is to use nixGL, but that will incur a runtime pentalty as
    # a nix evaluation must be done and may require hundreds of megabytes to
    # be downloaded at launch time.
    #
    # Nvidia support is ignored, if that is needed, use nixGL without this wrapper.
    #
    # If subprocesses are launched and have an issue with the GPU drivers, the user
    # should configure the system to create a symlink to the mesa.drivers output
    # at /run/opengl-driver and stop using this wrapper.

    export LIBGL_DRIVERS_PATH=${mesa.drivers}/lib/dri
    export LD_LIBRARY_PATH=${mesa.drivers}/lib
    exec ${openDsh_dash}/bin/dash "$@"
  '';
}
