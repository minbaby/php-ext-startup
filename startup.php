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

include __DIR__ . "/spec/const.php";
include __DIR__ . "/spec/functions.php";

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
_('Stringy', ['Fòô Bàř', 'UTF-8'])->swapCase();
echo (string)__('Stringy')::create('Fòô Bàř', 'UTF-8');
