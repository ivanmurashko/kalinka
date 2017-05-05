#!/bin/bash

CURRENT_DIR=`pwd`
PREFIX=$CURRENT_DIR/build
DATADIR=$PREFIX/usr/share/doc

./cleanup.sh
./bootstrap.sh

rm -rf $PREFIX/usr/share/kalinka/modules

./configure --enable-debug --prefix=$PREFIX --datadir=$DATADIR  --docdir=$DATADIR 
make all && make install && make install-data && make tags
