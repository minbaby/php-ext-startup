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

    it("test construct with array", function () {
        $closure = function () {
            (string)_($this->class, [[]]);
        };
        \expect($closure)->toThrow(new \InvalidArgumentException('Passed value cannot be an array'));
    });

    it("test missing __toString", function () {
        $closure = function () {
            (string)_($this->class, [new \stdClass()]);
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
            list($key, $value) = $v;
            \expect($key)->toBe((string) _($this->class, [$value]));
        }
    });

    it("test create", function () {
        $stringy = __('Stringy')::create('foo bar', 'UTF-8');

        \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
        \expect('foo bar')->toBe((string) $stringy);
        \expect('UTF-8')->toBe($stringy->getEncoding());
    });

    it('test chaining', function () {
        $stringy = __('Stringy')::create('x    y', 'UTF-8');
        \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
        \expect("X    Y")->toBe((string) $stringy->swapCase());
    });

    it('test count', function () {
        $stringy = __('Stringy')::create('Fòô', 'UTF-8');
        \expect($stringy->count())->toBe(3);
        \expect(count($stringy))->toBe(3);
    });

    it('test GetIterator', function () {
        $stringy = __('Stringy')::create('Fòô Bàř', 'UTF-8');
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

        $stringy = __('Stringy')::create('fòô', 'UTF-8');

        foreach ($data as $value) {
            list($expected, $offset) = $value;
            \expect($stringy->offsetExists($offset))->toBe($expected);
            \expect(isset($stringy[$offset]))->toBe($expected);
        }
    });

    it('test OffsetGet', function () {
        $stringy = __('Stringy')::create('fòô', 'UTF-8');

        \expect($stringy->offsetGet(0))->toBe('f');
        \expect($stringy->offsetGet(1))->toBe('ò');

        \expect($stringy[2])->toBe('ô');
    });

    it('test OffsetGet out of bounds', function () {
        $stringy = __('Stringy')::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            $test = $stringy[3];
        };
        \expect($callable)->toThrow(new \OutOfBoundsException('No character exists at the index'));
    });

    it('test OffsetSet', function () {
        $stringy = __('Stringy')::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            $stringy[1] = 'invalid';
        };
        \expect($callable)->toThrow(new \Exception('Stringy object is immutable, cannot modify char'));
    });

    it('test OffsetUnset', function () {
        $stringy = __('Stringy')::create('fòô', 'UTF-8');
        $callable = function () use ($stringy) {
            unset($stringy[1]);
        };
        \expect($callable)->toThrow(new \Exception('Stringy object is immutable, cannot unset char'));
    });

    it('test IndexOf', function () {
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
            @list($expectedd, $str, $subStr, $offset, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->indexOf($subStr, $offset);
            \expect($result)->toBe($expectedd);
        }
    });

    it('test IndexOfLast', function () {
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
            @list($expectedd, $str, $subStr, $offset, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->indexOfLast($subStr, $offset);
            \expect($result)->toBe($expectedd);
        }
    });

    it('test append', function () {
        $data = [
            ['foobar', 'foo', 'bar'],
            ['fòôbàř', 'fòô', 'bàř', 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $string, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->append($string);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$result)->toBe($expected);
        }
    });

    it('test prepend', function () {
        $data = [
            ['foobar', 'bar', 'foo'],
            ['fòôbàř', 'bàř', 'fòô', 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $string, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->prepend($string);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$result)->toBe($expected);
        }
    });

    it('test chars', function () {
        $data = [
            [[], ''],
            [['T', 'e', 's', 't'], 'Test'],
            [['F', 'ò', 'ô', ' ', 'B', 'à', 'ř'], 'Fòô Bàř', 'UTF-8']
        ];

        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;

            $result = __('Stringy')::create($str, $encoding)->chars();
            \expect($result)->toBeA('array');
            
            foreach ($result as $char) {
                \expect($char)->toBeA('string');
            }
            
            \expect($result)->toBe($expected);
        }
    });

    it('test lines', function () {
        $data = [
            [[], ""],
            [[''], "\r\n"],
            [['foo', 'bar'], "foo\nbar"],
            [['foo', 'bar'], "foo\rbar"],
            [['foo', 'bar'], "foo\r\nbar"],
            [['foo', '', 'bar'], "foo\r\n\r\nbar"],
            [['foo', 'bar', ''], "foo\r\nbar\r\n"],
            [['', 'foo', 'bar'], "\r\nfoo\r\nbar"],
            [['fòô', 'bàř'], "fòô\nbàř", 'UTF-8'],
            [['fòô', 'bàř'], "fòô\rbàř", 'UTF-8'],
            [['fòô', 'bàř'], "fòô\n\rbàř", 'UTF-8'],
            [['fòô', 'bàř'], "fòô\r\nbàř", 'UTF-8'],
            [['fòô', '', 'bàř'], "fòô\r\n\r\nbàř", 'UTF-8'],
            [['fòô', 'bàř', ''], "fòô\r\nbàř\r\n", 'UTF-8'],
            [['', 'fòô', 'bàř'], "\r\nfòô\r\nbàř", 'UTF-8'],
        ];

        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;

            $result = __('Stringy')::create($str, $encoding)->lines();
            \expect($result)->toBeA('array');
            
            foreach ($result as $char) {
                \expect($char)->toBeAnInstanceOf(__('Stringy'));
            }
            
            for ($i = 0; $i < count($expected); $i++) {
                \expect((string)$result[$i])->toBe($expected[$i]);
            }
        }
    });

    it('test upperCaseFirst', function () {
        $data = [
            ['Test', 'Test'],
            ['Test', 'test'],
            ['1a', '1a'],
            ['Σ test', 'σ test', 'UTF-8'],
            [' σ test', ' σ test', 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->upperCaseFirst();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$result)->toBe($expected);
        }
    });

    it('test lowerCaseFirst', function () {
        $data = [
            ['test', 'Test'],
            ['test', 'test'],
            ['1a', '1a'],
            ['σ test', 'Σ test', 'UTF-8'],
            [' Σ test', ' Σ test', 'UTF-8']
        ];

        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            \expect((string)$stringy)->toBe($str);
            $result = $stringy->lowerCaseFirst();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
        }

    });

    it('test regexReplce', function () {

        $data = [
            ['', '', '', ''],
            ['bar', 'foo', 'f[o]+', 'bar'],
            ['o bar', 'foo bar', 'f(o)o', '\1'],
            ['bar', 'foo bar', 'f[O]+\s', '', 'i'],
            ['foo', 'bar', '[[:alpha:]]{3}', 'foo'],
            ['', '', '', '', 'msr', 'UTF-8'],
            ['bàř', 'fòô ', 'f[òô]+\s', 'bàř', 'msr', 'UTF-8'],
            ['fòô', 'fò', '(ò)', '\\1ô', 'msr', 'UTF-8'],
            ['fòô', 'bàř', '[[:alpha:]]{3}', 'fòô', 'msr', 'UTF-8']
        ];

        foreach($data as $value) {
            @list($expected, $str, $pattern, $replacement, $options, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->regexReplace($pattern, $replacement, $options);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
        }
    });
});
