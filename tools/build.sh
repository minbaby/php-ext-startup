#!/usr/bin/env bash


pwd

echo "rm startup.so"
rm -rf `phpbrew path ext/startup.so`

echo "..."
phpize && ./configure && make && make install && phpize --clean
