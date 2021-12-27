#!/bin/bash
if [ -f .env ];
then
    export env $(cat .env | xargs)
else
    echo "Error: could not load enviroment variables!"
    exit
fi

if [ -z "$SUDO_USER" ];
then
    echo "install requires sudo"
    exit
fi

TARGET="/usr/local/bin/"

if [ "$1" == "install" ]; then
    sudo mkdir -p "${TARGET}"
    sudo cp "./dist/${DIST_NAME}" "${TARGET}${DIST_NAME}"
elif [ "$1" == "remove" ]; then
    sudo rm -f "${TARGER}${DIST_NAME}"
else
    echo "Invalid argument, use:"
    echo "    install.sh install"
    echo "    install.sh remove"
fi