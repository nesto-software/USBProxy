# TODO

- list all deps for .deb (dynamic libraries)
- link libzmq statically (remove all -lzmq)
- copy all relevant files to debian/tmp

- search for mem leaks
- investigate destructor in zeromq plugin
- test usb device hotplugging
- find out why usb-mitm crashes sometimes on exit
- setup cross-toolchain, see: https://github.com/usb-tools/USBProxy-legacy/wiki

- write proper README:
  - configure rpi OTG port
  - install stuff: see install-* scripts
  - use vscode builtins: build button, Install + Run, sigterm/sigkill