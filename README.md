USB Proxy for Raspberry Pi (armhf)   
========

<p align="center">
  <img src=".github/imgs/project_logo.png">
</p>

![.github/workflows/build-app-nightly.yaml](https://github.com/nesto-software/USBProxy/workflows/.github/workflows/build-app-nightly.yaml/badge.svg?branch=dev)
![.github/workflows/build-app-release.yaml](https://github.com/nesto-software/USBProxy/workflows/.github/workflows/build-app-release.yaml/badge.svg)


Heads Up!
------
This project is currently being refactored by Nesto.   
If you want to participate, feel free to reach out!
 
Martin Löper `<martin.loeper@nesto-software.de>`

Mission
-------
Nesto is developing an [IoT solution to interface Point of Sale (POS) systems](https://nesto-software.de/datenintegration/) for the German gastronomy. One strategy of integrating legacy systems on the market, is to observe the traffic between terminal and its printer. Printers are either connected via Ethernet or USB. We strive for a reliable software solution for mirroring print jobs using a Raspberry Pi as a USB proxy device.
That is, the POS system is connected to the Raspberry Pi which in turn is connected to the printer. The Raspberry Pi is running additional software to export the obtained data securely into the cloud.

We want to share our progress on this project with the open-source community as we [forked the original codebase](https://github.com/usb-tools/USBProxy-legacy) which is under the GPL-2.0.


Our Setup
-------
- Tested Devices: [Raspberry Pi 4B](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/specifications/)
- Distributions: Raspberry Pi OS / Raspbian (we use Debian's packaging system)
- Architecture: armhf (we do not build for arm64 yet)
- Build System: crosstool-NG (we are cross-building using GitHub workflows)
- Additional Plugins: We implemented IPC capability using [ZeroMQ](http://zeromq.org/) to channel the data out to other applications (running Python or Node.js). The language bindings which were provided by the original project did not work for us (throwing segfaults).
- Additional Packaging: We provide an alternative version of the application as [AWS Greengrass Lambda Package](https://github.com/aws/aws-greengrass-core-sdk-c). [Greengrass](https://aws.amazon.com/de/greengrass/) can be used to run the application on IoT devices in production. It guarantees that the process is running isolated and does stuff similar to systemd, such as auto-restarting the application on failure. Furthermore it is an integral part of delivering a secure transport into the AWS cloud.

> :information_source: **Supported Devices**: There are many devices which are working with this application. You have to make sure the device has a USB port which can operate in client mode. OTG ports are usually capable of doing that. Make also sure your device is being added to the [list of device ids for GadgetFS](https://github.com/nesto-software/USBProxy/blob/master/src/Plugins/Hosts/GadgetFS_helpers.c#L188).



Install
-------

In order to install USBProxy on your Raspberry Pi, please use the following snippet.
You must add AWS credentials at the top of the file in advance.

```bash
#!/bin/bash
set -e

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

```bash
#!/bin/bash
set -e

FILE=/tmp/nesto-usbproxy-latest.deb

curl -s https://api.github.com/repos/nesto-software/USBProxy/releases/latest \
| grep "browser_download_url.*deb" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -qi - -O "$FILE"

sudo dpkg -i "$FILE"
```

| Method    | Command                                                                                           |
|:----------|:--------------------------------------------------------------------------------------------------|
| **curl**  | `sh -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/USBProxy/master/scripts/install-from-release.sh)"` |
| **wget**  | `sh -c "$(wget -O- https://raw.githubusercontent.com/nesto-software/USBProxy/master/scripts/install-from-release.sh)"`   |

GPG
---------

#### Add our key to your keychain!

We use [GPG](https://de.wikipedia.org/wiki/GNU_Privacy_Guard) to sign our binary releases.
In order to install packages from internal repositories, you must add our key for SecureApt to work.
The GitHub releases do not provide signatures - just download the respective .deb file and you are ready to go.

<a target="_blank" href="https://keyoxide.org/F1C6636C27019FD0D29307DEAE25CBF30C0DDB0C" rel="Nesto Cloud Operations">![Nesto Cloud Operations](.github/imgs/gpg_qr.svg)</a> 

<img align="left" src=".github/imgs/openkeychain.png" width="50px">   
<a target="_blank" href="https://www.openkeychain.org/">Download OpenKeychain for Android</a><br />
<a target="_blank" href="https://gnupg.org/download/">Download GNU Privacy Guard for Linux</a>
<br clear="both">
<br />
<b>Keyserver: <a target="_blank" href="https://keys.openpgp.org/">keys.openpgp.org</a></b>