#!/usr/bin/env bash

set -e

pwd

# echo "disable ext startup.so"
phpbrew ext disable startup
./vendor/bin/kahlan

# echo "enable ext start.so"
phpbrew ext enable startup
./vendor/bin/kahlan



