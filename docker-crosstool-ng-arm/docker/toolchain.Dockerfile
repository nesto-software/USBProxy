FROM ubuntu:14.04.2 as crosstool-ng_install
LABEL org.opencontainers.image.source https://github.com/nesto-software/USBProxy

# from: https://github.com/amclain/docker-crosstool-ng/blob/master/Dockerfile
# and: https://crosstool-ng.github.io/docs/install/

ENV CROSSTOOL crosstool-ng-1.24.0

# Install system packages
RUN apt-get -qq update
RUN apt-get -y dist-upgrade
RUN apt-get -y install wget curl
RUN apt-get -y install git
RUN apt-get -y install build-essential
RUN apt-get -y install automake
RUN apt-get -y install libtool
RUN apt-get -y install gawk
RUN apt-get -y install bison
RUN apt-get -y install flex
RUN apt-get -y install texinfo
RUN apt-get -y install gperf
RUN apt-get -y install libncurses5-dev
RUN apt-get -y install libexpat1-dev
RUN apt-get -y install subversion
RUN apt-get -y install unzip
RUN apt-get -y install help2man

WORKDIR /opt

# Install crosstool-ng
RUN curl -s http://crosstool-ng.org/download/crosstool-ng/${CROSSTOOL}.tar.bz2 | tar -xj

WORKDIR ${CROSSTOOL}
RUN ./configure --prefix=/opt/crosstool-ng
RUN make
RUN make install
ENV PATH="${PATH}:/opt/crosstool-ng/bin"

WORKDIR /

FROM crosstool-ng_install AS crosstool-ng_build-toolchain

# choose correct triplet
# see: https://wiki.osdev.org/Target_Triplet
# raspberry arm versions (see: https://wiki.debian.org/RaspberryPi and https://www.raspbian.org/RaspbianFAQ "What compilation options should be set Raspbian code?")
# - armhf for rev. 2
#
# cross compilers:
# armhf: arm-linux-gnueabihf
# we use the config from following snippet to create our own toolchain: 
# https://raw.githubusercontent.com/rvagg/rpi-newer-crosstools/master/x64-gcc-4.9.4-binutils-2.28.config
ENV TOOLCHAIN arm-rpi-linux-gnueabihf

WORKDIR /opt/crosstool-ng

# Build ARM toolchain
COPY ./docker-crosstool-ng-arm/config/arm.config .config

# apply download server fix from https://github.com/pfalcon/esp-open-sdk/issues/306
COPY "./docker-crosstool-ng-arm/fixes/140-mpc.sh" "/opt/crosstool-ng/lib/ct-ng.1.20.0/scripts/build/companion_libs/140-mpc.sh" 

# note: the following is needed because I copy-pasted the crosstool-ng config and updated the crosstool-ng version afterwards
RUN ct-ng upgradeconfig

# build the toolchain
RUN ct-ng build CT_ALLOW_BUILD_AS_ROOT_SURE=true

# add the toolchain to the path
ENV PATH="${PATH}:/opt/crosstool-ng/x-tools/${TOOLCHAIN}/bin"

WORKDIR /