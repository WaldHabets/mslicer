#!/bin/bash
if [ -z "$SUDO_USER" ];
then
    echo "install requires sudo"
    exit
fi

if [ "$1" == "install" ];
then
    sudo mkdir -p "/usr/local/bin/"
    sudo cp "./dist/mslicer" "/usr/local/bin/mslicer"
fi

if [ "$1" == "remove" ];
then
    sudo rm -f "/usr/local/bin/mslicer"
fi