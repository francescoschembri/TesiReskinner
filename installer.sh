#!/bin/bash

sudo apt-get update
sudo apt-get install git
sudo apt-get install xorg-dev
sudo apt-get install make
sudo apt-get install cmake
sudo apt-get install build-essential
cd ~/Desktop
git clone --recursive https://github.com/francescoschembri/TesiReskinner
cd TesiReskinner/build
conan install ..
cmake -S .. -B .
make