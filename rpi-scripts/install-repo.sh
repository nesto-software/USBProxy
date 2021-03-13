#!/bin/bash
set -e

echo "This script is intended to configure the debian repository for the USB Proxy project and install the latest binary."
echo ""
echo "Setting up the APT repository which is hosted on S3..."

read -p 'AWS Access Key: '
echo "";
ACCESS_KEY_ID=${REPLY}

read -s -p 'AWS Secret Access Key (hidden input): '
echo "";
SECRET_ACCESS_KEY=${REPLY}

REGION=eu-central-1
BUCKET=nesto-debian-repo-devel
GPG_KEY_ID=92F91ABA4816493E
PKG_NAME=nesto-usbproxy
GPG_KEYSERVER=keys.openpgp.org

echo "Installing tools which are needed by APT to access S3..."
sudo apt-get update
sudo apt-get install -y apt-transport-s3

echo "Configuring the S3 transport for APT..."
echo -e "AccessKeyId = '$ACCESS_KEY_ID'\nSecretAccessKey = '$SECRET_ACCESS_KEY'\nRegion = '$REGION'\nToken = ''" | sudo tee /etc/apt/s3auth.conf

# note: please do not use nightly for production systems
echo "deb [trusted=yes] s3://$BUCKET main aws" | sudo tee -a /etc/apt/sources.list
echo "deb [trusted=yes] s3://$BUCKET nightly aws" | sudo tee -a /etc/apt/sources.list

echo "Setting up APT keys for our S3 repo..."
gpg --keyserver "$GPG_KEYSERVER" --receive-key "$GPG_KEY_ID"
gpg --export --armor "$GPG_KEY_ID" | sudo apt-key add -

echo "Updating the package list with the index from our S3 repo..."
sudo apt-get update

echo "Finally installing the latest version of our application..."
sudo apt-get install -y $PKG_NAME