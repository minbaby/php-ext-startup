#!/usr/bin/env bash

set -e
set -x

pwd

# echo "disable ext startup.so"
phpbrew ext disable startup
export MINBABY_TEST_EXT=0
./vendor/bin/kahlan

# echo "enable ext start.so"
phpbrew ext enable startup
export MINBABY_TEST_EXT=1
./vendor/bin/kahlan
