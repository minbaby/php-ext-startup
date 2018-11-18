#!/usr/bin/env bash

set -e
set -x

pwd

echo "disabled startup"
# phpbrew ext disable startup
rm -f $PHPBREW_ROOT/php/$PHPBREW_PHP/var/db/start.ini

echo "go..."
phpize && ./configure && make && make install && make clean && phpize --clean

echo "enabled startup"
phpbrew ext enable startup
