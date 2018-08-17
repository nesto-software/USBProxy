#
# This file is part of USBProxy.
#

set(TOOL usb-mitm)

include_directories(${libusbproxy_SOURCE_DIR})
LIST(APPEND TOOLS_LINK_LIBS USBProxy -pthread zmq)

add_executable(${TOOL} ${TOOL}.cpp)
set_source_files_properties(${TOOL}.cpp PROPERTIES COMPILE_FLAGS  "-std=c++11 -pthread -Wall -I.")
target_link_libraries(${TOOL} ${TOOLS_LINK_LIBS})
install(TARGETS ${TOOL} RUNTIME DESTINATION ${INSTALL_DEFAULT_BINDIR})
