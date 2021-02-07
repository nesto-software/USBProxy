# TODO

- list all deps for .deb (dynamic libraries)
- link libzmq statically (remove all -lzmq)
- copy all relevant files to debian/tmp

- search for mem leaks
- investigate destructor in zeromq plugin
- test usb device hotplugging
- find out why usb-mitm crashes sometimes on exit
- add debian postinst scripts to set up drw2 usb otg if missing
- integrate aws lambda greengrass sdk for c --> publish as lambda package (different build target)

- write proper README:
  - configure rpi OTG port
  - install stuff: see install-* scripts
  - use vscode builtins: build button, Install + Run, sigterm/sigkill

IMPORTANT: merge changes from https://github.com/mweal-ed/USBProxy/commits/master