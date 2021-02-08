#!/bin/bash

if [ -z "$1" ]; then
    echo "Pass AccessKeyId as first parameter."
    exit 1
fi

if [ -z "$2" ]; then
    echo "Pass nSecretAccessKey as second parameter."
    exit 1
fi

sudo apt-get update
sudo apt-get install apt-transport-s3
echo -e "AccessKeyId = '$1'\nSecretAccessKey = '$2'\nRegion = 'eu-central-1'\nToken = ''" > /etc/apt/s3auth.conf
echo "deb s3://nesto-debian-repo-devel unofficial local" >> /etc/apt/sources.list
gpg --keyserver keys.openpgp.org --receive-key 92F91ABA4816493E
gpg --export --armor "92F91ABA4816493E" | apt-key add -
sudo apt-get update
sudo apt-get install nesto-usbproxy