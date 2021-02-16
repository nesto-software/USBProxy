# minimal CMakeLists.txt for the AWS Greengrass SDK for C
cmake_minimum_required(VERSION 2.8)
set(TOOL usb-mitm-gg)
LIST(APPEND TOOLS_LINK_LIBS USBProxy -pthread zmq aws-greengrass-core-sdk-c)

find_package(aws-greengrass-core-sdk-c REQUIRED)

include_directories(${libusbproxy_SOURCE_DIR})
add_executable(${TOOL} main.cpp)
target_link_libraries(${TOOL} ${TOOLS_LINK_LIBS})
target_compile_options(${TOOL} PRIVATE -Werror -Wall -Wextra -pedantic -std=c++11)

install(TARGETS ${TOOL} RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})

# build the deployment package
# the following is only working with: https://github.com/awslabs/aws-lambda-cpp
#aws_lambda_package_target(${TOOL})