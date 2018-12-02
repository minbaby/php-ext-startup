# #!/usr/bin/env bash

# set -e
# set -x

# source your_php_src_path/.gdbinit
# gdb php -f startup.php

# [[ ! -e ~/.gdbinit ]]&& cp　~/.gdbinit ~/.gdbinit.bak

# ln -sf $PHPBREW_HOME/build/$PHPBREW_PHP/.gdbinit ~/.gdbinit

# ulimit -c unlimited

# MINBABY_TEST_EXT=1 php -f startup.php

# ulimit -c 0

# gdb -c core php

b src/ext/stringy.c:665
b /home/minbaby/.phpbrew/build/php-7.1.23/Zend/zend_execute_API.c:782
