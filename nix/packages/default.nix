{
  lib,
  stdenv,
  cmake,
  ninja,
  pkg-config,
  git,
  boost186,
  libusb1,
  protobuf_21,
  openssl,
  rtaudio,
  taglib,
  qt5,
  gst_all_1,
  glib,
  libGL,
  libx11,
  xrandr,
  kdePackages,
  sources,
  dashSrc,
}:

let
  boostCompat = boost186;
  protobufCompat = protobuf_21;

  gstPlugins = with gst_all_1; [
    gstreamer
    gst-plugins-base
    gst-plugins-good
    gst-plugins-bad
    gst-libav
  ];

  qtInputs = with qt5; [
    qtbase
    qtconnectivity
    qtdeclarative
    qtmultimedia
    qtserialbus
    qtserialport
    qtsvg
    qtwayland
    qtwebsockets
  ];

  boostCmakeFlags = [
    "-DBoost_NO_BOOST_CMAKE=ON"
    "-DBOOST_INCLUDEDIR=${boostCompat.dev}/include"
    "-DBOOST_LIBRARYDIR=${boostCompat.out}/lib"
  ];

  bluez-qt5 = stdenv.mkDerivation {
    pname = "bluez-qt";
    version = "5.116.0";

    src = sources.bluez-qt5;

    nativeBuildInputs = [
      cmake
      kdePackages.extra-cmake-modules
      ninja
      qt5.wrapQtAppsHook
    ];

    buildInputs = [
      qt5.qtbase
    ];

    dontWrapQtApps = true;

    cmakeFlags = [
      "-DBUILD_TESTING=OFF"
    ];
  };

  qt-gstreamer = stdenv.mkDerivation {
    pname = "qt-gstreamer";
    version = "1.2.0-unstable-2024-05-04";

    src = sources.qt-gstreamer;

    patches = [
      ../../patches/greenline_fix.patch
      ../../patches/qt-gstreamer-1.18.patch
      ../../patches/qt-gstreamer_atomic-load.patch
    ];

    nativeBuildInputs = [
      cmake
      ninja
      pkg-config
      qt5.wrapQtAppsHook
    ];

    buildInputs = [
      boostCompat
      boostCompat.out
      glib
      libGL
      qt5.qtbase
      qt5.qtdeclarative
    ]
    ++ gstPlugins;

    dontWrapQtApps = true;

    cmakeFlags = boostCmakeFlags ++ [
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
      "-DQT_VERSION=5"
      "-DQTGSTREAMER_CODEGEN=OFF"
      "-DQTGSTREAMER_EXAMPLES=OFF"
      "-DQTGSTREAMER_TESTS=OFF"
      "-DUSE_GST_PLUGIN_DIR=OFF"
      "-DUSE_QT_PLUGIN_DIR=OFF"
      "-DCMAKE_INSTALL_LIBDIR=lib"
    ];

    postInstall = ''
      for pc in "$out"/lib/pkgconfig/*.pc; do
        sed -i 's|//nix/store/[^/]*/include/Qt5GStreamer|/include/Qt5GStreamer|g' "$pc"
      done
    '';
  };

  h264bitstream = stdenv.mkDerivation {
    pname = "h264bitstream";
    version = "0.2.0-unstable-2024-12-21";

    src = sources.h264bitstream;

    nativeBuildInputs = [
      cmake
      ninja
    ];

    cmakeFlags = [
      "-DBUILD_SHARED_LIBS=ON"
    ];

    postInstall = ''
      rm -f "$out/share/pkgconfig/libh264bitstream.pc"
    '';
  };

  aasdk = stdenv.mkDerivation {
    pname = "aasdk";
    version = "3.1.0-unstable-2026-05-06";

    src = sources.aasdk;

    patches = [
      ../../patches/aasdk_openssl-fips-fix.patch
    ];

    nativeBuildInputs = [
      cmake
      ninja
      protobufCompat
    ];

    buildInputs = [
      boostCompat
      boostCompat.out
      libusb1
      openssl
      protobufCompat
    ];

    cmakeFlags = boostCmakeFlags ++ [
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
      "-DAASDK_TEST=OFF"
    ];
  };

  openauto = stdenv.mkDerivation {
    pname = "openauto";
    version = "2.1.0-unstable-2024-06-18";

    src = sources.openauto;

    nativeBuildInputs = [
      cmake
      ninja
      pkg-config
      protobufCompat
      qt5.wrapQtAppsHook
    ];

    buildInputs = [
      aasdk
      boostCompat
      boostCompat.out
      h264bitstream
      libusb1
      openssl
      protobufCompat
      qt-gstreamer
      rtaudio
    ]
    ++ qtInputs
    ++ gstPlugins;

    postPatch = ''
      cp ${../../cmake_modules/Findaasdk.cmake} cmake_modules/Findaasdk.cmake
      cp ${../cmake/Findh264.cmake} cmake_modules/Findh264.cmake

      substituteInPlace CMakeLists.txt \
        --replace-fail 'set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ''${CMAKE_CURRENT_SOURCE_DIR}/lib)' 'set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ''${CMAKE_BINARY_DIR}/lib)' \
        --replace-fail 'set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ''${CMAKE_CURRENT_SOURCE_DIR}/lib)' 'set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ''${CMAKE_BINARY_DIR}/lib)' \
        --replace-fail 'set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ''${CMAKE_CURRENT_SOURCE_DIR}/bin)' 'set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ''${CMAKE_BINARY_DIR}/bin)' \
        --replace-fail 'set(EXECUTABLE_OUTPUT_PATH ''${CMAKE_CURRENT_SOURCE_DIR}/bin)' 'set(EXECUTABLE_OUTPUT_PATH ''${CMAKE_BINARY_DIR}/bin)' \
        --replace-fail 'install(DIRECTORY lib DESTINATION lib COMPONENT libraries)' '# Libraries are installed by their targets.' \
        --replace-fail 'install(DIRECTORY include DESTINATION include COMPONENT headers)' '# Headers are installed by their targets.' \
        --replace-fail 'install(DIRECTORY bin DESTINATION bin COMPONENT applications)' '# Applications are installed by their targets.'
    '';

    cmakeFlags = boostCmakeFlags ++ [
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
      "-DGST_BUILD=ON"
      "-DRPI_BUILD=OFF"
    ];
  };

  dash = stdenv.mkDerivation {
    pname = "dash";
    version = "0-unstable";

    src = lib.cleanSourceWith {
      src = dashSrc;
      filter =
        path: type:
        let
          base = baseNameOf path;
        in
        !(lib.elem base [
          ".github"
          ".git"
          ".gitignore"
          "README.md"
          "build"
          "bin"
          "docs"
          "flake.lock"
          "flake.nix"
          "lib"
          "nix"
          "result"
          "result-pi4-headunit-image"
          "dist"
        ]);
    };

    nativeBuildInputs = [
      cmake
      git
      ninja
      pkg-config
      protobufCompat
      qt5.wrapQtAppsHook
    ];

    buildInputs = [
      aasdk
      bluez-qt5
      boostCompat
      boostCompat.out
      libusb1
      openauto
      openssl
      protobufCompat
      qt-gstreamer
      rtaudio
      taglib
      libx11
      xrandr
    ]
    ++ qtInputs
    ++ gstPlugins;

    cmakeFlags = boostCmakeFlags ++ [
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
      "-DGST_BUILD=ON"
      "-DRPI_BUILD=OFF"
    ];

    qtWrapperArgs = [
      "--prefix"
      "GST_PLUGIN_SYSTEM_PATH_1_0"
      ":"
      (lib.makeSearchPath "lib/gstreamer-1.0" ([ qt-gstreamer ] ++ gstPlugins))
    ];

    meta.mainProgram = "dash";
  };
in
{
  inherit
    aasdk
    bluez-qt5
    dash
    h264bitstream
    openauto
    qt-gstreamer
    ;
}
