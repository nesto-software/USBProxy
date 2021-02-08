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

IMPORTANT: merge changes from... (seee: https://techgaun.github.io/active-forks/index.html#usb-tools/USBProxy-legacy)
- https://github.com/mweal-ed/USBProxy/commits/master
- https://github.com/foxmiti/USBProxy-legacy/commit/fe91833ea886a7a3dd38a6f647fc8e172427950d
- https://github.com/retrospy/USBProxy ???

Issues:
- What about dh_shlibdeps?
- Build a new docker package on github push and publish to S3 via GitHub actions.
- check copyright for IPC: https://opensource.stackexchange.com/questions/7492/ipc-between-open-source-and-closed-source-applications?newreg=8e5e8eb3cc58402eb59f0ff60748fa72