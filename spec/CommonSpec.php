<?php

namespace Minbbaby\Ext\Spec;

describe('Check Test Ojbect', function () {
    it('test hello world', function() {
        expect( method_exists(\Minbaby\Ext\Test::class, 'helloworld'))->toBe(true);
    });
});

var_dump(get_declared_classes());
