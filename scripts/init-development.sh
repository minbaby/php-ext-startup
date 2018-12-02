#!/usr/bin/env bash

set -e -x

export PATH=/usr/bin:$PATH

export PHP_VERSION=php-7.1.23

export SYSTEM_NAME=`uname`

[ "$SYSTEM_NAME" == "Darwin" ] && LIB_BZ2='+bz2=/usr/local/opt/bzip2/'
[ "$SYSTEM_NAME" == "Darwin" ] && LIB_ZIP='+zlib=/usr/local/opt/zlib'

phpbrew --debug install --mirror=http://cn2.php.net -j 4 $PHP_VERSION +default $LIB_BZ2 $LIB_ZIP +mb +openssl=/usr/local/opt/openssl/

echo $PHP_VERSION

phpbrew switch $PHP_VERSION
