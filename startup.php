<?php
include __DIR__ . '/vendor/autoload.php';

_ns(NS_STRINGY);

// $data = [
//     ['camelCase', 'CamelCase'],
//     ['camelCase', 'Camel-Case'],
//     ['camelCase', 'camel case'],
//     ['camelCase', 'camel -case'],
//     ['camelCase', 'camel - case'],
//     ['camelCase', 'camel_case'],
//     ['camelCTest', 'camel c test'],
//     ['stringWith1Number', 'string_with1number'],
//     ['stringWith22Numbers', 'string-with-2-2 numbers'],
//     ['dataRate', 'data_rate'],
//     ['backgroundColor', 'background-color'],
//     ['yesWeCan', 'yes_we_can'],
//     ['mozSomething', '-moz-something'],
//     ['carSpeed', '_car_speed_'],
//     ['serveHTTP', 'ServeHTTP'],
//     ['1Camel2Case', '1camel2case'],
//     ['camelΣase', 'camel σase', 'UTF-8'],
//     ['στανιλCase', 'Στανιλ case', 'UTF-8'],
//     ['σamelCase', 'σamel  Case', 'UTF-8']
// ];

// foreach($data as $value) {
//     @list($expected, $str, $encoding) = $value;
//     $x = __('Stringy')::create($str, $encoding)->lowerCaseFirstY();
//     var_dump($x);
// }

$x = __('Stringy')::create("")->camelize();
var_dump(get_class_methods($x));
