# OpenDash

OpenDash is a Qt-based infotainment center for your Linux OpenAuto installation!
The OpenDash project includes OpenAuto, AASDK, and Dash.

Main features of Dash include:

*	Embedded OpenAuto `Windowed/Fullscreen`
*	Wireless OpenAuto Capability
*	On-screen Volume, Brightness, & Theme Control
*	Responsive Scalable UI `Adjustable for screen size`
*	Bluetooth Media Control
*	Real-Time Vehicle OBD-II Data & SocketCAN Capabilities
*	Theming `Dark/Light mode` `Customizable RGB Accent Color`
*	True Raspberry Pi 7” Official Touchscreen Brightness Control
*	App-Launcher built in
*	Camera Access `Streaming/Local` `Backup` `Dash`
*	Keyboard Shortcuts `GPIO Triggerable`

![](docs/imgs/opendash-ui.gif)

## Getting Started

Dash is built with the repository flake. The supported entrypoints are:

```sh
nix develop
nix flake check --print-build-logs
nix build .#dash --print-build-logs
```

After a successful package build, the executable is available at:

```sh
./result/bin/dash
```

## Raspberry Pi Image

The flake also defines a Raspberry Pi 4 NixOS image for a head-unit style deployment:

```sh
nix run .#make-images
```

Compressed image artifacts are copied to:

```sh
dist/images/*.img.zst
```

The image target is `aarch64-linux`. On an `x86_64-linux` host, local image
builds need host-level emulation before Nix can start the build. QEMU may come
from Nix or from the host distribution, but `binfmt_misc` registration and Nix's
accepted build platforms are configured on the builder machine.

For a NixOS builder, add this to the host configuration and rebuild the host:

```nix
{
  boot.binfmt.emulatedSystems = [ "aarch64-linux" ];
  nix.settings.extra-platforms = [ "aarch64-linux" ];
}
```

For a non-NixOS Linux builder, register `qemu-aarch64` with `binfmt_misc` using
the host's service manager or container tooling, then add this to `nix.conf` and
restart the Nix daemon:

```conf
extra-platforms = aarch64-linux
```

The local preflight can be checked with:

```sh
nix config show | sed -n '/^extra-platforms =/p'
test -r /proc/sys/fs/binfmt_misc/qemu-aarch64
```

Install QEMU ARM64 Handler (Fedora)

```
sudo dnf install -y qemu-user-static-aarch64
sudo systemctl restart systemd-binfmt.service
```

## Flake Outputs

The initial Nix surface is intentionally small:

- `devShells.default`
- `packages.dash`
- `checks.dash`
- `apps.make-images`
- `nixosConfigurations.pi4-headunit`
