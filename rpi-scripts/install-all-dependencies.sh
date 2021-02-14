#!/bin/bash

echo "[1/5] Installing libusb"
sudo apt-get install -y libusb-1.0.0-dev

echo "[2/5] Installing boost"
sudo apt-get install -y libboost-all-dev

echo "[3/5] Installing libzmq"
git clone https://github.com/zeromq/libzmq.git /tmp/libzmq
(cd /tmp/libzmq/ && /tmp/libzmq/autogen.sh; ./configure; make; sudo make install)

echo "[4/5] Installing cppzmq"
git clone https://github.com/zeromq/cppzmq.git /tmp/cppzmq
(cd /tmp/cppzmq && mkdir build && cd build && cmake .. && sudo make -j4 install)

echo "[5/5] Installing msgpack"
sudo apt-get install -y doxygen
git clone https://github.com/msgpack/msgpack-c.git /tmp/msgpack-c
(cd /tmp/msgpack-c && git checkout cpp_master && cmake -DMSGPACK_CXX17=ON . && sudo make install)
