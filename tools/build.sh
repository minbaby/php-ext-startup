#!/usr/bin/env bash

set -e
set -x

pwd

echo "disabled startup"
phpbrew ext disable startup

echo "go..."
phpize && ./configure && make && make install && phpize --clean

echo "enabled startup"
phpbrew ext enable startup
