#!/bin/bash
set -e
set -o pipefail

echo -e "[0/7] Updating package list\n"
sudo apt-get update

echo -e "[1/7] Installing cmake"
sudo apt-get install -y cmake

echo -e "[2/7] Installing libusb\n"
sudo apt-get install -y libusb-1.0.0-dev

echo -e "[3/7] Installing boost libs: chrono, timer, system\n"
sudo apt-get install -y libboost-chrono-dev libboost-timer-dev libboost-system-dev

echo -e "[4/7] Installing libzmq\n"
sudo apt-get install -y libzmq3-dev

echo -e "[5/7] Installing cppzmq\n"
git clone https://github.com/zeromq/cppzmq.git /tmp/cppzmq || echo "Skipped clone."
(cd /tmp/cppzmq && mkdir -p build && cd build && cmake -DCPPZMQ_BUILD_TESTS=off .. && sudo make -j4 install)

echo -e "[6/7] Installing msgpack\n"
git clone https://github.com/msgpack/msgpack-c.git /tmp/msgpack-c || echo "Skipped clone."
(cd /tmp/msgpack-c && git checkout cpp_master && cmake -DMSGPACK_CXX17=ON . && sudo make install)

echo -e "[7/7] Put RPi USB into client mode"
set +e
set +o pipefail

cat /boot/config.txt | grep -q dwc2
rc=$?

if [ $rc -ne 0 ]; then
    echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
    echo "dwc2" | sudo tee -a /etc/modules

    echo "Changed USB c port to OTG client mode."
fi

echo -e "\nYou must restart the pi now for changes to usb mode to take effect!!! Use: 'sudo reboot now'\n"