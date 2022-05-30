#!/bin/bash

sudo apt-get update
sudo apt-get install git
sudo apt-get install xorg-dev
sudo apt-get install make
sudo apt-get install cmake
sudo apt-get install build-essential
cd build
rm -rf *
conan install .. --settings os="Linux" --settings compiler="gcc" --settings compiler.version=11 --build missing
cmake -S .. -B . -D CMAKE_BUILD_TYPE=Release
make