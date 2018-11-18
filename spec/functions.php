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

function _(string $str, array $args = [])
{
    $class = __($str);
    return new $class(...$args);
}

function __(string $str): string
{
    global $________;
    if (empty($________['ns'])) {
        throw new Exception("ns can not empty");
    }

   return sprintf('%s\%s', rtrim($________['ns'], SEP_DIR), $str);
}
