#/bin/bash

VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}

## Platform independent bootstrap (mainly vcpkg installs)
./scripts/internal/bootstrap-common.sh

# Install arrow
pushd $HOME
wget https://apache.bintray.com/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-archive-keyring-latest-$(lsb_release --codename --short).deb
sudo apt install -y -V ./apache-arrow-archive-keyring-latest-$(lsb_release --codename --short).deb
sudo apt update
sudo apt install libarrow-dev
popd

## Loaders.gl Dependencies
sudo apt install libgdal-dev libgdal20  # For GDAL

## Graphics Dependencies
sudo apt install libglwf3-dev
sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev # For GLFW
