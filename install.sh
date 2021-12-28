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

install() {
    sudo mkdir -p "${TARGET}"
    sudo cp "./dist/${DIST_NAME}" "${TARGET}${DIST_NAME}"
}

if [ "$1" == "update" ]; then
    git pull origin main
    make
    install
    make clean
elif [ "$1" == "install" ]; then
    install
elif [ "$1" == "remove" ]; then
    sudo rm -f "${TARGET}${DIST_NAME}"
else
    echo "Invalid argument, use:"
    echo "    install.sh update"
    echo "    install.sh install"
    echo "    install.sh remove"
fi