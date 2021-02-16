#!/bin/bash

git clone https://github.com/aws/aws-greengrass-core-sdk-c.git /tmp/aws-sdk
(cd /tmp/aws-sdk/ && git checkout a71613711438b48588e177f86ab322dd3992e780 && cd ./aws-greengrass-core-sdk-c && mkdir -p build && cd build && cmake .. && cmake --build . && sudo make install)