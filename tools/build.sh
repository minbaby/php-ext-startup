#!/usr/bin/env bash


pwd

# echo "rm startup.so"
# rm -rf `phpbrew path ext/startup.so`

echo "disabled startup"
phpbrew ext disable startup

echo "go..."
phpize && ./configure && make && make install && phpize --clean

echo "enabled startup"
phpbrew ext enable startup
