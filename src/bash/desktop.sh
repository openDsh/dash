#!/bin/bash

###
# Helper script to setup an desktop icon for openDash
###

#remove existing opendash desktop
rm /home/pi/Desktop/opendash

#copy icon to pixmaps folder
sudo cp assets/icons/opendash.xpm /usr/share/pixmaps/opendash.xpm

#copy executable to /usr/local/bin
sudo cp bin/dash /usr/local/bin/dash

#create shortcut on dashboard
echo "
[Desktop Entry]
Name=OpenDash
Comment=Open OpenDash
Icon=/usr/share/pixmaps/opendash.xpm
Exec=/usr/local/bin/dash
Type=Application
Encoding=UTF-8
Terminal=true
Categories=None;
" > /home/pi/Desktop/opendash
