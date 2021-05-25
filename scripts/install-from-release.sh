#!/bin/bash
set -e

FILE=/tmp/nesto-usbproxy-latest.deb
RELEASE=v0.1.0

echo "Downloading .deb file from latest GitHub release..."
curl -s https://api.github.com/repos/nesto-software/USBProxy/releases/tags/$RELEASE \
| grep "browser_download_url.*deb" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -qi - -O "$FILE"

echo "Installing .deb file..."
sudo dpkg -i "$FILE"
