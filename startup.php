<?php

$br = (php_sapi_name() == 'cli') ? '' : '<br>';
$module = 'startup';

include __DIR__.'/vendor/autoload.php';

_ns(NS_STRINGY);


__('Stringy')::create("afdsafsd")->toAscii();