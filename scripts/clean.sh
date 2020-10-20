#!/usr/bin/env bash

set -e
set -x

echo "clean..."
[[ -f Makefile ]] && make clean && phpize --clean
