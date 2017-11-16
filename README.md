# libCVD

Note, the master branch is now libCVD-2.0 which is in beta and requires C++14.

## Compiling and installing

To install on a UNIX system:

    ./configure && make && sudo make install

To verify that a few things work, you can optionally run

    make test

To build on Windows, use [CMake](https://cmake.org/):

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=<directory> ..
    cmake --build . --target INSTALL --config Release

## System compatibility

You need a C++14 compiler. 

All libraries are optional but you will be missing features if the libraries
aren't present. The configure script will tell you what's present and what's
not.


## Documentation

[![Documentation Status](https://codedocs.xyz/edrosten/libcvd.svg)](https://codedocs.xyz/edrosten/libcvd/)

Latest documentation here: https://codedocs.xyz/edrosten/libcvd/ or just run Doxygen.

## Status of unit tests
![Build Status](https://circleci.com/gh/edrosten/libcvd.svg?style=shield&circle-token=db58907af52b26d11f2c4f5de2ff3b1a59543ddc)


## News and main page

https://www.edwardrosten.com/cvd/
