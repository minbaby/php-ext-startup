#!/usr/bin/env bash

set -e
set -x

echo "prepare..."
phpize && ./configure --enable-debug
