FROM ghcr.io/nesto-software/nesto-toolchain:latest as nesto-custom-build
 
ENV STAGING_DIR /usr/raspberry-build/staging
ENV SYSROOT /opt/crosstool-ng/x-tools/arm-rpi-linux-gnueabihf/arm-rpi-linux-gnueabihf/sysroot
ENV ROOT_FS /usr/raspberry-build/rootfs

# prepare directories for staging and rootfs, see: https://crosstool-ng.github.io/docs/toolchain-usage/ - Option 3
RUN mkdir -p "${STAGING_DIR}" && mkdir -p "${ROOT_FS}"

# Install cmake and build utils for build host
RUN apt-get install -y build-essential cmake3

# install libzmq
RUN git clone https://github.com/zeromq/libzmq.git /tmp/libzmq && cd /tmp/libzmq && git checkout 92282785ed8e3a954d379a0ac0e784dc29d94746
RUN apt-get install -y pkg-config

WORKDIR /tmp/libzmq/
RUN ./autogen.sh
# note: we must use both prefix options here because DESTPATH for make install places the files correctly but sets wrong pkg-config contents
RUN CC=${TOOLCHAIN}-gcc CXX=${TOOLCHAIN}-g++ AR=${TOOLCHAIN}-ar STRIP=${TOOLCHAIN}-strip RANLIB=${TOOLCHAIN}-ranlib ./configure --host=arm-none-linux-gnueabi --exec-prefix=$STAGING_DIR/usr/local --prefix=${STAGING_DIR}/usr/local --disable-curve-keygen 
RUN make
RUN make install

# install the CMAKE_TOOLCHAIN_FILE for our toolchain
ENV TOOLCHAIN_FILE /usr/raspberry-build/tmp/raspberry_pi_3_b_plus.cmake.tc
RUN mkdir -p /usr/raspberry-build/tmp/
COPY ./docker-crosstool-ng-arm/config/raspberry_pi_3_b_plus.cmake.tc $TOOLCHAIN_FILE

# install cppzmq
RUN git clone https://github.com/zeromq/cppzmq.git /tmp/cppzmq && cd /tmp/cppzmq && git checkout c591113bb7975e1be6fa6b0c758cacfe0411c66e

WORKDIR /tmp/cppzmq
RUN mkdir build
WORKDIR ./build

# install latest cmake binary directly (reason: cppzmq needs a feature of cmake 3.7+ regarding tarball name extraction)
RUN wget https://github.com/Kitware/CMake/releases/download/v3.19.4/cmake-3.19.4-Linux-x86_64.sh -P /tmp/
RUN chmod +x /tmp/cmake-3.19.4-Linux-x86_64.sh && mkdir /tmp/cmake && /tmp/cmake-3.19.4-Linux-x86_64.sh --skip-license --prefix=/tmp/cmake

# note: confirm pkg-content with pkg-config cli if in doubt: PKG_CONFIG_PATH=/usr/raspberry-build/staging/usr/local/lib/pkgconfig pkg-config libzmq --exists; echo $?
# note: without setting CMAKE_FIND_ROOT_PATH, the cmake find_* methods do not work as expected because of the toolchain file setting for CMAKE_FIND_ROOT_PATH_MODE_LIBRARY
RUN  CC=${TOOLCHAIN}-gcc CXX=${TOOLCHAIN}-g++ PKG_CONFIG_PATH=$STAGING_DIR/usr/local/lib/pkgconfig /tmp/cmake/bin/cmake .. "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE" "-DCMAKE_FIND_ROOT_PATH=$STAGING_DIR"
RUN  CC=${TOOLCHAIN}-gcc CXX=${TOOLCHAIN}-g++ make
RUN DESTDIR=${STAGING_DIR} make -j4 install

# install msgpack
RUN git clone https://github.com/msgpack/msgpack-c.git /tmp/msgpack-c

# install dependencies for msgpack: doxygen and boost
RUN sudo apt-get install doxygen
RUN git clone https://github.com/boostorg/boost.git /tmp/boost && cd /tmp/boost && git checkout eeb338c73f90028145c52ec9de07b6eb2b2ad4e8
WORKDIR /tmp/boost

# install specific boost submodules
RUN git submodule init
# note: not sure which boost modules are actually required because they have nested dependencies - we install a bit more than probably required
# dependency graph: https://pdimov.github.io/boostdep-report/master/module-overview.html
# note: headers and date_time can be removed (probably)
RUN git submodule update libs/chrono libs/date_time libs/timer libs/config libs/core libs/detail tools/build libs/system tools/boost_install libs/headers libs/assert libs/integer libs/static_assert libs/throw_exception libs/move libs/detail libs/preprocessor libs/type_traits libs/winapi libs/predef libs/mpl libs/utility libs/container_hash libs/io  libs/ratio libs/rational libs/typeof
# the following are undocument but needed for tests!
RUN git submodule update libs/numeric/conversion libs/conversion libs/function_types libs/tuple libs/fusion libs/variant libs/type_index libs/smart_ptr libs/optional
RUN ./bootstrap.sh --with-libraries=chrono,system,timer
RUN sed -i "/using gcc/c\using gcc : arm : $TOOLCHAIN-g++ ;" project-config.jam
RUN ./b2 -toolset=$TOOLCHAIN address-model=32 architecture=arm --prefix=$STAGING_DIR/usr/local link=static install

# install gtest
RUN apt-get install libgtest-dev
WORKDIR /usr/src/gtest
RUN DESTDIR=$STAGING_DIR cmake CMakeLists.txt
RUN make 

# note: we need a hash from branch cpp_master
WORKDIR /tmp/msgpack-c 
RUN git checkout 6b6a05e07cbadd4332bf16d48a09efb997756e4b
# note: cmake version must always be newer than boost version
RUN BOOST_ROOT=$STAGING_DIR/usr/local/ CXXFLAGS="-I${STAGING_DIR}/usr/local/include" /tmp/cmake/bin/cmake "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE" "-DCMAKE_FIND_ROOT_PATH=$STAGING_DIR" .
RUN DESTDIR=${STAGING_DIR} make install toolset=gcc-arm

# install libusb dependency: libudev

# see: https://ubuntu.pkgs.org/14.04/ubuntu-main-armhf/libudev-dev_204-5ubuntu20_armhf.deb.html
RUN mkdir /tmp/libudev-dev
RUN wget http://ports.ubuntu.com/pool/main/s/systemd/libudev-dev_204-5ubuntu20_armhf.deb -O /tmp/libudev-dev.deb
RUN dpkg -x /tmp/libudev-dev.deb /tmp/libudev-dev
RUN mkdir -p $STAGING_DIR/usr/include && cp /tmp/libudev-dev/usr/include/libudev.h $STAGING_DIR/usr/include/libudev.h
RUN mkdir -p $STAGING_DIR/usr/lib/pkgconfig && cp /tmp/libudev-dev/usr/lib/arm-linux-gnueabihf/pkgconfig/libudev.pc $STAGING_DIR/usr/lib/pkgconfig/libudev.pc

# see: https://ubuntu.pkgs.org/14.04/ubuntu-main-armhf/libudev1_204-5ubuntu20_armhf.deb.html
RUN mkdir /tmp/libudev1
RUN wget http://ports.ubuntu.com/pool/main/s/systemd/libudev1_229-4ubuntu4_armhf.deb -O /tmp/libudev1.deb
RUN dpkg -x /tmp/libudev1.deb /tmp/libudev1
RUN mkdir -p $STAGING_DIR/usr/lib && cp /tmp/libudev1/lib/arm-linux-gnueabihf/libudev.so* $STAGING_DIR/usr/lib

#RUN echo 'deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports trusty main universe ' >> /etc/apt/sources.list
#RUN echo 'deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports trusty-updates main universe' >> /etc/apt/sources.list
#RUN apt-get install -y libusb-1.0.0-dev:armhf
#RUN dpkg -L libusb-1.0.0-dev
# install libusb v1
RUN git clone https://github.com/libusb/libusb.git /tmp/libusb && cd /tmp/libusb && git checkout b51c743e4210756a98f4f60c69a34745e4b27a55
WORKDIR /tmp/libusb
# note: we need to create a symlink with the lib name and .so suffix (unversioned) only - otherwise linker won't find it
RUN ln -s $STAGING_DIR/usr/lib/libudev.so.1 $STAGING_DIR/usr/lib/libudev.so
RUN NOCONFIGURE=1 ./autogen.sh
RUN C=$TOOLCHAIN-gcc ./configure --host=$TOOLCHAIN --enable-udev=yes --enable-shared CFLAGS="--sysroot=/opt/crosstool-ng/x-tools/arm-rpi-linux-gnueabihf/arm-rpi-linux-gnueabihf/sysroot -I$STAGING_DIR/usr/include" LDFLAGS="-L$STAGING_DIR/usr/lib"
RUN LDFLAGS="-L$STAGING_DIR/usr/lib" make
RUN make install DESTDIR=${STAGING_DIR}

# install dep for debian build
RUN apt-get install -y dh-make


# install aws-greengrass-sdk

COPY ./rpi-scripts/install-aws-greengrass-sdk.sh ./install-aws-greengrass-sdk.sh
RUN ./install-aws-greengrass-sdk.sh "${STAGING_DIR}" "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE" "-DCMAKE_FIND_ROOT_PATH=$STAGING_DIR"