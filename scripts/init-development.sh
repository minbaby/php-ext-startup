#!/usr/bin/env bash

set -e -x

export PATH=/usr/bin:$PATH

export PHP_VERSION=php-7.1.23

export SYSTEM_NAME=`uname`

[ "$SYSTEM_NAME" == "Darwin" ] && LIB_BZ2='/usr/local/opt/bzip2/'
[ "$SYSTEM_NAME" == "Darwin" ] && LIB_ZIP='/usr/local/opt/zlib/'
[ "$SYSTEM_NAME" == "Darwin" ] && LIB_SSL='/usr/local/opt/openssl/'

# sudo apt install libxml2-dev
# sudo apt install libssh-dev
# sudo apt install libbz2-dev
# sudo apt install libcurl4-openssl-dev
# sudo apt install libmcrypt-dev
# sudo apt install libreadline-dev
# sudo apt install libxslt1-dev 
# sudo apt install autoconf

phpbrew --debug install \
    --mirror=http://cn2.php.net -j 4 \
    $PHP_VERSION \
    +default \
    +bz2=$LIB_BZ2 \
    +zlib=$LIB_ZIP \
    +mb \
    +openssl=$LIB_SSL +intl \
    +mysql \
    +pdo \
    +xml

echo $PHP_VERSION

phpbrew switch $PHP_VERSION
