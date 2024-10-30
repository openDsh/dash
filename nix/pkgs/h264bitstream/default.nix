{
  cmake,
  fetchFromGitHub,
  ffmpeg,
  lib,
  stdenv,
}:
{
  h264bitstream = stdenv.mkDerivation rec {
    pname = "h264bitstream";
    version = "dev-${src.rev}";

    src = fetchFromGitHub {
      owner = "aizvorski";
      repo = "h264bitstream";
      rev = "ae72f7395f328876199a7e928d3b4a6dc6a7ce14";
      hash = "sha256-V96hO97PNxq7FHhr/Sk3cAvtXD30MO4LlL9M/OZ3H18=";
    };
    patches = [ ../../../patches/h264bitstream_path_patch_fixup.patch ];

    outputs = [
      "out"
      "dev"
    ];
    buildInputs = [ ffmpeg ];
    nativeBuildInputs = [ cmake ];
    postInstall = ''
      install -d $out/lib/pkgconfig/
      install libh264bitstream.pc $out/lib/pkgconfig/libh264bitstream.pc
    '';

    meta = with lib; {
      homepage = "https://github.com/aizvorski/h264bitstream";
      description = "A complete set of functions to read and write H.264 video bitstreams, in particular to examine or modify headers.";
      license = licenses.lgpl21;
      platforms = with platforms; intersectLists linux (aarch64 ++ x86_64);
    };
  };
}
