FROM debian@sha256:7f2706b124ee835c3bcd7dc81d151d4f5eca3f4306c5af5c73848f5f89f10e0b

RUN apt-get update && \
apt-get install gnupg=2.1.18-8~deb9u4 gnupg2 -y && \
apt-get clean

RUN echo "deb http://repo.aptly.info/ squeeze main" > /etc/apt/sources.list.d/aptly.list && \
apt-key adv --keyserver pool.sks-keyservers.net --recv-keys ED75B5A4483DA07C && \
apt-get update && \
apt-get install aptly=1.4.0 ca-certificates=20200601~deb9u1 -y && \
apt-get clean

ADD ./aptly/aptly.conf /etc/aptly.conf
ADD ./docker-crosstool-ng-arm/bin/nesto-usbproxy_0.0.1_armhf.deb /root/usb-proxy/

ENV APTLY_REPO nesto-pos-adapter-devel
RUN aptly repo create $APTLY_REPO
RUN aptly repo add $APTLY_REPO /root/usb-proxy/nesto-usbproxy_0.0.1_armhf.deb

ADD ./aptly/private.asc /root/usb-proxy/private.asc
ADD ./aptly/public.asc /root/usb-proxy/public.asc

RUN gpg --batch -v --import /root/usb-proxy/private.asc
