#!/usr/bin/env bash

set -e

pwd

echo "disabled startup"
# phpbrew ext disable startup # 如果 ext 出问题了，这个命令就无法执行了
rm -f $PHPBREW_ROOT/php/$PHPBREW_PHP/var/db/start.ini

echo "build"
make && make install

# echo "enabled startup"
# phpbrew ext enable startup
