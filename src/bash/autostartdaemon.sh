#!/bin/bash
###
# Helper script to setup an autostart daemon for openDash
###
WorkingDirectory="/home/pi/dash/"
if [[ $1 != "" ]]
then
    WorkingDirectory="/home/pi/$1/dash/"
fi
echo ${WorkingDirectory}

#stop and disable dash service
sudo systemctl stop dash.service || true
sudo systemctl disable dash.service || true

#remove existing opendash service
sudo systemctl unmask dash.service || true

sudo bash -c "echo '[Unit]
Description=Dash
After=graphical.target

[Service]
Type=idle
User=pi
StandardOutput=inherit
StandardError=inherit
Environment=DISPLAY=:0
Environment=XAUTHORITY=/home/pi/.Xauthority
WorkingDirectory=${WorkingDirectory}
ExecStart=/usr/local/bin/dash
Restart=on-failure
RestartSec=5s
KillMode=process
TimeoutSec=infinity

[Install]
WantedBy=graphical.target
' > /etc/systemd/system/dash.service"

sudo systemctl daemon-reload
sudo systemctl enable dash.service
sudo systemctl start dash.service
sudo systemctl status dash.service