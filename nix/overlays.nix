{ lib, self }:
{
  default = lib.composeManyExtensions (with self.overlays; [ openDsh-packages ]);

  openDsh-packages = lib.composeManyExtensions [
    (
      final: prev:
      let
        packages = final.callPackage ./pkgs { inherit self; };
      in
      {
        inherit (packages)
          openDsh_aasdk
          openDsh_dash
          openDsh_openauto
          h264bitstream
          qtGStreamer
          ;
      }
    )
  ];
}
