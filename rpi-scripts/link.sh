#!/bin/bash

shopt -s globstar
(cd staging; rsync -r --relative --links ./usr/local/lib/** /)
