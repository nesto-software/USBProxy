# minimal CMakeLists.txt for the AWS Greengrass SDK for C
cmake_minimum_required(VERSION 2.8)
set(TOOL usb-mitm-gg)

# Locate the AWS Greengras SDK for C package.
# Requires that you build with:
#   -Daws-greengrass-core-sdk-c_DIR=/usr/local/lib/
# or export/set:
#   CMAKE_PREFIX_PATH=/usr/local/lib/
find_package(aws-greengrass-core-sdk-c REQUIRED)

add_executable(${TOOL} main.cpp)
target_link_libraries(${TOOL} aws-greengrass-core-sdk-c)