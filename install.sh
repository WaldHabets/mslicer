#!/bin/bash
if [ -z "$SUDO_USER" ];
then
    echo "install requires sudo"
    exit
fi

if [ "$1" == "install" ];
then
    sudo mkdir -p "./dist/usr/local/bin/"
    sudo cp "./dist/mslicer" "./dist/usr/local/bin/mslicer"
fi

if [ "$1" == "remove" ];
then
    sudo rm -f "./dist/usr/local/bin/mslicer"
fi