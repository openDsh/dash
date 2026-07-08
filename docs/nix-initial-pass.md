# Initial Nix Conversion Pass

Do not modify this document during implementation.

## Goal

Replace the current install-script/apt build path with a flake-first Nix build, development shell, CI setup, and NixOS Raspberry Pi image pipeline.

This is a clean replacement, not a compatibility layer. The old `install.sh` workflow should be retired once the Nix path can build the project.

## Required Commands

These commands are the public interface for both local development and GitHub Actions:

```sh
nix develop
nix flake check --print-build-logs
nix build .#dash --print-build-logs
nix run .#make-images
```

`nix run .#make-images` should build NixOS Raspberry Pi image artifacts and place them in `dist/images/`.

Keep the flake surface minimal:

- `devShells.default`
- `packages.dash`
- `checks.dash`
- `apps.make-images`
- `nixosConfigurations.pi4-headunit`

Support these systems in the first pass:

- `x86_64-linux` for the development shell, Dash package build, and fast flake checks.
- `aarch64-linux` for the Raspberry Pi 4 NixOS image.

## Nix Direction

- Use `flake.nix` and `flake.lock`.
- Use the latest stable NixOS/Nixpkgs release available when the work starts.
- Do not add legacy entrypoints such as `shell.nix`, `default.nix`, `nix-build`, `nix-shell`, channels, or `nixos-generators`.
- Use current NixOS image outputs through `system.build.images` / `nixos-rebuild build-image`-style outputs. Do not build Raspberry Pi OS, Debian, Ubuntu, or other non-NixOS images.
- Model external source dependencies as flake inputs with `flake = false`, including AASDK and OpenAuto. They should be pinned by `flake.lock`, not cloned at build time.
- Keep CMake as the project build system for now, but remove assumptions like `~/aasdk`, `~/openauto`, global installs, `sudo make install`, and apt-provided dependencies.
- Use QEMU/binfmt for `aarch64-linux` image builds on non-ARM hosts in the first pass. Do not make Nix cross-compilation part of this initial conversion.

## Dependency Direction

- Package the current project stack first.
- AASDK and OpenAuto must be packaged through Nix derivations instead of installed globally.
- Pin AASDK and OpenAuto to exact commits through the flake lock. Start from known-good commits that match the current project, then let normal PRs update them deliberately.
- Do not vendor AASDK or OpenAuto into this repo during the initial pass.
- Existing patches should be applied by the relevant Nix package definitions.
- Keep dependency changes to the minimum needed to make the current project build reproducibly.

## CMake Direction

Keep CMake as the C++ build system, but make it packageable by Nix:

- Support clean out-of-source builds without writing generated files into the repo.
- Remove hardcoded dependency lookup paths such as `~/aasdk` and `~/openauto`.
- Find dependencies through CMake/package-config paths provided by Nix.
- Install the Dash binary and plugin outputs through CMake install rules so Nix can package them from `$out`.
- Avoid configure-time host probing for the target image, such as relying on `/proc/cpuinfo` from the build machine.

## Pi Image Direction

- Target Raspberry Pi 4 first with `aarch64-linux`.
- Build a NixOS headunit image that boots into Dash.
- The image output should be a complete NixOS appliance image, not a package bundle intended to be installed on Raspberry Pi OS.
- Image artifacts should be compressed SD-card-style images, written as `dist/images/*.img.zst`.
- The image should include required graphics, audio, Bluetooth, DBus, udev, USB, and Android Auto permissions.
- Preserve the current broad Android Auto USB access behavior in the first pass. Do not tighten udev rules until there is a hardware test loop that proves Android Auto still launches reliably.
- Use NixOS-provided audio and Bluetooth services in the first pass. Do not custom-build PulseAudio, BlueZ, or ofono unless hardware testing proves the old Raspberry Pi workaround is still required.
- Prefer a minimal appliance-style graphical session, such as Wayland plus Cage, unless hardware testing proves that another stack is required.
- Define a dedicated `dash` runtime user, home/state directory, and service/session startup in the NixOS configuration.
- Do not bake personal Wi-Fi, SSH keys, tokens, or machine-specific secrets into the image.
- Preserve Dash plugin discovery by installing plugins where the app expects them, or by making the plugin path configurable during the CMake cleanup.

## GitHub Actions

Replace the existing apt/install-script workflow with Nix-only workflows.

Required CI should run:

```sh
nix flake check --print-build-logs
nix build .#dash --print-build-logs
```

Image CI should run the same command developers run locally:

```sh
nix run .#make-images
```

Use QEMU/binfmt in GitHub Actions for the Pi image job and upload `dist/images/` as workflow artifacts. Keep the workflow thin: GitHub Actions should install Nix, check out the repo, set up QEMU for image builds, and run the repo-owned Nix commands.

Pin third-party GitHub Actions by full commit SHA instead of floating tags.

Run required CI on pull requests and pushes. Run image CI on `main`, manual dispatch, and pull requests that touch Nix, CMake, GitHub Actions, image, or dependency-packaging files.

Do not add a binary cache in the first pass. Add caching later only if build time becomes a real bottleneck.

## Dependabot

Add `.github/dependabot.yml` for:

- `package-ecosystem: "nix"` at `/`
- `package-ecosystem: "github-actions"` at `/`

Dependabot should keep `flake.lock` and GitHub Actions versions current. If an upstream source branch or tag is hardcoded in `flake.nix`, changing that source selection still requires a normal PR.

For AASDK and OpenAuto, Dependabot updates should be reviewed like dependency upgrades, not auto-merged. These projects are core runtime dependencies and may have API or behavior changes even when the source URL still resolves cleanly.

## Retire Old Build Path

Remove the old supported path once the Nix build is functional:

- Delete `install.sh`.
- Delete or replace helper scripts whose behavior belongs in the NixOS image, including `autostart.sh`, `rpi.sh`, and `uninstall.sh`.
- Delete or replace `.github/workflows/build.yml`.
- Update `README.md` so Nix is the only documented build and install process.
- Move existing patch files into the relevant Nix package definitions or keep them only if the Nix derivations apply them directly.
- Update `.gitignore` for Nix outputs such as `result`, `result-*`, and generated image artifacts under `dist/images/`. Keep `flake.lock` tracked.

Cleanup check:

```sh
rg "install.sh|apt-get|nix-build|nix-shell|nixos-generators"
```

No result should describe a supported build path.

## Acceptance Criteria

- `nix develop` provides the complete development environment.
- `nix flake check --print-build-logs` passes locally and in CI.
- `nix build .#dash --print-build-logs` builds Dash without global installs.
- `nix run .#make-images` produces NixOS Raspberry Pi 4 image artifacts in `dist/images/`.
- GitHub Actions run only the Nix commands documented here.
- Dependabot is configured for Nix and GitHub Actions updates.
- The old apt/install-script process is no longer documented or used.

# Extra

## QEMU
* Add repo docs for local image builds on NixOS and non-NixOS Linux.
* Add a preflight check in make-images that detects missing aarch64-linux support and prints the exact fix instead of failing deep in the build.