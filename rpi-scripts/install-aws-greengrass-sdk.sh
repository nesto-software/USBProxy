#!/bin/bash

git clone https://github.com/aws/aws-greengrass-core-sdk-c.git /tmp/aws-sdk
(cd /tmp/aws-sdk/aws-greengrass-core-sdk-c && mkdir -p build && cd build && cmake .. && cmake --build . && sudo make install)