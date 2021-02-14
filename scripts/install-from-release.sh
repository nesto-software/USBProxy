#!/bin/bash
set -e

FILE=/tmp/nesto-usbproxy-latest.deb

curl -s https://api.github.com/repos/nesto-software/USBProxy/releases/latest \
| grep "browser_download_url.*deb" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -qi - -O "$FILE"

sudo dpkg -i "$FILE"