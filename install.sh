#!/bin/bash

#repo addresses
aasdkRepo="https://github.com/OpenDsh/aasdk"
gstreamerRepo="git://anongit.freedesktop.org/gstreamer/qt-gstreamer"
openautoRepo="https://github.com/rhysmorgan134/openauto.git"

#Help text
display_help() {
    echo
    echo "   --deps           install all dependencies"
    echo "   --aasdk          install and build aasdk"
    echo "   --openauto       install and build openauto "
    echo "   --gstreamer      install and build gstreamer "
    echo "   --dash           install and build dash "
    echo
    exit 1
}

#set default arguments
deps=false
aasdk=false
gstreamer=false
openauto=false
dash=false
perms=false

if [ $# -gt 0 ]; then
    while [ "$1" != "" ]; do
        case $1 in
            --deps )           shift
                                    deps=true
                                    ;;
            --aasdk )           aasdk=true
                                    ;;
            --gstreamer )       gstreamer=true
                                    ;;
            --openauto )       openauto=true
                                    ;;
            --dash )           dash=true
                                    ;;
            --perms )          perms=true
                                    ;;
            -h | --help )           display_help
                                    exit
                                    ;;
            * )                     display_help
                                    exit 1
        esac
        shift
    done

else
    echo "Full install running"
    deps=true
    aasdk=true
    gstreamer=true
    openauto=true
    dash=true
    perms=true
fi



#Array of dependencies any new dependencies can be added here
dependencies=(
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
"qml-module-qtquick2"
"doxygen"
"qml-module-qtquick*"
"libglib2.0-dev"
"libgstreamer1.0-dev"
"gstreamer1.0-plugins-base-apps"
"gstreamer1.0-plugins-bad"
"gstreamer1.0-libav"
"gstreamer1.0-alsa"
"libgstreamer-plugins-base1.0-dev"
"qtdeclarative5-dev"
"qt5-default"
"libgstreamer-plugins-bad1.0-dev"
"libunwind-dev"
"qml-module-qtmultimedia"
)

###############################  dependencies  #########################
if [ "$deps" = true ]; then
  #loop through dependencies and install
	echo "installing dependencies"
	echo "Running apt update"
	sudo apt update
  for app in ${dependencies[@]}; do
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
else
	echo "skipping dependencies"
fi


###############################  AASDK #########################
if [ "$aasdk" = true ]; then
  #change to parent directory
  cd ..

  #clone aasdk
  git clone $aasdkRepo
  if [[ $? > 0 ]]
      then
        cd aasdk
        if [[ $? > 0 ]]
          then
            echo "clone/pull error"
          exit
        else
          git pull $aasdkRepo
          echo "cloned OK"
          cd ..
          echo
        fi
  else
    echo "cloned ok"
    echo
  fi

  #change into aasdk folder
  echo "moving to aasdk"
  cd aasdk

  #beginning cmake
  cmake -DCMAKE_BUILD_TYPE=Release .
  if [[ $? > 0 ]]
    then
      echo "CMake error check logs"
    exit
  else
    echo "Cmake ok"
    echo
  fi

  #beginning make
  make -j2

  if [[ $? > 0 ]]
    then
      echo "make error check logs"
    exit
  else
    echo "make ok"
    echo
  fi

  #begin make install
  sudo make install

  if [[ $? > 0 ]]
    then
      echo "install error check logs"
    exit
  else
    echo "installed ok ok"
    echo
  fi
  cd ../dash
else
	echo "skipping aasdk"
fi


###############################  gstreamer  #########################
#check if gstreamer install is requested
if [ "$gstreamer" = true ]; then
  echo "installing gstreamer"
  #change to parent directory
  cd ..
  #clone gstreamer
  echo "Cloning Gstreamer"
  git clone $gstreamerRepo
  if [[ $? > 0 ]]
    then
      cd qt-gstreamer
      if [[ $? > 0 ]]
        then
          echo "clone/pull error"
        exit
      else
        git pull gstreamerRepo
        echo "cloned OK"
        cd ..
        echo
      fi
  else
    echo "cloned OK"
    echo
  fi

  #change into newly cloned directory
  cd qt-gstreamer

  #create build directory
  echo "creating Gstreamer build directory"

  mkdir build

  if [[ $? > 0 ]]
    then
      echo "unable to create Gstreamer build directory assuming it exists..."
  else
    echo "build directory made"
    echo
  fi

  cd build

  #run cmake
  echo "beginning cmake"

  cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH) -DCMAKE_INSTALL_INCLUDEDIR=include -DQT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-std=c++11

  if [[ $? > 0 ]]
    then
      echo "cmake failed"
    exit
  else
    echo "make ok"
    echo
  fi

  #make j4
  echo "making J4"
  make -j4

  if [[ $? > 0 ]]
    then
      echo "make error check logs"
    exit
  else
    echo "make ok"
    echo
  fi

  #run make install
  echo "beginning make install"
  sudo make install

  if [[ $? > 0 ]]
    then
      echo "install error check logs"
    exit
  else
    echo "installed ok"
    echo
  fi

  #run ldconfig
  sudo ldconfig
  cd ../../dash

else
	echo "skipping gstreamer"
fi



###############################  openauto  #########################
if [ "$openauto" = true ]; then
  echo "installing openauto"
  cd ..

  clone openauto
  git clone $openautoRepo
  if [[ $? > 0 ]]
    then
      cd openauto
      if [[ $? > 0 ]]
        then
          echo "clone/pull error"
        exit
      else
        git pull $openautoRepo
        echo "cloned OK"
        cd ..
        echo
      fi
  else
    echo "cloned OK"
    echo
  fi

  cd openauto

  #beginning cmake
  cmake -DRPI_BUILD=true -DGST_BUILD=true
  if [[ $? > 0 ]]
    then
      echo "cmake error check logs"
    exit
  else
    echo "cmake OK"
    echo
  fi

  #beginning cmake
  make
  if [[ $? > 0 ]]
    then
      echo "make error check logs"
    exit
  else
    echo "make OK"
    echo
  fi

  #run make install
  echo "beginning make install"
  sudo make install
  if [[ $? > 0 ]]
    then
      echo "install error check logs"
    exit
  else
    echo "installed ok"
    echo
  fi
  cd ../dash
else
	echo "skipping openauto"
fi


###############################  dash  #########################
if [ "$dash" = true ]; then
  #loop through dependencies and install
	echo "installing dash"
  echo "making ilclient"
  make /opt/vc/src/hello_pi/libs/ilclient
  if [[ $? > 0 ]]
    then
      echo "error making ilclient check logs"
    exit
  else
    echo "make ok"
    echo
  fi

  echo "running cmake for dash"
  cmake -DRPI_BUILD=TRUE -DGST_BUILD=TRUE .
  if [[ $? > 0 ]]
    then
      echo "cmake error check logs"
    exit
  else
    echo "cmake OK"
    echo
  fi

  echo "running make"
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
  cd bin
  ./ia
else
	echo "skipping dash"
fi