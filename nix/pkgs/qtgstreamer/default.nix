{
  boost,
  cmake,
  elfutils,
  fetchFromGitHub,
  glib,
  gst_all_1,
  lib,
  libunwind,
  orc,
  pcre2,
  pkg-config,
  qt5,
  stdenv,
  zstd,
}:
{
  qtGStreamer = stdenv.mkDerivation rec {
    pname = "qt-gstreamer";
    version = "dev-${src.rev}";

    src = fetchFromGitHub {
      owner = "GStreamer";
      repo = "qt-gstreamer";
      rev = "6e4fb2f3fcfb453c5522c66457ac5ed8c3b1b05c";
      hash = "sha256-4re3LDZS0NNeC2800J4TJ5SsI/b7MtdElwWcOPEYftU=";
    };

    patches = [
      ../../../patches/qt-gstreamer_atomic-load.patch
      ../../../patches/qt-gstreamer_pc_path.patch
      ../../../patches/qt-gstreamer-1.18.patch
      ../../../patches/greenline_fix.patch
    ];

    outputs = [
      "out"
      "dev"
    ];

    cmakeFlags = [
      "-DQT_VERSION=5"
      "-DCMAKE_CXX_FLAGS=-std=c++11"
      "-DUSE_GST_PLUGIN_DIR=OFF"
      "-DUSE_QT_PLUGIN_DIR=OFF"
    ];

    buildInputs = [
      boost
      elfutils
      glib
      gst_all_1.gstreamer
      gst_all_1.gst-plugins-base
      gst_all_1.gst-plugins-good
      gst_all_1.gst-plugins-ugly
      gst_all_1.gst-plugins-bad
      gst_all_1.gst-libav
      libunwind
      orc
      pcre2
      qt5.full
      qt5.qtdeclarative
      zstd
    ];
    nativeBuildInputs = [
      cmake
      pkg-config
      qt5.wrapQtAppsHook
    ];

    meta = with lib; {
      homepage = "https://github.com/aizvorski/h264bitstream";
      description = "A complete set of functions to read and write H.264 video bitstreams, in particular to examine or modify headers.";
      license = licenses.lgpl21;
      platforms = platforms.all;
    };
  };
}
