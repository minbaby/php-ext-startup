<?php

namespace Minbaby\Ext\Stringy;

\describe("Stringy Test", function () {
    it("test construct", function () {
        $stringy = new Stringy('test test2 test3', 'UTF-8');

        \expect($stringy instanceof Stringy)->toBe(true);
        \expect((string) $stringy)->toBe('test test2 test3');
        \expect($stringy->getEncoding())->toBe('UTF-8');
    });

    it("test empty construct", function () {
        $stringy = new Stringy();

        \expect($stringy instanceof Stringy)->toBe(true);
        \expect((string) $stringy)->toBe('');
    });

    it("test construct with array", function () {
        $closure = function () {
            (string) new Stringy([]);
        };

        \expect($closure)->toThrow(new \InvalidArgumentException('Passed value cannot be an array'));
    });

    it("test construct with object", function () {
        $closure = function () {
            (string) new Stringy(new \stdClass());
        };

        \expect($closure)->toThrow(new \InvalidArgumentException('Passed object must have a __toString method'));
    });
});
