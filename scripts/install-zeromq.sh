#!/bin/bash

# is following needed?
#echo "deb http://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/ ./" >> /etc/apt/sources.list
#wget https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/Release.key -O- | sudo apt-key add
#apt-get install libzmq3-dev

git clone https://github.com/zeromq/libzmq.git /tmp/libzmq
(cd /tmp/libzmq/ && /tmp/libzmq/autogen.sh; ./configure; make; sudo make install)

git clone https://github.com/zeromq/cppzmq.git /tmp/cppzmq
(cd /tmp/cppzmq && mkdir build && cd build && cmake .. && sudo make -j4 install)