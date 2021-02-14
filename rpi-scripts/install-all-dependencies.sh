#!/bin/bash

echo -e "[0/6] Updating package list\n"
sudo apt-get update

echo -e "[1/6] Installing cmake"
sudo apt-get install -y cmake

echo -e "[2/6] Installing libusb\n"
sudo apt-get install -y libusb-1.0.0-dev

echo -e "[3/6] Installing boost\n"
sudo apt-get install -y libboost-all-dev

echo -e "[4/6] Installing libzmq\n"
git clone https://github.com/zeromq/libzmq.git /tmp/libzmq
(cd /tmp/libzmq/ && /tmp/libzmq/autogen.sh; ./configure; make; sudo make install)

echo -e "[5/6] Installing cppzmq\n"
git clone https://github.com/zeromq/cppzmq.git /tmp/cppzmq
(cd /tmp/cppzmq && mkdir build && cd build && cmake .. && sudo make -j4 install)

echo - e"[6/6] Installing msgpack\n"
sudo apt-get install -y doxygen
git clone https://github.com/msgpack/msgpack-c.git /tmp/msgpack-c
(cd /tmp/msgpack-c && git checkout cpp_master && cmake -DMSGPACK_CXX17=ON . && sudo make install)
