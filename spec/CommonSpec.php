<?php

namespace Minbbaby\Ext\Spec;

describe('Check Extension', function () {
    it('Check Extison loaded', function() {
        expect(extension_loaded('startup'))->toBe(true);
    });

    it('Check Extension Test Class Exists', function() {
        expect(class_exists(\Minbaby\Ext\Test::class))->toBe(true);
    });
});
