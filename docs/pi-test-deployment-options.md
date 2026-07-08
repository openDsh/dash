# Pi Test Deployment Options

This document compares three practical ways to let an agent build, deploy, and
verify the NixOS-based Dash image or system configuration.

The three options are:

1. Local QEMU test
2. `nixos-rebuild` over SSH to a lab Pi
3. Pi auto-update from GitHub

These are complementary. QEMU gives cheap local confidence, SSH deploy gives
fast real-hardware iteration, and GitHub auto-update gives unattended lab
validation.

## Current Baseline

The repository already has:

- `nix run .#make-images` to build a Raspberry Pi 4 NixOS SD image.
- `nixosConfigurations.pi4-headunit` for the Pi image.
- `.github/workflows/images.yml` to build and upload image artifacts.
- A production Pi module at `nix/nixos/pi4-headunit.nix`.

The current production image disables SSH:

```nix
services.openssh.enable = lib.mkDefault false;
```

That is a good default for a locked-down appliance image, but lab deployment and
verification need a separate test profile that enables controlled SSH access.

## Shared Module Layout

To avoid maintaining multiple independent NixOS systems, split the current Pi
configuration into shared behavior plus thin target profiles.

Suggested layout:

```text
nix/nixos/
  headunit-base.nix          # Dash, Cage, users, services, env, packages
  hardware/
    pi4.nix                  # Pi 4 image/boot/hardware specifics
    qemu.nix                 # VM-friendly hardware assumptions
  profiles/
    production.nix           # locked-down defaults
    lab-test.nix             # SSH, debug tools, deploy user
  pi4-headunit.nix           # production Pi composition
  pi4-lab.nix                # lab Pi composition
  qemu-headunit.nix          # local VM composition
```

The final systems would be small compositions:

```nix
nixosConfigurations.pi4-headunit = lib.nixosSystem {
  system = "aarch64-linux";
  modules = [
    ./nix/nixos/headunit-base.nix
    ./nix/nixos/hardware/pi4.nix
    ./nix/nixos/profiles/production.nix
  ];
};

nixosConfigurations.pi4-lab = lib.nixosSystem {
  system = "aarch64-linux";
  modules = [
    ./nix/nixos/headunit-base.nix
    ./nix/nixos/hardware/pi4.nix
    ./nix/nixos/profiles/lab-test.nix
  ];
};

nixosConfigurations.qemu-headunit = lib.nixosSystem {
  system = "aarch64-linux";
  modules = [
    ./nix/nixos/headunit-base.nix
    ./nix/nixos/hardware/qemu.nix
    ./nix/nixos/profiles/lab-test.nix
  ];
};
```

This gives multiple deployable systems without duplicating the actual Dash
runtime configuration.

## Option 1: Local QEMU Test

### What It Is

Run a local VM-style NixOS test that boots a Dash headunit configuration under
QEMU.

This should usually test a VM-oriented NixOS configuration, not the exact
Raspberry Pi SD-card image. The Pi image imports SD-card and Pi-specific boot
modules that are awkward to boot directly under generic QEMU.

### What It Verifies Well

- The NixOS configuration evaluates.
- The Dash package closure builds.
- Users, groups, environment variables, and packages are present.
- systemd units are installed and can be inspected.
- Basic boot to `multi-user.target`.
- Basic app/service wiring.

### What It Does Not Verify Well

- Raspberry Pi firmware boot.
- Real KMS/DRM behavior on Pi hardware.
- Official Pi touchscreen behavior.
- Bluetooth, USB, audio, Android Auto, or CAN hardware behavior.
- Real Cage/Wayland behavior against the Pi display stack.

### Possible Command

```sh
nix run .#test-qemu-headunit
```

or as a check:

```sh
nix flake check
```

### Implementation Shape

Add a QEMU-friendly NixOS test:

```nix
checks.x86_64-linux.qemu-headunit = pkgs.nixosTest {
  name = "dash-headunit-qemu";

  nodes.machine = { ... }: {
    imports = [
      ./nix/nixos/headunit-base.nix
      ./nix/nixos/hardware/qemu.nix
      ./nix/nixos/profiles/lab-test.nix
    ];
  };

  testScript = ''
    machine.start()
    machine.wait_for_unit("multi-user.target")
    machine.succeed("test -x /run/current-system/sw/bin/dash")
    machine.succeed("id dash")
    machine.succeed("systemctl status cage || true")
  '';
};
```

If Cage cannot run cleanly in the VM at first, the initial test can still verify
that the service is present and correctly configured. Real graphical launch can
be handled by the hardware Pi tests.

### Pros

- Fastest feedback loop.
- Works locally and in CI.
- Does not need a physical Pi.
- Good for catching broken NixOS module composition.
- Cheap to run on every PR.

### Cons

- Not a real Pi hardware test.
- May need a slightly different hardware module.
- Graphical validation can be limited unless the VM display stack is carefully
  configured.

## Option 2: `nixos-rebuild` Over SSH To A Lab Pi

### What It Is

Keep a lab Pi already running NixOS with SSH enabled, then have the agent push
the current local flake configuration to it with `nixos-rebuild`.

This avoids flashing SD cards during normal development.

### Possible Commands

Deploy current checkout:

```sh
DASH_PI_HOST=dash@dash-pi-lab.local nix run .#deploy-lab-pi
```

Verify only:

```sh
DASH_PI_HOST=dash@dash-pi-lab.local nix run .#verify-lab-pi
```

Deploy and verify:

```sh
DASH_PI_HOST=dash@dash-pi-lab.local nix run .#deploy-and-verify-lab-pi
```

The wrapper would run something like:

```sh
nixos-rebuild switch \
  --flake .#pi4-lab \
  --target-host "$DASH_PI_HOST" \
  --build-host localhost \
  --use-remote-sudo
```

### Verification Script

The verification app can run checks over SSH:

```sh
ssh "$DASH_PI_HOST" systemctl is-system-running
ssh "$DASH_PI_HOST" systemctl status cage --no-pager
ssh "$DASH_PI_HOST" journalctl -u cage -b --no-pager
ssh "$DASH_PI_HOST" pgrep dash
ssh "$DASH_PI_HOST" test -e /dev/dri/card0
ssh "$DASH_PI_HOST" test -d /dev/input
ssh "$DASH_PI_HOST" test -d /dev/snd
ssh "$DASH_PI_HOST" bluetoothctl show
```

The verification should collect logs even on failure so the agent has useful
evidence to report.

### Lab Profile Requirements

The lab profile should add:

- SSH enabled.
- A deploy user or root SSH policy.
- Passwordless sudo for `nixos-rebuild`, if using `--use-remote-sudo`.
- Debug tools such as `jq`, `usbutils`, `evtest`, `alsa-utils`, and
  `bluez-tools`.
- Optional serial console settings.
- Optional test metadata, such as the deployed Git revision.

Example:

```nix
{
  services.openssh.enable = true;

  users.users.dash.extraGroups = [ "wheel" ];
  security.sudo.wheelNeedsPassword = false;

  environment.systemPackages = [
    pkgs.alsa-utils
    pkgs.bluez-tools
    pkgs.evtest
    pkgs.jq
    pkgs.usbutils
  ];
}
```

### Pros

- Fast real-hardware iteration.
- Tests the actual Pi runtime environment.
- Avoids reflashing SD cards.
- Lets the agent deploy from a local checkout before code is merged.
- Best day-to-day hardware test loop.

### Cons

- Requires a reachable Pi with working SSH.
- A bad switch can break the lab Pi until manual recovery.
- Does not test the full SD-image flashing and first-boot path.
- Needs key management and sudo policy.

## Option 3: Pi Auto-Update From GitHub

### What It Is

Configure the lab Pi to periodically update itself from a GitHub flake using
NixOS `system.autoUpgrade`.

Instead of the agent pushing a local checkout, the Pi follows a branch or ref.
The agent then verifies the resulting generation, services, and logs.

### Possible NixOS Configuration

```nix
{
  system.autoUpgrade = {
    enable = true;
    flake = "github:OpenDsh/dash#pi4-lab";
    dates = "hourly";
    randomizedDelaySec = "5min";
    allowReboot = true;
    flags = [
      "--print-build-logs"
    ];
  };
}
```

For production, do not point devices directly at an unstable branch unless that
is an intentional release policy. A lab Pi can track a branch such as
`lab-auto`, while production devices track a slower `stable` branch or pinned
release.

### Agent Verification Commands

```sh
ssh "$DASH_PI_HOST" systemctl status nixos-upgrade.service --no-pager
ssh "$DASH_PI_HOST" journalctl -u nixos-upgrade.service -b --no-pager
ssh "$DASH_PI_HOST" readlink /run/current-system
ssh "$DASH_PI_HOST" systemctl is-system-running
ssh "$DASH_PI_HOST" systemctl status cage --no-pager
ssh "$DASH_PI_HOST" journalctl -u cage -b --no-pager
ssh "$DASH_PI_HOST" pgrep dash
```

It is useful to write the source revision into the system:

```nix
environment.etc."dash-build-revision".text =
  self.rev or self.dirtyRev or "unknown";
```

Then verification can assert:

```sh
ssh "$DASH_PI_HOST" cat /etc/dash-build-revision
```

### Pros

- Fully unattended lab update path.
- Good for testing what happens after GitHub changes land.
- The agent only needs to verify and report.
- Can be paired with CI branch promotion.

### Cons

- Slower feedback than direct SSH deploy.
- The Pi only sees committed/pushed code.
- A bad update can still break SSH or boot.
- Needs branch/release discipline.
- More difficult to test local uncommitted changes.

## Recommended Order

Implement the options in this order:

1. Split NixOS configuration into shared base, hardware modules, and profiles.
2. Add QEMU smoke test as a cheap local and CI check.
3. Add `deploy-lab-pi` and `verify-lab-pi` flake apps for explicit hardware
   testing.
4. Add a `lab-auto` profile or branch policy for GitHub-driven auto-updates.

## Recommended Daily Workflow

For local confidence:

```sh
nix run .#test-qemu-headunit
```

For fast hardware validation:

```sh
DASH_PI_HOST=dash@dash-pi-lab.local nix run .#deploy-and-verify-lab-pi
```

For unattended validation:

```text
push branch -> CI checks/builds -> lab Pi auto-updates -> agent verifies logs
```

## Summary

Use QEMU for cheap structural confidence, SSH rebuild for fast real-hardware
iteration, and GitHub auto-update for unattended lab validation. The key design
choice is to keep Dash runtime behavior in one shared NixOS module and compose
small hardware/profile modules around it.
