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

Scope
-------
- **Tested Device**: [Raspberry Pi 4B](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/specifications/)
- **Distribution**: Raspberry Pi OS / Raspbian (we use Debian's packaging system)
- **Architecture**: armhf (we do not build for arm64 yet)
- **Build System**: crosstool-NG (we are cross-building using GitHub workflows)
- **Additional Plugins**: We implemented IPC capability using [ZeroMQ](http://zeromq.org/) to channel the data out to other applications (running Python or Node.js). The language bindings which were provided by the original project did not work for us (throwing segfaults).
- **Additional Packaging**: We provide an alternative version of the application as [AWS Greengrass Lambda Package](https://github.com/aws/aws-greengrass-core-sdk-c). [Greengrass](https://aws.amazon.com/de/greengrass/) can be used to run the application on IoT devices in production. It guarantees that the process is running isolated and does stuff similar to systemd, such as auto-restarting the application on failure. Furthermore, it is an integral part of delivering a secure transport into the AWS cloud.

> :information_source: **Supported Devices**: There are many more devices which are working with this application. You have to make sure the device has a USB port which can operate in client mode. OTG ports are usually capable of doing that. Make also sure your device is being added to the [list of device ids for GadgetFS](https://github.com/nesto-software/USBProxy/blob/master/src/Plugins/Hosts/GadgetFS_helpers.c#L188).



Getting Started
-------

## Installation

There are 4 installation methods:
- Binaries uploaded to GitHub releases (public; production ready)
- Binaries uploaded to Debian repository on S3 (Nesto-internal; production & nightly builds)
- Manually cross-compile source code (using code in *./docker-crosstool-ng-arm* folder)
- Manually compile source code on the Raspberry Pi using [VS Code Remote Development](https://code.visualstudio.com/docs/remote/remote-overview)

We provide instructions for each method in the following.

### Install via GitHub Releases Download (binary)

| Method    | Command                                                                                           |
|:----------|:--------------------------------------------------------------------------------------------------|
| **curl**  | `sh -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/USBProxy/master/scripts/install-from-release.sh)"` |
| **wget**  | `sh -c "$(wget -O- https://raw.githubusercontent.com/nesto-software/USBProxy/master/scripts/install-from-release.sh)"`   |

### Install via APT Package Manager (binary)

> :information_source: **Internal**: We cannot provide a public package repository at the moment. The access is thus restricted to project members and Nesto employees. Others should use the GitHub releases option above.


| Method    | Command                                                                                           |
|:----------|:--------------------------------------------------------------------------------------------------|
| **curl**  | `sh -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/USBProxy/master/rpi-scripts/install-repo.sh)"` |
| **wget**  | `sh -c "$(wget -O- https://raw.githubusercontent.com/nesto-software/USBProxy/master/rpi-scripts/install-repo.sh)"`   |

### Manually compile on x86-64 (source)
```bash
cd ./docker-crosstool-ng-arm
./build-binary.sh
```

The binary should be cross-compiled using a docker container and the result is placed in `docker-crosstool-ng-arm/bin`.

### Manually compile on armhf (source)
This option is the fastest for development.

> TBD: describe how to use VS Code Remote development


Usage
---------

> TBD: show cli parameters
> TBD: note that -n flag is new and registers a filter


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

IPC Example
---------
We provided a sample application for Node.js in the *./nodejs-client* folder.   
The sample application connects to the USB Proxy and receives data which is read from the USB relaying.

Development
----------

> TBD: we use master, dev and feature branches
> TBD: we publish stable versions to main debian dist and daily builds from dev branch to nighty debian distribution (both are hosted internally at Nesto)

Building a Release (for Maintainer)
----------

> TBD