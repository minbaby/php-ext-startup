#!/usr/bin/env php
<?php

$data = [
    'pht'           => 'https://github.com/tpunt/pht',
    'php-ds'        => 'https://github.com/php-ds/extension.git',
    'phactor'       => 'https://github.com/tpunt/phactor',
    'phpredis'      => 'https://github.com/phpredis/phpredis',
    'yaf'           => 'https://github.com/laruence/yaf.git',
    'yar'           => 'https://github.com/laruence/yar.git',
    'yac'           => 'https://github.com/laruence/yac.git',
    'yaconf'        => 'https://github.com/laruence/yaconf.git',
    'yaf'           => 'https://github.com/laruence/yaf.git',
    'xhprof'        => 'https://github.com/phacility/xhprof',
    'msgpack-php'   => 'https://github.com/msgpack/msgpack-php',
    'phptrace'      => 'https://github.com/Qihoo360/phptrace',
    'php-memcached' => 'https://github.com/php-memcached-dev/php-memcached',
    'swoole'        => 'https://github.com/swoole/swoole-src',
    'pthreads'      => 'https://github.com/krakjoe/pthreads',
    'SeasLog'       => 'https://github.com/SeasX/SeasLog',
    'phpdbg'        => 'https://github.com/krakjoe/phpdbg',
    'php-rdkafka'   => 'https://github.com/arnaud-lb/php-rdkafka',
    'runkit'        => 'https://github.com/zenovich/runkit',
    'php-git'       => 'https://github.com/libgit2/php-git',
    'AOP'           => 'https://github.com/AOP-PHP/AOP',
    'xdeubg'        => 'https://github.com/xdebug/xdebug.git'
];

// PHPBREW_ROOT/build/php-7.1.17/ext/

$path = sprintf("%s/build/%s/ext", $_SERVER['PHPBREW_ROOT'], $_SERVER['PHPBREW_PHP']);

foreach($data as $key => $value) {
    if (file_exists("{$path}/{$key}")) {
        echo "[INFO] 有了 {$path}/{$key}", PHP_EOL;
        continue;
    }
    echo "[INFO] clone repo:[", $key, "]", PHP_EOL;
    echo "[INFO] path:[", $path, "]", PHP_EOL;
    `cd $path && git clone $value $key && cd -`;
}
