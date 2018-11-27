#!/usr/bin/env bash

set -e
set -x

echo "clean..."
make clean && phpize --clean
