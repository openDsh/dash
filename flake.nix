{
  description = "OpenDash flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

    aasdk = {
      url = "github:OpenDsh/aasdk/1bc0fe69d5f5f505c978a0c6e32c860e820fa8f6";
      flake = false;
    };

    openauto = {
      url = "github:openDsh/openauto/e7caeb4d49186af867c1d4693c9f3bf9e9ef99e0";
      flake = false;
    };

    h264bitstream = {
      url = "github:aizvorski/h264bitstream/70124d3051ba45e6b326264f0b25e6f48a7479e7";
      flake = false;
    };

    qt-gstreamer = {
      url = "github:GStreamer/qt-gstreamer/6e4fb2f3fcfb453c5522c66457ac5ed8c3b1b05c";
      flake = false;
    };

    bluez-qt5 = {
      url = "https://download.kde.org/stable/frameworks/5.116/bluez-qt-5.116.0.tar.xz";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      aasdk,
      openauto,
      h264bitstream,
      qt-gstreamer,
      bluez-qt5,
    }:
    let
      lib = nixpkgs.lib;
      systems = [ "x86_64-linux" ];
      sources = {
        inherit
          aasdk
          openauto
          h264bitstream
          qt-gstreamer
          bluez-qt5
          ;
      };
      overlays.default = final: prev: {
        dashPackages = final.callPackage ./nix/packages {
          inherit sources;
          dashSrc = self.outPath;
        };
      };
      pkgsFor =
        system:
        import nixpkgs {
          inherit system;
          overlays = [ overlays.default ];
        };
      forAllSystems = lib.genAttrs systems;
    in
    {
      inherit overlays;

      packages = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
        in
        {
          dash = pkgs.dashPackages.dash;
        }
      );

      checks = forAllSystems (system: {
        dash = self.packages.${system}.dash;
      });

      devShells = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ pkgs.dashPackages.dash ];
            packages = [
              pkgs.git
              pkgs.nixfmt-rfc-style
            ];
          };
        }
      );

      apps = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
        in
        {
          make-images = {
            type = "app";
            program = lib.getExe (
              pkgs.writeShellApplication {
                name = "make-images";
                runtimeInputs = [
                  pkgs.coreutils
                  pkgs.findutils
                  pkgs.jq
                  pkgs.nix
                ];
                text = ''
                  set -euo pipefail

                  required_system="aarch64-linux"
                  nix_config="$(nix config show --json)"
                  host_system="$(printf '%s\n' "$nix_config" | jq -r '.system.value')"
                  extra_platforms="$(printf '%s\n' "$nix_config" | jq -r '."extra-platforms".value | join(" ")')"

                  nix_platform_ok="no"
                  if [ "$host_system" = "$required_system" ]; then
                    nix_platform_ok="yes"
                  elif printf '%s\n' "$nix_config" | jq -e --arg system "$required_system" '."extra-platforms".value | index($system)' >/dev/null; then
                    nix_platform_ok="yes"
                  fi

                  binfmt_ok="yes"
                  if [ "$host_system" != "$required_system" ] && [ ! -r /proc/sys/fs/binfmt_misc/qemu-aarch64 ]; then
                    binfmt_ok="no"
                  fi

                  if [ "$nix_platform_ok" != "yes" ] || [ "$binfmt_ok" != "yes" ]; then
                    cat >&2 <<EOF
                  Cannot build the Raspberry Pi image on this host yet.

                  This image targets $required_system, but the local host is $host_system.

                  Current host status:
                    Nix extra-platforms: ''${extra_platforms:-<empty>}
                    qemu-aarch64 binfmt: $binfmt_ok

                  On a NixOS builder, add this to the host configuration:
                    boot.binfmt.emulatedSystems = [ "$required_system" ];
                    nix.settings.extra-platforms = [ "$required_system" ];

                  On non-NixOS Linux, register qemu-aarch64 with binfmt_misc using the host's
                  service manager or container tooling, then add this to nix.conf and restart
                  the Nix daemon:
                    extra-platforms = $required_system
                  EOF
                    exit 1
                  fi

                  out_link="result-pi4-headunit-image"
                  nix build .#nixosConfigurations.pi4-headunit.config.system.build.sdImage \
                    --print-build-logs \
                    --out-link "$out_link"

                  mkdir -p dist/images
                  find "$out_link/sd-image" -type f -name '*.img.zst' \
                    -exec cp -f '{}' dist/images/ ';'
                  find dist/images -maxdepth 1 -type f -name '*.img.zst' -print
                '';
              }
            );
            meta.description = "Build the Raspberry Pi 4 NixOS head-unit image into dist/images";
          };
        }
      );

      nixosConfigurations.pi4-headunit = lib.nixosSystem {
        system = "aarch64-linux";
        modules = [
          ({ ... }: {
            nixpkgs.overlays = [ overlays.default ];
          })
          ./nix/nixos/pi4-headunit.nix
        ];
      };
    };
}
