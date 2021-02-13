FROM ghcr.io/nesto-software/nesto-usbproxy-deps:latest as nesto-custom-build
 
ENV STAGING_DIR /usr/raspberry-build/staging
ENV SYSROOT /opt/crosstool-ng/x-tools/arm-rpi-linux-gnueabihf/arm-rpi-linux-gnueabihf/sysroot
ENV ROOT_FS /usr/raspberry-build/rootfs

# checkout git repo for usb proxy project
WORKDIR /root
#RUN git clone https://github.com/nesto-software/USBProxy-legacy.git usbproxy
COPY ./src ./usbproxy/src
RUN cd ./usbproxy && mkdir -p src/build

# build the executable for usb-mitm using the crosstool chain
WORKDIR /root/usbproxy/src/build

RUN LDFLAGS="-L${STAGING_DIR}/usr/local/lib" \
    CFLAGS="-I${STAGING_DIR}/usr/local/include" \
    CXXFLAGS=$CFLAGS \
    PKG_CONFIG_PATH=$STAGING_DIR/usr/local/lib/pkgconfig \
    cmake \
    # note: following is needed to prefix pkg-config entries
    "-DCMAKE_PREFIX_PATH=$STAGING_DIR/usr/local" \
    "-DCMAKE_FIND_ROOT_PATH=$STAGING_DIR" \
    "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE" \
    "-DCMAKE_INSTALL_PREFIX=/usr" \
    "-DCMAKE_BUILD_TYPE=Release" \
    "-DUSE_LIBUSB1=1" \
    ..

RUN make

# install the executable and its dependencies properly into rootfs
RUN DESTDIR=${ROOT_FS} make install

# build the debian package
WORKDIR /root/usbproxy/src
RUN dpkg-buildpackage -d -aarmhf -tarm-rpi-linux-gnueabihf

WORKDIR /root/usbproxy