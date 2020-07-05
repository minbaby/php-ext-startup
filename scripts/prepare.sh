#!/usr/bin/env bash

set -e

echo "prepare..."
phpize && ./configure --enable-debug
