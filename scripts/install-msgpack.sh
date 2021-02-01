#!/bin/bash

sudo apt-get install libboost-all-dev
sudo apt-get install doxygen
git clone https://github.com/msgpack/msgpack-c.git /tmp/msgpack-c
(cd /tmp/msgpack-c && git checkout cpp_master && cmake -DMSGPACK_CXX17=ON . && sudo make install)