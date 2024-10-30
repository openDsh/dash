{
  boost,
  cmake,
  elfutils,
  fetchFromGitHub,
  ffmpeg,
  glib,
  gst_all_1,
  gtest,
  lib,
  libusb,
  openssl,
  protobuf3_20,
  stdenv,
}:
{
  openDsh_aasdk = stdenv.mkDerivation {
    pname = "openDsh_aasdk";
    version = "unstable-2024-02-06";

    src = fetchFromGitHub {
      owner = "openDsh";
      repo = "aasdk";
      rev = "1bc0fe69d5f5f505c978a0c6e32c860e820fa8f6";
      hash = "sha256-Gqd+IHn3G3yU1/SSrp8B+isn1mhsGj2w41oMmSgkpQY=";
    };

    outputs = [
      "dev"
      "out"
    ];

    cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];

    patches = [ ../../../patches/aasdk_openssl-fips-fix.patch ];

    buildInputs = [
      boost
      libusb
      protobuf3_20
      openssl
      gtest
    ];
    nativeBuildInputs = [ cmake ];

    meta = with lib; {
      homepage = "https://github.com/openDsh/aasdk";
      description = "Library containing implementation of core AndroidAuto(tm) functionalities";
      license = licenses.gpl3;
      platforms = with platforms; intersectLists linux (aarch64 ++ x86_64);
    };
  };
}
