<?php

static $________ = [];

function _ns(string $ns = '\\')
{
    global $________;

    if (!empty($_SERVER['MINBABY_TEST_EXT'])) {
        $ns = str_replace(NS_DEFAULT, NS_EXT_DEFAULT, $ns);
    }

    $________['ns'] = $ns;
}

/**
 * @var string 类名
 *
 * @return object 类的实例化对象
 */
function _(string $str, array $args = [])
{
    $class = __($str);

    return new $class(...$args);
}

/**
 * @var string 类名
 *
 * @return string 带命名空间的完整类名
 */
function __(string $str): string
{
    global $________;
    if (empty($________['ns'])) {
        throw new Exception('ns can not empty');
    }

    return sprintf('%s\%s', rtrim($________['ns'], SEP_DIR), $str);
}

function __formatMessage(string $message, array $data): string
{
    return sprintf('%s: ==> %s <==', $message, json_encode($data, JSON_UNESCAPED_UNICODE|JSON_INVALID_UTF8_IGNORE));
}

function __debug_array(array $arr): string
{
    $i = $j = 0;

    $ret = '';
    for ($i = 0; $i < count($arr); $i++) {
        for ($j = 0; $j < count($arr[$i]); $j++) {
            $ret .= " ($i, $j) ";
        }
        $ret .= "\n";
    }
    $ret .= "\n";

    return $ret;
}
