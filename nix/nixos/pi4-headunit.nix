{
  config,
  lib,
  modulesPath,
  pkgs,
  ...
}:

{
  imports = [
    "${modulesPath}/installer/sd-card/sd-image-aarch64.nix"
  ];

  image.baseName = "dash-pi4-headunit-${config.system.nixos.label}-aarch64-linux";

  boot.zfs.forceImportRoot = false;

  networking.hostName = "dash-headunit";
  networking.networkmanager.enable = true;
  time.timeZone = "UTC";

  users.users.root.hashedPassword = "!";
  users.groups.dash = { };
  users.groups.plugdev = { };
  users.users.dash = {
    isSystemUser = true;
    group = "dash";
    home = "/var/lib/dash";
    createHome = true;
    extraGroups = [
      "audio"
      "input"
      "plugdev"
      "video"
    ];
  };

  systemd.tmpfiles.rules = [
    "d /var/lib/dash 0750 dash dash -"
  ];

  services.dbus.enable = true;
  security.polkit.enable = true;

  hardware.bluetooth.enable = true;
  services.pipewire = {
    enable = true;
    alsa.enable = true;
    pulse.enable = true;
  };

  hardware.graphics.enable = true;

  services.udev.extraRules = ''
    SUBSYSTEM=="usb", ATTR{idVendor}=="*", ATTR{idProduct}=="*", MODE="0660", GROUP="plugdev"
    SUBSYSTEM=="backlight", RUN+="${pkgs.coreutils}/bin/chmod 0660 /sys/class/backlight/%k/brightness", GROUP="video"
  '';

  services.cage = {
    enable = true;
    user = "dash";
    program = "${pkgs.dashPackages.dash}/bin/dash";
    environment = {
      QT_QPA_PLATFORM = "wayland";
      XDG_STATE_HOME = "/var/lib/dash/.local/state";
      XDG_CACHE_HOME = "/var/lib/dash/.cache";
      XDG_CONFIG_HOME = "/var/lib/dash/.config";
    };
  };

  environment.systemPackages = [
    pkgs.alsa-utils
    pkgs.bluez
    pkgs.bluez-tools
    pkgs.dashPackages.dash
    pkgs.usbutils
  ];

  services.openssh.enable = lib.mkDefault false;
  system.stateVersion = "26.05";
}
