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
- **OS:** Linux
- **Distribution**: Raspberry Pi OS / Raspbian (we use Debian's packaging system)
- **Architecture**: armhf (we do not build for arm64 yet)
- **Build System**: crosstool-NG (we are cross-compiling using GitHub workflows)
- **Additional Plugins**: We implemented IPC capability using [ZeroMQ](http://zeromq.org/) to channel the data out to other applications (running Python or Node.js). The language bindings which were provided by the original project did not work for us (throwing segfaults).
- **Additional Packaging**: We provide an alternative version of the application as [AWS Greengrass Lambda Package](https://github.com/aws/aws-greengrass-core-sdk-c). [Greengrass](https://aws.amazon.com/de/greengrass/) can be used to run the application on IoT devices in production. It guarantees that the process is running isolated and does stuff similar to systemd, such as auto-restarting the application on failure. Furthermore, it is an integral part of delivering a secure transport into the AWS cloud.

> :information_source: **Supported Devices**: There are many more devices which are working with this application. You have to make sure the device has a USB port which can operate in client mode. OTG ports are usually capable of doing that. Make also sure your device is being added to the [list of device ids for GadgetFS](https://github.com/nesto-software/USBProxy/blob/master/src/Plugins/Hosts/GadgetFS_helpers.c#L188).


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
| **curl**  | `bash -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/USBProxy/master/scripts/install-from-release.sh)"` |
| **wget**  | `bash -c "$(wget -O- https://raw.githubusercontent.com/nesto-software/USBProxy/master/scripts/install-from-release.sh)"`   |

### Install via APT Package Manager (binary)

> :information_source: **Internal**: We cannot provide a public package repository at the moment. The access is thus restricted to project members and Nesto employees. Others should use the GitHub releases option above.


| Method    | Command                                                                                           |
|:----------|:--------------------------------------------------------------------------------------------------|
| **curl**  | `bash -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/USBProxy/master/rpi-scripts/install-repo.sh)"` |
| **wget**  | `bash -c "$(wget -O- https://raw.githubusercontent.com/nesto-software/USBProxy/master/rpi-scripts/install-repo.sh)"`   |

### Manually compile on x86-64 (source)
```bash
cd ./docker-crosstool-ng-arm
./build-binary.sh
```

The binary should be cross-compiled using a docker container and the result is placed in `docker-crosstool-ng-arm/bin`.

### Manually compile on armhf (source)
This option is the fastest for development.

#### Prepare Raspberry Pi and Connection to Laptop

1. Install rpi-imager: `sudo apt install rpi-imager`
2. Insert SD card into laptop
3. Start rpi-imager, choose SD card, choose *Raspberry Pi OS (Other)* -> *Raspberry Pi OS Lite (32-bit)* and flash
4. Mount the SD card on your laptop and set env variable $SD_BOOT to boot partition and $SD_DATA to data partition
5. Enable ssh for the pi by placing an empty file called *ssh* into boot partition: `touch ${SD_BOOT}/ssh`
6. Set a link-local IP for your pi by appending the following to *${SD_DATA}/etc/network/interfaces*:   
```
auto eth0
allow-hotplug eth0
iface eth0 inet static
address 169.254.100.1
netmask 255.255.255.0
gateway 169.254.100.2
```
7. Insert the SD card into your pi and connect the pi to your laptop using an ethernet cable
8. Boot your pi. It should be accessible via the link-local ip `169.254.100.1`.
9. Configure your laptop with a link-local ip (e.g. *169.254.100.2*) on the interface which is connected to the pi (e.g. a USB to ethernet adapter labeled *enx00e04c6b1c7b*):   
```
sudo ip addr add 169.254.100.2 dev enx00e04c6b1c7b
sudo route add -net 169.254.0.0 netmask 255.255.0.0 dev enx00e04c6b1c7b
```
10. Allow the pi to access the internet via your laptop by confguring your laptop accordingly as follows:   
```
# enable ip forwarding
sysctl -w net.ipv4.ip_forward=1

# enable ip masquerading on the interface which is used to access the internet (e.g. wlp59s0)
sudo iptables -t nat -A POSTROUTING -o wlp59s0 -j MASQUERADE
```
11. Connect to the pi using ssh: `ssh pi@169.254.100.1` using default password `raspberry`.
12. Check if your pi can access the internet via your laptop: `ping 8.8.8.8` and `ping google.de` (to check domain resolution)

#### Preparing the development environment
1. Install [Visual Studio Code](https://code.visualstudio.com/download)
2. Install the following extensions: `ms-vscode-remote.remote-ssh` and `ms-vscode-remote.remote-ssh-edit`
3. Restart VSCode
4. Open the default configuration file using Ctrl+Shift+P + *Remote-SSH: Open Configuration File...* -> Choose default config file in your user's home directory (i.e. *~/.ssh/config*) and paste the following:
```
Host Pi
  HostName 169.254.100.1
  User pi
```
5. Connect to the Pi using Ctrl+Shift+P + *Remote-SSH: Connect to Host...* -> Pi and with the default password *raspberry* (when prompted for it). This might take a while because VSCode will transfer a bundle to the pi and install everything that is needed for remote development.
6. Install remote VSCode extensions: `ms-vscode.cpptools`, `twxs.cmake`, `ms-vscode.cmake-tools`
7. Open a new terminal on the remote device using Ctrl+Shift+` and use it for subsequent Linux commands
8. Optional: Create a personal access token to be able to clone the USB Proxy repo and push to it. The token needs the *public_repo* scope.
9. Clone the USB Proxy repository from GitHub: `git clone https://${TOKEN}:x-oauth-basic@github.com/nesto-software/USBProxy.git`
10. Open the folder inside the explorer using Ctrl+Shift+E -> */home/pi/USBProxy/*

#### Installing dependencies
- Run `./rpi-scripts/install-all-dependencies.sh`

#### Compiling
1. Click on the Build button in the bottom VSCode task bar
2. Choose a kit (e.g. GCC 7.5.0)
3. Wait for the build to finish

If the build finished without errors, you could try to install and run the binary.


#### Install and run (with debugger attached)
1. Connect a host device to the raspberry pi's USB C port. This could be another Linux computer or even the same device which you are using for remote development.
2. Connect a client device to one of the raspberry pi's USB A ports. This could be a USB keyboard for example.
3. Find out the keyboard's USB vendor and product ID:   
```bash
sudo apt install usbutils
sudo lsusb -v
```
4. The header of the device descriptor looks something like this: `Bus 001 Device 004: ID 045e:07f8 Microsoft Corp. Wired Keyboard 600 (model 1576)` with *045e* being the vendor id and *07f8* being the product id.
5. Open `.vscode/launch.json` and adjust the *-v* and *-p* arguments in L14 with the values obtained from step 4. Example given in L13.
6. Open the *Run and Debug* view using Ctlr+Shift+D and start the *Install + Run* launch configuration

Please note that you must run the application with root privileges. The launch configuration takes care of that for you.

If you want to run the script on your own, make sure to run the install task before (i.e. Ctrl+P -> *task install* -> Enter -> Enter). This is needed to copy shared libraries into appropriate system folders. You can run the binary from the repository root by doing: `./src/build/tools/usb-mitm --help`. Do not forget to use **sudo** when running anything other than the help menu view. We need root permissions to access the usb subsystem and read from devices.

> :warning: **Linking Libraries**: If you want to run the globally installed executable, you must make sure that your dynamic linker is up-to-date. Please run `sudo ldconfig` to ensure libraries can be linked correctly at runtime.

Usage
---------
```
usb-mitm - command line tool for controlling USBProxy
Usage: ./src/build/tools/usb-mitm [OPTIONS]
Options:
    -v <vendorId> VendorID of target device
    -p <productId> ProductID of target device
    -P <PluginName> Use PluginName (order is preserved)
    -D <DeviceProxy> Use DeviceProxy
    -H <HostProxy> Use HostProxy
    -d Enable debug messages (-dd for increased verbosity)
    -s Server mode, listen on port 10400
    -c <hostname | address> Client mode, connect to server at hostname or address
    -l Enable stream logger (logs to stderr)
    -i Enable UDP injector
    -x Enable Xbox360 UDPHID injector & filter
    -k Keylogger with ROT13 filter (for demo), specify optional filename to output to instead of stderr
    -w <filename> Write to pcap file for viewing in Wireshark
    -h Display this message
```

There is a new option `-z` which registers the ZeroMQ filter.

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
<b>Keyserver: <a target="_blank" href="https://keys.openpgp.org/search?q=F1C6636C27019FD0D29307DEAE25CBF30C0DDB0C">keys.openpgp.org</a></b>

IPC Example
---------
We provided a sample application for Node.js in the *./nodejs-client* folder.   
The sample application connects to the USB Proxy and receives data which is read from the USB relaying.

You can run the example by doing:
1. Start the usb-mitm application, e.g. using `./scripts/usb-mitm.sh` if you already built it. Please make sure to adjust the vendor and product ids in the shell script beforehand.
2. Install Node.js binary from [nodejs.org](https://nodejs.org/en/download/) or via nvm.
3. Install Node.js dependencies:
```bash
cd nodejs-client
npm install
```
4. Run the script: `node ./nodejs-client/index.js`
5. You should see that the application receives buffers once data is transferred between your USB device and the host. In case you are using a USB keyboard as test device, you should see an incoming buffer for each keydown and keyup event.

Development
----------

We use the following Git Feature-Branch-Workflow:

The master branch is used to build stable releases. The code in master must always compile. Only project maintainers are allowed to merge into master via a PR. Merging into master is allowed from dev branch only. Merging into master usually results into a new release version when files inside the *src* folder are modified.

The dev branch is used to prepare a release. Developers are expected to merge or rebase their branch with dev frequently. The dev branch is used to build nightly releases. The code in dev should always compile. Merging into dev is allowed from all feature branches and requires a PR which must be approved by at least one project maintainer. Merging into dev results into an instant nightly release when files inside the *src* folder are modified.

Developers are expected to fork the repository and to work on their own feature branches. Once the work is done, please submit a PR into dev branch. We will merge into master and create a release as soon as possible.

Building a Release (for Maintainers)
----------

1. Switch to dev branch and pull
2. `./.github/create-release.sh (major|minor|patch)`
3. git add -A && git commit && git push
4. Create a PR into master and describe the changes; Make sure to squash the commits.

Use a commit message like: `chore: prepare release for vx.y.z`.   
Use a PR title like: `chore: release vx.y.z`.
