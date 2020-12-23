#!/bin/bash

#repo addresses
aasdkRepo="https://github.com/OpenDsh/aasdk"
gstreamerRepo="git://anongit.freedesktop.org/gstreamer/qt-gstreamer"
openautoRepo="https://github.com/openDsh/openauto"

#Help text
display_help() {
    echo
    echo "   --deps           install all dependencies"
    echo "   --aasdk          install and build aasdk"
    echo "   --openauto       install and build openauto "
    echo "   --gstreamer      install and build gstreamer "
    echo "   --dash           install and build dash "
    echo "   --debug          create a debug build "
    echo
}

#location of OS details for linux
OS_RELEASE_FILE="/etc/os-release"

#check if Raspian is in the file, if not set the install Args to be false
if grep -q "Raspbian" ${OS_RELEASE_FILE}; then
  installArgs="-DRPI_BUILD=true"
  isRpi=true
else
  installArgs=""
  isRpi=false
fi

BUILD_TYPE="Release"

#check to see if there are any arguments supplied, if none are supplied run full install
if [ $# -gt 0 ]; then
  #initialize all arguments to false before looping to find which are available
  deps=false
  aasdk=false
  gstreamer=false
  openauto=false
  dash=false
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
            --debug )          BUILD_TYPE="Debug"
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
    echo -e Full install running'\n'
    deps=true
    aasdk=true
    gstreamer=true
    openauto=true
    dash=true
fi

installArgs="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} $installArgs"

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
if [ $deps = false ]
  then
    echo skipping dependencies '\n'
  else
    #loop through dependencies and install
    echo installing dependencies
    echo Running apt update
    sudo apt update

    installString="sudo apt install -y "

    #create apt install string
    for i in ${dependencies[@]}; do
      installString+=" $i"
    done

    #run install
    ${installString}
    if [[ $? -eq 0 ]]; then
        echo -e All dependencies Installed ok '\n'
    else
        echo Package failed to install with error code $?, quitting check logs above
        exit 1
    fi
fi


###############################  AASDK #########################
if [ $aasdk = false ]; then
	echo -e Skipping aasdk '\n'
else
  #change to parent directory
  cd ..

  #clone aasdk
  git clone $aasdkRepo
  if [[ $? -eq 0 ]]; then
    echo -e Aasdk Cloned ok '\n'
  else
    cd aasdk
    if [[ $? -eq 0 ]]; then
      git pull $aasdkRepo
      echo -e Aasdk Cloned OK '\n'
      cd ..
    else
      echo Aasdk clone/pull error
      exit 1
    fi
  fi

  #change into aasdk folder
  echo -e moving to aasdk '\n'
  cd aasdk

  #create build directory
  echo Creating aasdk build directory
  mkdir build

  if [[ $? -eq 0 ]]; then
    echo -e aasdk build directory made
  else
    echo Unable to create aasdk build directory assuming it exists...
  fi

  cd build

  #beginning cmake
  cmake -DCMAKE_BUILD_TYPE=Release ../
  if [[ $? -eq 0 ]]; then
      echo -e Aasdk CMake completed successfully'\n'
  else
    echo Aasdk CMake failed with code $?
    exit 1
  fi

  #beginning make
  make -j2

  if [[ $? -eq 0 ]]; then
    echo -e Aasdk Make completed successfully '\n'
  else
    echo Aasdk Make failed with code $?
    exit 1
  fi

  #begin make install
  sudo make install

  if [[ $? -eq 0 ]]
    then
    echo -e Aasdk installed ok'\n'
    echo
  else
    echo Aasdk install failed with code $?
    exit 1
  fi
  cd ../../dash
fi


###############################  gstreamer  #########################
#check if gstreamer install is requested
if [ $gstreamer = true ]; then
  echo installing gstreamer

  #change to parent directory
  cd ..

  #clone gstreamer
  echo Cloning Gstreamer
  git clone $gstreamerRepo
  if [[ $? -eq 0 ]]; then
    echo -e Gstreamer cloned OK
  else
    cd qt-gstreamer
      if [[ $? -eq 0 ]]; then
        git pull $gstreamerRepo
        echo -e cloned OK '\n'
        cd ..
      else
        echo Gstreamer clone/pull error
        exit 1
      fi
  fi

  #change into newly cloned directory
  cd qt-gstreamer

  #create build directory
  echo Creating Gstreamer build directory
  mkdir build

  if [[ $? -eq 0 ]]; then
    echo -e Gstreamer build directory made
  else
    echo Unable to create Gstreamer build directory assuming it exists...
  fi

  cd build

  #run cmake
  echo Beginning cmake
  cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH) -DCMAKE_INSTALL_INCLUDEDIR=include -DQT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-std=c++11

  if [[ $? -eq 0 ]]; then
    echo -e Make ok'\n'
  else
    echo Gstreamer CMake failed
    exit 1
  fi

  echo Making Gstreamer
  make -j4

  if [[ $? -eq 0 ]]; then
    echo -e Gstreamer make ok'\n'
  else
    echo Make failed with error code $?
    exit 1
  fi

  #run make install
  echo Beginning make install
  sudo make install

  if [[ $? -eq 0 ]]; then
    echo -e Gstreamer installed ok'\n'
  else
    echo Gstreamer make install failed with error code $?
    exit 1
  fi

  sudo ldconfig
  cd ../../dash

else
	echo -e Skipping Gstreamer'\n'
fi



###############################  openauto  #########################
if [ $openauto = false ]; then
  echo -e skipping openauto'\n'
else
  echo Installing openauto
  cd ..

  echo -e cloning openauto'\n'
  git clone $openautoRepo
  if [[ $? -eq 0 ]]; then
    echo -e cloned OK'\n'
  else
    cd openauto
    if [[ $? -eq 0 ]]; then
      git pull $openautoRepo
      echo -e Openauto cloned OK'\n'
      cd ..
    else
      echo Openauto clone/pull error
      exit 1
    fi
  fi

  cd openauto

  #create build directory
  echo Creating openauto build directory
  mkdir build

  if [[ $? -eq 0 ]]; then
    echo -e openauto build directory made
  else
    echo Unable to create openauto build directory assuming it exists...
  fi

  cd build

  echo Beginning openauto cmake
  cmake ${installArgs} -DGST_BUILD=true ../
  if [[ $? -eq 0 ]]; then
    echo -e Openauto CMake OK'\n'
  else
    echo Openauto CMake failed with error code $?
    exit 1
  fi

  echo Beginning openauto make
  make
  if [[ $? -eq 0 ]]; then
    echo -e Openauto make OK'\n'
  else
    echo Openauto make failed with error code $?
    exit 1
  fi

  #run make install
  echo Beginning make install
  sudo make install
  if [[ $? -eq 0 ]]; then
    echo -e Openauto installed ok'\n'
  else
    echo Openauto make install failed with error code $?
    exit 1
  fi
  cd ../../dash
fi


###############################  dash  #########################
if [ $dash = false ]; then
	echo -e Skipping dash'\n'
else

  #create build directory
  echo Creating dash build directory
  mkdir build

  if [[ $? -eq 0 ]]; then
    echo -e dash build directory made
  else
    echo Unable to create dash build directory assuming it exists...
  fi

  cd build

	echo -e Installing dash'\n'
  echo Running CMake for dash
  cmake ${installArgs} -DGST_BUILD=TRUE ../
  if [[ $? -eq 0 ]]; then
    echo -e Dash CMake OK'\n'
  else
    echo Dash CMake failed with error code $?
    exit 1
  fi

  echo Running Dash make
  make
  if [[ $? -eq 0 ]]; then
      echo -e Dash make ok, executable can be found ../bin/dash
      echo

      #check and add usb rules for openauto if they dont exist
      echo Checking if permissions exist
      #udev rule to be created below, change as needed
      FILE=/etc/udev/rules.d/51-dashusb.rules
      if [[ ! -f "$FILE" ]]; then
          # OPEN USB RULE, CREATE MORE SECURE RULE IF REQUIRED
          echo "SUBSYSTEM==\"usb\", ATTR{idVendor}==\"*\", ATTR{idProduct}==\"*\", MODE=\"0660\", GROUP=\"plugdev\"" | sudo tee $FILE
        if [[ $? -eq 0 ]]; then
            echo -e Permissions created'\n'
          else
            echo -e Unable to create permissions'\n'
        fi
        else
          echo -e Rules exists'\n'
      fi
    else
      echo Dash make failed with error code $?
      exit 1
  fi

  #Setting openGL driver and GPU memory to 256mb
  if $isRpi; then
    sudo raspi-config nonint do_memory_split 256
    if [[ $? -eq 0 ]]; then
      echo -e Memory set to 256mb'\n'
    else
      echo Setting memory failed with error code $? please set manually
      exit 1
    fi

    sudo raspi-config nonint do_gldriver G2
    if [[ $? -eq 0 ]]; then
      echo -e OpenGL set ok'\n'
    else
      echo Setting openGL failed with error code $? please set manually
      exit 1
    fi

  fi


  #Start app
  echo Starting app
  cd ../bin
  ./dash
fi
