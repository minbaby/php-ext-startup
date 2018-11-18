<?php

namespace Minbaby\Startup\Spec\Stringy;

\describe("Stringy Test", function () {
    \beforeAll(function () {
        _ns(NS_STRINGY);
        $this->class = 'Stringy';
        $this->className = __($this->class);
    });

    it("test construct", function () {
        $stringy = _('Stringy', ['test test2 test3', 'UTF-8']);

        \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
        \expect((string) $stringy)->toBe('test test2 test3');
        \expect($stringy->getEncoding())->toBe('UTF-8');
    });

    it("test empty construct", function () {
        $stringy = _('Stringy');

        \expect($stringy instanceof $this->className)->toBe(true);
        \expect((string) $stringy)->toBe('');
    });

    xit("test construct with array", function () {
        $closure = function () {
            (string)_($this->class, [[]]);
        };
        \expect($closure)->toThrow(new \InvalidArgumentException('Passed value cannot be an array'));
    });

    xit("test missing to string", function () {
        $closure = function () {
            (string)_($this->class, [new \stdClass()]);
        };

        \expect($closure)->toThrow(new \InvalidArgumentException('Passed object must have a __toString method'));
    });

    xit("test __toString", function () {
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
            \expect($key)->toBe((string) _($this->class, [$value]));
        }
    });

    xit("test create", function () {
        $stringy = Stringy::create('foo bar', 'UTF-8');

        \expect($stringy)->toBeAnInstanceOf(Stringy::class);
        \expect('foo bar')->toBe((string) $stringy);
        \expect('UTF-8')->toBe($stringy->getEncoding());
    });

    xit('test chaining', function () {
        $stringy = Stringy::create('Fòô     Bàř', 'UTF-8');
        \expect($stringy)->toBeAnInstanceOf(Stringy::class);
        \expect('FÒÔ bÀŘ')->toBe((string) $stringy->collapseWhiteSpace()->swapCase()->upperCaseFirst());
    });

    xit('test count', function () {
        $stringy = Stringy::create('Fòô', 'UTF-8');
        \expect($stringy->count())->toBe(3);
        \expect(count($stringy))->toBe(3);
    });

    xit('test GetIterator', function () {
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

    xit('test offsetExists', function () {
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

    xit('test OffsetGet', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');

        \expect($stringy->offsetGet(0))->toBe('f');
        \expect($stringy->offsetGet(1))->toBe('ò');

        \expect($stringy[2])->toBe('ô');
    });

    xit('test OffsetGet out of bounds', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            $test = $stringy[3];
        };
        \expect($callable)->toThrow(new \OutOfBoundsException('No character exists at the index'));
    });

    xit('test OffsetSet', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            $stringy[1] = 'invalid';
        };
        \expect($callable)->toThrow(new \Exception('Stringy object is immutable, cannot modify char'));
    });

    xit('test OffsetUnset', function () {
        $stringy = Stringy::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            unset($stringy[1]);
        };
        \expect($callable)->toThrow(new \Exception('Stringy object is immutable, cannot unset char'));
    });

    xit('test IndexOf', function () {
        $data = [
            [6, 'foo & bar', 'bar'],
            [6, 'foo & bar', 'bar', 0],
            [false, 'foo & bar', 'baz'],
            [false, 'foo & bar', 'baz', 0],
            [0, 'foo & bar & foo', 'foo', 0],
            [12, 'foo & bar & foo', 'foo', 5],
            [6, 'fòô & bàř', 'bàř', 0, 'UTF-8'],
            [false, 'fòô & bàř', 'baz', 0, 'UTF-8'],
            [0, 'fòô & bàř & fòô', 'fòô', 0, 'UTF-8'],
            [12, 'fòô & bàř & fòô', 'fòô', 5, 'UTF-8']
        ];

        foreach ($data as $value) {
            @list($expectd, $str, $subStr, $offset, $encoding) = $value;
            $result = Stringy::create($str, $encoding)->indexOf($subStr, $offset);
            \expect($result)->toBe($expectd);
        }
    });

    xit('test IndexOfLast', function () {
        $data = [
            [6, 'foo & bar', 'bar'],
            [6, 'foo & bar', 'bar', 0],
            [false, 'foo & bar', 'baz'],
            [false, 'foo & bar', 'baz', 0],
            [12, 'foo & bar & foo', 'foo', 0],
            [0, 'foo & bar & foo', 'foo', -5],
            [6, 'fòô & bàř', 'bàř', 0, 'UTF-8'],
            [false, 'fòô & bàř', 'baz', 0, 'UTF-8'],
            [12, 'fòô & bàř & fòô', 'fòô', 0, 'UTF-8'],
            [0, 'fòô & bàř & fòô', 'fòô', -5, 'UTF-8']
        ];

        foreach ($data as $value) {
            @list($expectd, $str, $subStr, $offset, $encoding) = $value;
            $result = Stringy::create($str, $encoding)->indexOfLast($subStr, $offset);
            \expect($result)->toBe($expectd);
        }
    });
});
