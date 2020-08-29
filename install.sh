#!/bin/bash

#Array of dependencies any new dependencies can be added here
deps=(
"alsa-utils"
"cmake"
"libboost-all-dev"
"libusb-1.0.0-dev"
"libssl-dev"
"libprotobuf-dev"
"protobuf-c-compiler"
"protobuf-compiler"
"libqt5multimedia5"
"libqt5multimedia5-plugins"
"libqt5multimediawidgets5"
"qtmultimedia5-dev"
"libqt5bluetooth5"
"libqt5bluetooth5-bin"
"qtconnectivity5-dev"
"pulseaudio"
"librtaudio-dev"
"librtaudio6"
"libkf5bluezqt-dev"
"libtag1-dev"
)

#loop through dependencies and install
for app in ${deps[@]}; do
	echo "installing: " $app
	sudo apt -qq -o=Dpkg::Use-Pty=0 install $app -y > /dev/null 2> /dev/null

	if [[ $? > 0 ]]
	then
	    echo $app " Failed to install, quitting"
	    exit
	else
	    echo $app " Installed ok"
	    echo
	    
	fi
done

echo "All dependencies installed"
echo

#make ilclient
echo "making ilclient"
make /opt/vc/src/hello_pi/libs/ilclient
if [[ $? > 0 ]]
  then
    echo "unable to make ilclient"
  exit
else
  echo "made ok"
  echo
fi

#begin cmake
echo "beginning cmake for raspberry pi"
cmake -DRPI_BUILD=TRUE -DGST_BUILD=TRUE -DCMAKE_BUILD_TYPE=Release . 
if [[ $? > 0 ]]
  then
    echo "Cmake error"
  exit
else
  echo "Cmake ok"
  echo #####
fi

#start make
echo "beginning make, this will take a while"
make
if [[ $? > 0 ]]
  then
    echo "make error check output above"
    exit
  else
    echo "make ok, executable can be found ../bin/ia"
    echo

    #check if usb rules exist
    echo "checking if permissions exist"

    #udev rule to be created below, change as needed
    FILE=/etc/udev/rules.d/51-iadash.rules
    if [[ -f "$FILE" ]]
      then
        echo "rules exists"
        echo
      else
        sudo touch $FILE

        # OPEN USB RULE, CREATE MORE SECURE RULE IF REQUIRED
        echo "SUBSYSTEM==\"usb\", ATTR{idVendor}==\"*\", ATTR{idProduct}==\"*\", MODE=\"0660\", GROUP=\"plugdev\"" | sudo tee $FILE
      if [[ $? > 0 ]]
        then
          echo "unable to create permissions"
        else
          echo "permissions created"
      fi
    fi
fi

#Start app
echo "starting app"
cd ../bin
./ia
