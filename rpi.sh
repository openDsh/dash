#!/bin/bash

#location of OS details for linux
OS_RELEASE_FILE="/etc/os-release"

#check if Raspian OS is active, otherwise kill script
if grep -q "Raspbian" ${OS_RELEASE_FILE};
then
 echo "Great this script works for RaspberryPI OS"
else
 echo "This script works only for an RaspberryPI OS"
 exit 1;
fi

display_version(){
    echo "Version 0.2 RaspberryPI Dash additional install helpers"
}

#########################
# The command line help #
#########################
display_help() {
    echo "Usage: $0 [option...]" >&2
    echo
    echo "   -arb, --addrulebrightness        Add udev rules for brightness"
    echo "   -adi, --adddesktopicon           Add desktop icon"
    echo "   -asd, --autostartdaemon          Add autostart daemon"
    echo "   -v, --version                    Show version of script"
    echo "   -h, --help                       Show help of script"
    echo
    echo
    echo "Example: Setup udev rule for controlling brightness of an official 7inch Touch screen"
    echo "   rpi -arb"
    echo
    echo "Example: Add an desktop icon on your RPI."
    echo "   rpi -adi"
    echo
    echo "Example: Add autostart daemon on your RPI."
    echo "   rpi -asd"
    echo
    exit 1
}

################################
# Check if parameters options  #
# are given on the commandline #
################################
while :
do
    #echo "$1"
    case "$1" in
        -arb | --addrulebrightness)
            /bin/bash src/bash/brightness.sh
            exit 0
          ;;
        -adi | --adddesktopicon)
            /bin/bash src/bash/desktop.sh
            exit 0
          ;;
        -asd | --autostartdaemon)
            if [ $# -ne 0 ]; then
              /bin/bash src/bash/autostartdaemon.sh $2
              exit 0
            fi
          ;;
        -h | --help)
            display_help  # Call your function
            exit 0
          ;;
        -v | --version)
            display_version  # Call your function
            exit 0
          ;;

        --) # End of all options
            shift
            break
          ;;
        -*)
            echo "Error: Unknown option: $1" >&2
            ## or call function display_help
            exit 1
          ;;
        *)  # No more options
            break
          ;;
    esac
done