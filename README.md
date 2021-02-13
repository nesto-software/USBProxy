USBProxy   
![.github/workflows/build-app-nightly.yaml](https://github.com/nesto-software/USBProxy/workflows/.github/workflows/build-app-nightly.yaml/badge.svg?branch=dev)
========

Status
------
This project is currently being refactored by Nesto.   
If you want to participate, feel free to reach out!

Martin Löper `<martin.loeper@nesto-software.de>`

Install
-------

In order to install USBProxy on your Raspberry Pi, please use the following snippet.
You must add AWS credentials at the top of the file in advance.

```bash
#!/bin/bash

# set AWS credentials to access S3 bucket which hosts the debian repository
ACCESS_KEY_ID=
SECRET_ACCESS_KEY=

REGION=eu-central-1
BUCKET=nesto-debian-repo-devel
GPG_KEY_ID=92F91ABA4816493E
PKG_NAME=nesto-usbproxy
DISTRIBUTION=main   # main or nightly

sudo apt-get update
sudo apt-get install apt-transport-s3
echo -e "AccessKeyId = '$ACCESS_KEY_ID'\nSecretAccessKey = '$SECRET_ACCESS_KEY'\nRegion = '$REGION'\nToken = ''" > /etc/apt/s3auth.conf
echo "deb s3://$BUCKET $DISTRIBUTION aws" >> /etc/apt/sources.list
gpg --keyserver keys.openpgp.org --receive-key "$GPG_KEY_ID"
gpg --export --armor "$GPG_KEY_ID" | apt-key add -
sudo apt-get update
sudo apt-get install $PKG_NAME
```