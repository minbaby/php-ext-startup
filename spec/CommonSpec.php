<?php

namespace Minbbaby\Ext\Spec;

use Minbaby\Ext\Test;

describe('Check Test Ojbect', function () {
    it('test hello world', function() {
        expect(function () {
            return method_exists(Test::class, '');
        })->toBe(true);
    });
});
