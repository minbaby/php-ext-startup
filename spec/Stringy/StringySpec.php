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
});
