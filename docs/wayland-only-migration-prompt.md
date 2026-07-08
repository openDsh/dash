# Wayland-Only Migration Prompt

Use this prompt for an agent tasked with removing X11 support from Dash and making the project Wayland-only.

## Goal

Deprecate and remove all X11-specific build inputs, runtime tools, source includes, and plugin implementations from Dash. The target runtime is a Qt application launched as a Wayland client, with the Raspberry Pi NixOS headunit image running Dash under Cage.

## Current X11 Touchpoints

Search the tree for X11 references before making changes:

```sh
rg -n "x11|X11|wayland|Wayland|xorg|Xorg|xcb|libX|xrandr|xinput" .
```

Known X11-specific areas at the time this document was written:

- `CMakeLists.txt`
  - Has `find_package(X11 REQUIRED)`.
- `plugins/CMakeLists.txt`
  - Adds `${X11_INCLUDE_DIR}` and `${X11_LIBRARIES}` to plugin builds.
- `plugins/launcher/app`
  - Includes `<X11/Xatom.h>` and `<X11/Xlib.h>`.
  - Uses `_NET_CLIENT_LIST`, `_NET_WM_PID`, `XOpenDisplay`, `XGetWindowProperty`, and `QWindow::fromWinId` to find and embed external X11 windows.
- `plugins/brightness/x`
  - Uses `xrandr --version`.
  - Uses `xrandr --output ... --brightness ...`.
- `nix/packages/default.nix`
  - Accepts and uses `libx11` and `xrandr`.
- `nix/nixos/pi4-headunit.nix`
  - Already points in the desired direction by using Cage and setting `QT_QPA_PLATFORM = "wayland"`.

## Desired End State

- Dash builds without X11 development headers.
- Dash runtime closure for the Wayland headunit target does not include `xrandr`.
- The Pi headunit module continues to launch Dash with Cage.
- Qt Wayland support remains present through `qtwayland`.
- The app does not require XWayland.
- X11-specific plugins are removed, disabled, or replaced by Wayland-compatible behavior.
- The general Qt plugin system remains intact.

## Implementation Guidance

### 1. Preserve the Main Wayland Launch Path

Keep the main application startup path intact:

- `src/dash.cpp` creates `QApplication`, creates `MainWindow`, and runs `dash.exec()`.
- `nix/nixos/pi4-headunit.nix` should continue to use `services.cage.program = ".../bin/dash"`.
- Keep `QT_QPA_PLATFORM = "wayland"` for the Cage target unless there is a specific reason to rely on Qt auto-detection.

### 2. Remove or Disable X11 Brightness

Deprecate `plugins/brightness/x` because it relies on `xrandr`.

For the Raspberry Pi target, prefer `plugins/brightness/official_rpi`, which writes to:

```text
/sys/class/backlight/rpi_backlight/brightness
```

That path is below the display-server layer and does not depend on X11 or Wayland.

If a generic non-RPi Wayland brightness backend is needed, do not use `xrandr`. Consider a backend based on kernel backlight sysfs, DDC/CI, compositor-specific protocols, or a no-op/mocked fallback depending on the target hardware.

### 3. Decide the Fate of `plugins/launcher/app`

This is the main architectural decision.

The current plugin launches arbitrary external GUI apps and embeds their windows into Dash using X11 APIs. Native Wayland intentionally does not allow normal clients to enumerate and embed other clients' windows in the same way.

Pick one of these directions:

- Remove the plugin from Wayland-only builds.
- Keep a launcher that starts external apps but does not embed their windows.
- Replace the feature with Dash-native widgets/plugins.
- Move app/window composition responsibility into a custom compositor or shell if embedding external Wayland clients is truly required.

Do not attempt a direct one-for-one port of the X11 window embedding code to Wayland client APIs. That is not how Wayland is designed.

### 4. Clean Up CMake

Remove global X11 requirements.

Expected changes:

- Remove `find_package(X11 REQUIRED)` from the top-level build when building Wayland-only.
- Remove `${X11_INCLUDE_DIR}` and `${X11_LIBRARIES}` from generic plugin include/link lists.
- Stop building `plugins/brightness/x` and `plugins/launcher/app` if they remain X11-only.

Prefer a clean explicit plugin allowlist or denylist over building every plugin directory unconditionally. The current `plugins/CMakeLists.txt` discovers and builds all plugin directories automatically, which makes platform-specific plugin selection awkward.

### 5. Clean Up Nix

Update `nix/packages/default.nix` after the CMake changes:

- Remove `libx11` from function arguments and `buildInputs` if no longer needed.
- Remove `xrandr` from function arguments and `buildInputs`.
- Keep `qt5.qtwayland` in the Qt inputs.
- Confirm the wrapped Dash binary has the needed Qt, GStreamer, and Wayland runtime dependencies.

For the Pi image:

- Keep `services.cage.enable = true`.
- Keep the Dash user in groups needed for audio, input, video, USB, and backlight access.
- Keep or refine the udev rule that grants access to `/sys/class/backlight/.../brightness`.

### 6. Verify

Run static checks first:

```sh
rg -n "x11|X11|xorg|Xorg|xcb|libX|xrandr|xinput|XOpenDisplay|XGetWindowProperty|fromWinId" .
```

Expected result: no remaining X11-specific source or packaging references, except historical docs or deliberate migration notes.

Then build:

```sh
nix build .#dashPackages.dash
```

If the Pi image is in scope:

```sh
nix build .#nixosConfigurations.pi4-headunit.config.system.build.sdImage
```

Also test a local non-Nix CMake build if the project still supports it.

## Acceptance Criteria

- `nix build .#dashPackages.dash` succeeds without `libx11` or `xrandr` as package inputs.
- The Pi headunit image still configures Dash as a Cage/Wayland application.
- No production C++ source includes X11 headers.
- No production C++ source shells out to `xrandr`.
- The removed or changed launcher behavior is documented in `plugins/README.md` or another appropriate user-facing project doc.
- Settings UI does not offer a brightness backend that cannot work under Wayland-only builds.
- The general Qt plugin loading mechanism still works for supported plugin categories.

## Important Constraint

Do not remove the Qt plugin architecture just because one plugin is X11-specific. The plugin system itself is Qt-based and is compatible with Wayland. Only X11-specific plugin implementations and build inputs should be removed or replaced.
