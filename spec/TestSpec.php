<?php

namespace Minbbaby\Ext\Spec;

use Minbaby\Ext\Test;

describe('Test', function () {
    it('test hello world', function() {
        expect([Test::class, 'helloWorld'])->toEcho('hello world!');
    });
});


function xx() {
    it('test hello world', function() {
        expect([Test::class, 'helloWorld'])->toEcho('hello world!');
    });
}
