<?php

namespace Minbbaby\Ext\Spec;

use Minbaby\Ext\Test;

describe('Test', function () {
    it('test hello world', function() {
        expect([Test::class, 'helloWorld'])->toEcho('hello world!');
    });

    it('测试实例化 hello world', function() {
        $instance = new Test();
        expect([$instance, 'echoHelloWorld'])->toEcho('hello world!!');
    });
});
