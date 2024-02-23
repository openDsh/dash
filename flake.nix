{
  description = "OpenDash is a Qt-based infotainment center for your Linux OpenAuto installation!";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      inherit (nixpkgs) lib;

      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = f: lib.genAttrs systems (system: f system);

      pkgsWithOverlay = forAllSystems (
        system:
        import nixpkgs {
          localSystem = system;
          overlays = [ self.overlays.openDsh-packages ];
        }
      );
    in
    {
      overlays = import ./nix/overlays.nix { inherit self lib; };

      packages = forAllSystems (
        system: {
          default = self.packages.${system}.openDsh_dash;
          inherit (pkgsWithOverlay.${system})
            h264bitstream
            openDsh_aasdk
            openDsh_dash
            openDsh_openauto
            qtGStreamer
            ;
        }
      );

      formatter = forAllSystems (system: nixpkgs.legacyPackages.${system}.nixfmt-rfc-style);
    };
}
