#!/bin/bash

rm -fr bin
mkdir bin
rm -fr build
mkdir build
cd build

cmake ../
make

