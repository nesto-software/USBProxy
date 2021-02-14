#!/bin/bash
sudo apt-get install -y devscripts
git clone https://github.com/fsaintjacques/semver-tool /tmp/semver
(cd /tmp/semver && sudo make install)