#!/bin/bash

shopt -s globstar
(cd staging/rootfs; rsync -r --relative --links ./usr/local/lib/** /)
