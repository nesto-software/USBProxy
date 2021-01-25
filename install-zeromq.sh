#!/bin/bash

# Before installing, make sure you have installed all the needed packages
sudo apt-get install libtool pkg-config build-essential autoconf automake
sudo apt-get install libzmq-dev

# Install libsodium
git clone git://github.com/jedisct1/libsodium.git
cd libsodium
./autogen.sh
./configure && make check
sudo make install
sudo ldconfig

cd /opt

# Install zeromq
# latest version as of this post is 4.1.2
wget http://download.zeromq.org/zeromq-4.1.2.tar.gz
tar -xvf zeromq-4.1.2.tar.gz
cd zeromq-4.1.2
./configure
make
sudo make install