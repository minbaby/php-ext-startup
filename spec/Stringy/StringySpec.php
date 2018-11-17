<?php

namespace Minbaby\Ext\Stringy;

\describe("Stringy Test", function () {
    it("test construct", function () {
        $stringy = new Stringy('test test2 test3', 'UTF-8');

        \expect($stringy)->toBeAnInstanceOf(Stringy::class);
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

    it("test missing to string", function () {
        $closure = function () {
            (string) new Stringy(new \stdClass());
        };

        \expect($closure)->toThrow(new \InvalidArgumentException('Passed object must have a __toString method'));
    });

    it("test __toString", function () {
        $data = [
            ['', null],
            ['', false],
            ['1', true],
            ['-9', -9],
            ['1.18', 1.18],
            [' string  ', ' string  '],
            ['❤', '❤'],
        ];

        foreach($data as $v) {
            [$key, $value] = $v;
            \expect($key)->toBe((string) new Stringy($value));
        }
    });

    it("test create", function () {
        $stringy = Stringy::create('foo bar', 'UTF-8');

        \expect($stringy)->toBeAnInstanceOf(Stringy::class);
        \expect('foo bar')->toBe((string) $stringy);
        \expect('UTF-8')->toBe($stringy->getEncoding());
    });

    it('test chaining', function () {
        $stringy = Stringy::create('Fòô     Bàř', 'UTF-8');
        \expect($stringy)->toBeAnInstanceOf(Stringy::class);
        \expect('FÒÔ bÀŘ')->toBe((string) $stringy->collapseWhiteSpace()->swapCase()->upperCaseFirst());
    });

    it('test count', function () {
        $stringy = Stringy::create('Fòô', 'UTF-8');
        \expect($stringy->count())->toBe(3);
        \expect(count($stringy))->toBe(3);
    });

    it('test GetIterator', function () {
        $stringy = Stringy::create('Fòô Bàř', 'UTF-8');
        $valResult = [];
        foreach ($stringy as $char) {
            $valResult[] = $char;
        }
        $keyValResult = [];
        foreach ($stringy as $pos => $char) {
            $keyValResult[$pos] = $char;
        }

        \expect($valResult)->toBe(['F', 'ò', 'ô', ' ', 'B', 'à', 'ř']);
        \expect($keyValResult)->toBe(['F', 'ò', 'ô', ' ', 'B', 'à', 'ř']);
    });

    it('test offsetExists', function () {
        $data = [
            [true, 0],
            [true, 2],
            [false, 3],
            [true, -1],
            [true, -3],
            [false, -4]
        ];

        $stringy = Stringy::create('fòô', 'UTF-8');

        foreach ($data as $value) {
            [$expected, $offset] = $value;
            \expect($stringy->offsetExists($offset))->toBe($expected);
            \expect(isset($stringy[$offset]))->toBe($expected);
        }
    });

    it('test OffsetGet', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');

        \expect($stringy->offsetGet(0))->toBe('f');
        \expect($stringy->offsetGet(1))->toBe('ò');

        \expect($stringy[2])->toBe('ô');
    });

    it('test OffsetGet out of bounds', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            $test = $stringy[3];
        };
        \expect($callable)->toThrow(new \OutOfBoundsException('No character exists at the index'));
    });

    it('test OffsetSet', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            $stringy[1] = 'invalid';
        };
        \expect($callable)->toThrow(new \Exception('Stringy object is immutable, cannot modify char'));
    });

    it('test OffsetUnset', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            unset($stringy[1]);
        };
        \expect($callable)->toThrow(new \Exception('Stringy object is immutable, cannot modify char'));
    });
});
