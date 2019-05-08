<?php

$br = (php_sapi_name() == 'cli') ? '' : '<br>';
$module = 'startup';

include __DIR__.'/vendor/autoload.php';

_ns(NS_STRINGY);


echo __('Stringy')::create("afds\tafsd")->toSpaces();
