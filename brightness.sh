###
# Run this file to setup udev rule for controlling the brightness of a official RPI 7" touch screen
#
# ./brightness.sh
###
FILE=/etc/udev/rules.d/52-dashbrightness.rules
if [[ ! -f "$FILE" ]]; then
    # udev rules to allow write access to all users for Raspberry Pi 7" Touch Screen
    SUBSYSTEM=="backlight",RUN+="/bin/chmod 666 /sys/class/backlight/%k/brightness" | sudo tee $FILE
  if [[ $? -eq 0 ]]; then
      echo -e Permissions created'\n'
    else
      echo -e Unable to create permissions'\n'
  fi
  else
    echo -e Rules exists'\n'
fi
