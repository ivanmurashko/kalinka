#!/bin/bash

PREFIX=./build
DATADIR=$PREFIX/usr/share/doc

./cleanup.sh
./bootstrap.sh

rm -rf $PREFIX/usr/share/kalinka/modules

#./configure --enable-debug --prefix=$PREFIX --datadir=$DATADIR

