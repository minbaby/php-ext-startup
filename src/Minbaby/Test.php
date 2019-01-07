<?php

namespace Minbaby\Ext;

class Test
{
    public $publicProperty = 'hello world +property';

    public static $publicPropertyStatic = 'hello world +property +static';

    public const PUBLIC_CONST = 'hello world +const';

    public static function helloWorld()
    {
        echo 'hello world!';
    }

    public function echoHelloWorld()
    {
        echo 'hello world!!';
    }

    public function __call($method, $args)
    {
        return sprintf('method:%s,count:%s,args:%s', $method, count($args), \implode('-', $args));
    }
}
