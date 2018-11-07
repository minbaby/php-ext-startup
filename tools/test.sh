#!/usr/bin/env bash


pwd

echo "build && install";
phpize && ./configure && make && make install && phpize --clean
