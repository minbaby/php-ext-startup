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

# git clone https://github.com/snare/voltron
sudo apt install gdb -f
git clone https://github.com/longld/peda.git ~/peda
echo "source ~/peda/peda.py" >> ~/.gdbinit
echo "DONE! debug your program with gdb and enjoy"