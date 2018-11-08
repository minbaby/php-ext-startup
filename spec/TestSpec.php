<?php

namespace Minbbaby\Ext\Spec;

use Minbaby\Ext\Test;

describe('Test', function () {
    it('测试静态方法调用，且输出　hello world!', function() {
        expect([Test::class, 'helloWorld'])->toEcho('hello world!');
    });

    it('测试实例化 hello world!!', function() {
        $instance = new Test();
        expect([$instance, 'echoHelloWorld'])->toEcho('hello world!!');
    });

    it('测试 public 属性', function () {
        $instance = new Test();
        expect($instance->publicProperty)->toBe('hello world +property');
    });

    it('测试 public static 属性', function () {
        expect(Test::$publicPropertyStatic)->toBe('hello world +property +static');
    });

    it('测试 const 属性', function () {
        expect(Test::PUBLIC_CONST)->toBe('hello world +const');
    });
});