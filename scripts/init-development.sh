#!/usr/bin/env bash

set -e -x

export PATH=/usr/bin:$PATH

export PHP_VERSION=php-7.1.33

export SYSTEM_NAME=`uname`

export SCRIPT_DIR=`dirname $0`

[ "$SYSTEM_NAME" == "Darwin" ] && LIB_BZ2='=/usr/local/opt/bzip2/'
[ "$SYSTEM_NAME" == "Darwin" ] && LIB_ZIP='=/usr/local/opt/zlib/'
[ "$SYSTEM_NAME" == "Darwin" ] && LIB_SSL='=/usr/local/opt/openssl/'

if [ ! -x "$(command -v icu-config)" ]; then
    sudo cp $SCRIPT_DIR/resources/icu-config /usr/bin/icu-config
fi

if [ "$SYSTEM_NAME" == "Linux" ]; then
    sudo apt install \
        libxml2-dev \
        libssh-dev \
        libbz2-dev \
        libcurl4-openssl-dev \
        libmcrypt-dev \
        libreadline-dev \
        libxslt1-dev \
        autoconf \
        valgrind
fi

phpbrew --debug install \
    -j 4 \
    $PHP_VERSION \
    +default \
    +bz2$LIB_BZ2 \
    +zlib$LIB_ZIP \
    +mb \
    +openssl$LIB_SSL \
    +intl \
    +mysql \
    +pdo \
    +xml

echo $PHP_VERSION

phpbrew switch $PHP_VERSION
