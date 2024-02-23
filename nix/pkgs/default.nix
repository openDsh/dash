{
  lib,
  pkgs,
  self,
}:
let
  mkDate =
    longDate:
    (lib.concatStringsSep "-" [
      (builtins.substring 0 4 longDate)
      (builtins.substring 4 2 longDate)
      (builtins.substring 6 2 longDate)
    ]);
in
{
  inherit (pkgs.callPackage ./h264bitstream { }) h264bitstream;
  inherit (pkgs.callPackage ./qtgstreamer { }) qtGStreamer;
  inherit (pkgs.callPackage ./aasdk { }) openDsh_aasdk;
  inherit (pkgs.callPackage ./openauto { }) openDsh_openauto;
  inherit (pkgs.callPackage ./dash { date = (mkDate self.lastModifiedDate); }) openDsh_dash;
}
