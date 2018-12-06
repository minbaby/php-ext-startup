<?php
$br = (php_sapi_name() == "cli")? "":"<br>";
$module = 'startup';

function handleFunc()
{
    global $br, $module;

    $function = 'confirm_' . $module . '_compiled';
    if (extension_loaded($module)) {
        $str = $function($module);
    } else {
        $str = "Module $module is not compiled into PHP";
    }
    echo "$str\n";
}

function handleClass()
{
    if (class_exists('myclass')) {
        echo 'myclass exists';
     } else {
        echo 'myclass exists';
     }

     $class = new myclass();
     var_dump($class);
     $class->abab();
}

function checkExt()
{
    global $br, $module;
    if (!extension_loaded('startup')) {
        dl('startup.' . PHP_SHLIB_SUFFIX);
    }
    $module = 'startup';
    $functions = get_extension_funcs($module);
    echo "Functions available in the test extension:$br\n";
    foreach ($functions as $func) {
        echo $func . "$br\n";
    }
    echo "$br\n";
}

include __DIR__ . '/vendor/autoload.php';
// include __DIR__ . "/spec/const.php";
// include __DIR__ . "/spec/functions.php";

// function main() {
//     _ns(NS_STRINGY);
//     $stringy = __('Stringy')::create('Fòô Bàř', 'UTF-8');
//     $stringy = $stringy->swapCase();
//     // var_dump($stringy->test());
//     $valResult = [];
//     foreach ($stringy as $char) {
//         $valResult[] = $char;
//     }
//     // var_dump($valResult, function($a){});
//     // $keyValResult = [];
//     // foreach ($stringy as $pos => $char) {
//     //     $keyValResult[$pos] = $char;
//     // }
// }


// main();


_ns(NS_STRINGY);
// $x = _('Stringy', ['test test2 test3', 'UTF-8']);

$data = [
    ['', '', '', ''],
    ['bar', 'foo', 'f[o]+', 'bar'],
    ['o bar', 'foo bar', 'f(o)o', '\1'],
    ['bar', 'foo bar', 'f[O]+\s', '', 'i'],
    ['foo', 'bar', '[[:alpha:]]{3}', 'foo'],
    ['', '', '', '', 'msr', 'UTF-8'],
    ['bàř', 'fòô ', 'f[òô]+\s', 'bàř', 'msr', 'UTF-8'],
    ['fòô', 'fò', '(ò)', '\\1ô', 'msr', 'UTF-8'],
    ['fòô', 'bàř', '[[:alpha:]]{3}', 'fòô', 'msr', 'UTF-8']
];

foreach($data as $value) {
    @list($expected, $str, $pattern, $replacement, $options, $encoding) = $value;
    $x = __('Stringy')::create($str, 'UTF-8')->regexReplace($pattern, $replacement, $options);
    var_dump((string)$x == $expected);
}
