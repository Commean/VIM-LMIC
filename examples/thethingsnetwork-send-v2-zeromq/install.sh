#!/bin/bash

services=(service/cbridge.service)

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

echo "Installing CBridge"

cd build
make install


echo "Installing SystemD module"


for service in ${services[@]}; do
    echo "Installing $service..."
    cp $service /etc/systemd/system/
done

systemctl daemon-reload

for service in ${services[@]}; do
    echo "Enabling $service..."
    systemctl disable $service
    systemctl enable $service
done
