<?php
namespace Minbaby\Startup\Spec\Stringy;

use function Kahlan\context;
use function Kahlan\it;
use function Kahlan\describe;

\describe('Stringy Test', function () {
    \beforeAll(function () {
        _ns(NS_STRINGY);
        $this->class = 'Stringy';
        $this->className = __($this->class);
    });

    it('test construct', function () {
        $stringy = _('Stringy', ['test test2 test3', 'UTF-8']);

        \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
        \expect((string) $stringy)->toBe('test test2 test3');
        \expect($stringy->getEncoding())->toBe('UTF-8');
    });

    it('test empty construct', function () {
        $stringy = _('Stringy');

        \expect($stringy instanceof $this->className)->toBe(true);
        \expect((string) $stringy)->toBe('');
    });

    it('test construct with array', function () {
        $closure = function () {
            (string) _($this->class, [[]]);
        };
        \expect($closure)->toThrow(new \InvalidArgumentException('Passed value cannot be an array'));
    });

    it('test missing __toString', function () {
        $closure = function () {
            (string) _($this->class, [new \stdClass()]);
        };

        \expect($closure)->toThrow(new \InvalidArgumentException('Passed object must have a __toString method'));
    });

    it('test __toString', function () {
        $data = [
            ['', null],
            ['', false],
            ['1', true],
            ['-9', -9],
            ['1.18', 1.18],
            [' string  ', ' string  '],
            ['❤', '❤'],
        ];

        foreach ($data as $v) {
            list($key, $value) = $v;
            \expect($key)->toBe((string) _($this->class, [$value]));
        }
    });

    it('test create', function () {
        $stringy = __('Stringy')::create('foo bar', 'UTF-8');

        \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
        \expect('foo bar')->toBe((string) $stringy);
        \expect('UTF-8')->toBe($stringy->getEncoding());
    });

    it('test chaining', function () {
        $stringy = __('Stringy')::create('x    y', 'UTF-8');
        \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
        \expect((string) $stringy->swapCase())->toBe('X    Y');
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
            [false, -4],
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
            [12, 'fòô & bàř & fòô', 'fòô', 5, 'UTF-8'],
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
            [0, 'fòô & bàř & fòô', 'fòô', -5, 'UTF-8'],
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
            ['fòôbàř', 'fòô', 'bàř', 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $string, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->append($string);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test prepend', function () {
        $data = [
            ['foobar', 'bar', 'foo'],
            ['fòôbàř', 'bàř', 'fòô', 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $string, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->prepend($string);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test chars', function () {
        $data = [
            [[], ''],
            [['T', 'e', 's', 't'], 'Test'],
            [['F', 'ò', 'ô', ' ', 'B', 'à', 'ř'], 'Fòô Bàř', 'UTF-8'],
        ];

        foreach ($data as $value) {
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
            [[], ''],
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

        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;

            $result = __('Stringy')::create($str, $encoding)->lines();
            \expect($result)->toBeA('array');

            foreach ($result as $char) {
                \expect($char)->toBeAnInstanceOf(__('Stringy'));
            }

            for ($i = 0; $i < count($expected); $i++) {
                \expect((string) $result[$i])->toBe($expected[$i]);
            }
        }
    });

    it('test upperCaseFirst', function () {
        $data = [
            ['Test', 'Test'],
            ['Test', 'test'],
            ['1a', '1a'],
            ['Σ test', 'σ test', 'UTF-8'],
            [' σ test', ' σ test', 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->upperCaseFirst();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test lowerCaseFirst', function () {
        $data = [
            ['test', 'Test'],
            ['test', 'test'],
            ['1a', '1a'],
            ['σ test', 'Σ test', 'UTF-8'],
            [' Σ test', ' Σ test', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            \expect((string) $stringy)->toBe($str);
            $result = $stringy->lowerCaseFirst();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test testCamelize', function () {
        $data = [
            ['camelCase', 'CamelCase'],
            ['camelCase', 'Camel-Case'],
            ['camelCase', 'camel case'],
            ['camelCase', 'camel -case'],
            ['camelCase', 'camel - case'],
            ['camelCase', 'camel_case'],
            ['camelCTest', 'camel c test'],
            ['stringWith1Number', 'string_with1number'],
            ['stringWith22Numbers', 'string-with-2-2 numbers'],
            ['dataRate', 'data_rate'],
            ['backgroundColor', 'background-color'],
            ['yesWeCan', 'yes_we_can'],
            ['mozSomething', '-moz-something'],
            ['carSpeed', '_car_speed_'],
            ['serveHTTP', 'ServeHTTP'],
            ['1Camel2Case', '1camel2case'],
            ['camelΣase', 'camel σase', 'UTF-8'],
            ['στανιλCase', 'Στανιλ case', 'UTF-8'],
            ['σamelCase', 'σamel  Case', 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->camelize();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
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
            ['fòô', 'bàř', '[[:alpha:]]{3}', 'fòô', 'msr', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $pattern, $replacement, $options, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->regexReplace($pattern, $replacement, $options);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test trim', function () {
        $data = [
            ['foo   bar', '  foo   bar  '],
            ['foo bar', ' foo bar'],
            ['foo bar', 'foo bar '],
            ['foo bar', "\n\t foo bar \n\t"],
            ['fòô   bàř', '  fòô   bàř  '],
            ['fòô bàř', ' fòô bàř'],
            ['fòô bàř', 'fòô bàř '],
            [' foo bar ', "\n\t foo bar \n\t", "\n\t"],
            ['fòô bàř', "\n\t fòô bàř \n\t", null, 'UTF-8'],
            ['fòô', ' fòô ', null, 'UTF-8'], // narrow no-break space (U+202F)
            ['fòô', '  fòô  ', null, 'UTF-8'], // medium mathematical space (U+205F)
            ['fòô', '           fòô', null, 'UTF-8'], // spaces U+2000 to U+200A
        ];

        foreach ($data as $value) {
            @list($expected, $str, $chars, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->trim($chars);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test trimLeft', function () {
        $data = [
            ['foo   bar  ', '  foo   bar  '],
            ['foo bar', ' foo bar'],
            ['foo bar ', 'foo bar '],
            ["foo bar \n\t", "\n\t foo bar \n\t"],
            ['fòô   bàř  ', '  fòô   bàř  '],
            ['fòô bàř', ' fòô bàř'],
            ['fòô bàř ', 'fòô bàř '],
            ['foo bar', '--foo bar', '-'],
            ['fòô bàř', 'òòfòô bàř', 'ò', 'UTF-8'],
            ["fòô bàř \n\t", "\n\t fòô bàř \n\t", null, 'UTF-8'],
            ['fòô ', ' fòô ', null, 'UTF-8'], // narrow no-break space (U+202F)
            ['fòô  ', '  fòô  ', null, 'UTF-8'], // medium mathematical space (U+205F)
            ['fòô', '           fòô', null, 'UTF-8'], // spaces U+2000 to U+200A
        ];

        foreach ($data as $value) {
            @list($expected, $str, $chars, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->trimLeft($chars);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test trimRight', function () {
        $data = [
            ['  foo   bar', '  foo   bar  '],
            ['foo bar', 'foo bar '],
            [' foo bar', ' foo bar'],
            ["\n\t foo bar", "\n\t foo bar \n\t"],
            ['  fòô   bàř', '  fòô   bàř  '],
            ['fòô bàř', 'fòô bàř '],
            [' fòô bàř', ' fòô bàř'],
            ['foo bar', 'foo bar--', '-'],
            ['fòô bàř', 'fòô bàřòò', 'ò', 'UTF-8'],
            ["\n\t fòô bàř", "\n\t fòô bàř \n\t", null, 'UTF-8'],
            [' fòô', ' fòô ', null, 'UTF-8'], // narrow no-break space (U+202F)
            ['  fòô', '  fòô  ', null, 'UTF-8'], // medium mathematical space (U+205F)
            ['fòô', 'fòô           ', null, 'UTF-8'], // spaces U+2000 to U+200A
        ];

        foreach ($data as $value) {
            @list($expected, $str, $chars, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->trimRight($chars);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test at', function () {
        $data = [
            ['f', 'foo bar', 0],
            ['o', 'foo bar', 1],
            ['r', 'foo bar', 6],
            ['', 'foo bar', 7],
            ['f', 'fòô bàř', 0, 'UTF-8'],
            ['ò', 'fòô bàř', 1, 'UTF-8'],
            ['ř', 'fòô bàř', 6, 'UTF-8'],
            ['', 'fòô bàř', 7, 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $index, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->at($index);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test between', function () {
        $data = [
            ['', 'foo', '{', '}'],
            ['', '{foo', '{', '}'],
            ['foo', '{foo}', '{', '}'],
            ['{foo', '{{foo}', '{', '}'],
            ['', '{}foo}', '{', '}'],
            ['foo', '}{foo}', '{', '}'],
            ['foo', 'A description of {foo} goes here', '{', '}'],
            ['bar', '{foo} and {bar}', '{', '}', 1],
            ['', 'fòô', '{', '}', 0, 'UTF-8'],
            ['', '{fòô', '{', '}', 0, 'UTF-8'],
            ['fòô', '{fòô}', '{', '}', 0, 'UTF-8'],
            ['{fòô', '{{fòô}', '{', '}', 0, 'UTF-8'],
            ['', '{}fòô}', '{', '}', 0, 'UTF-8'],
            ['fòô', '}{fòô}', '{', '}', 0, 'UTF-8'],
            ['fòô', 'A description of {fòô} goes here', '{', '}', 0, 'UTF-8'],
            ['bàř', '{fòô} and {bàř}', '{', '}', 1, 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $start, $end, $offset, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->between($start, $end, $offset);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    \context('contains', function () {
        $data = [
            [true, 'Str contains foo bar', 'foo bar'],
            [true, '12398!@(*%!@# @!%#*&^%',  ' @!%#*&^%'],
            [true, 'Ο συγγραφέας είπε', 'συγγραφέας', 'UTF-8'],
            [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'å´¥©', true, 'UTF-8'],
            [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'å˚ ∆', true, 'UTF-8'],
            [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'øœ¬', true, 'UTF-8'],
            [false, 'Str contains foo bar', 'Foo bar'],
            [false, 'Str contains foo bar', 'foobar'],
            [false, 'Str contains foo bar', 'foo bar '],
            [false, 'Ο συγγραφέας είπε', '  συγγραφέας ', true, 'UTF-8'],
            [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ' ßå˚', true, 'UTF-8'],
            [true, 'Str contains foo bar', 'Foo bar', false],
            [true, '12398!@(*%!@# @!%#*&^%',  ' @!%#*&^%', false],
            [true, 'Ο συγγραφέας είπε', 'ΣΥΓΓΡΑΦΈΑΣ', false, 'UTF-8'],
            [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'Å´¥©', false, 'UTF-8'],
            [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'Å˚ ∆', false, 'UTF-8'],
            [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'ØŒ¬', false, 'UTF-8'],
            [false, 'Str contains foo bar', 'foobar', false],
            [false, 'Str contains foo bar', 'foo bar ', false],
            [false, 'Ο συγγραφέας είπε', '  συγγραφέας ', false, 'UTF-8'],
            [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ' ßÅ˚', false, 'UTF-8'],
        ];

        $dataContainsAll = array_merge(
            array_map(function ($array) {
                $array[2] = [$array[2]];

                return $array;
            }, $data),
            [
                // One needle
                [false, 'Str contains foo bar', []],
                // Multiple needles
                [true, 'Str contains foo bar', ['foo', 'bar']],
                [true, '12398!@(*%!@# @!%#*&^%', [' @!%#*', '&^%']],
                [true, 'Ο συγγραφέας είπε', ['συγγρ', 'αφέας'], 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['å´¥', '©'], true, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['å˚ ', '∆'], true, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['øœ', '¬'], true, 'UTF-8'],
                [false, 'Str contains foo bar', ['Foo', 'bar']],
                [false, 'Str contains foo bar', ['foobar', 'bar']],
                [false, 'Str contains foo bar', ['foo bar ', 'bar']],
                [false, 'Ο συγγραφέας είπε', ['  συγγραφέας ', '  συγγραφ '], true, 'UTF-8'],
                [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', [' ßå˚', ' ß '], true, 'UTF-8'],
                [true, 'Str contains foo bar', ['Foo bar', 'bar'], false],
                [true, '12398!@(*%!@# @!%#*&^%', [' @!%#*&^%', '*&^%'], false],
                [true, 'Ο συγγραφέας είπε', ['ΣΥΓΓΡΑΦΈΑΣ', 'ΑΦΈΑ'], false, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['Å´¥©', '¥©'], false, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['Å˚ ∆', ' ∆'], false, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['ØŒ¬', 'Œ'], false, 'UTF-8'],
                [false, 'Str contains foo bar', ['foobar', 'none'], false],
                [false, 'Str contains foo bar', ['foo bar ', ' ba'], false],
                [false, 'Ο συγγραφέας είπε', ['  συγγραφέας ', ' ραφέ '], false, 'UTF-8'],
                [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', [' ßÅ˚', ' Å˚ '], false, 'UTF-8'],
        ]);

        $dataContainsAny = array_merge(
            array_map(function ($array) {
                $array[2] = [$array[2]];

                return $array;
            }, $data),
            [
                // No needles
                [false, 'Str contains foo bar', []],
                // Multiple needles
                [true, 'Str contains foo bar', ['foo', 'bar']],
                [true, '12398!@(*%!@# @!%#*&^%', [' @!%#*', '&^%']],
                [true, 'Ο συγγραφέας είπε', ['συγγρ', 'αφέας'], 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['å´¥', '©'], true, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['å˚ ', '∆'], true, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['øœ', '¬'], true, 'UTF-8'],
                [false, 'Str contains foo bar', ['Foo', 'Bar']],
                [false, 'Str contains foo bar', ['foobar', 'bar ']],
                [false, 'Str contains foo bar', ['foo bar ', '  foo']],
                [false, 'Ο συγγραφέας είπε', ['  συγγραφέας ', '  συγγραφ '], true, 'UTF-8'],
                [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', [' ßå˚', ' ß '], true, 'UTF-8'],
                [true, 'Str contains foo bar', ['Foo bar', 'bar'], false],
                [true, '12398!@(*%!@# @!%#*&^%', [' @!%#*&^%', '*&^%'], false],
                [true, 'Ο συγγραφέας είπε', ['ΣΥΓΓΡΑΦΈΑΣ', 'ΑΦΈΑ'], false, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['Å´¥©', '¥©'], false, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['Å˚ ∆', ' ∆'], false, 'UTF-8'],
                [true, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ['ØŒ¬', 'Œ'], false, 'UTF-8'],
                [false, 'Str contains foo bar', ['foobar', 'none'], false],
                [false, 'Str contains foo bar', ['foo bar ', ' ba '], false],
                [false, 'Ο συγγραφέας είπε', ['  συγγραφέας ', ' ραφέ '], false, 'UTF-8'],
                [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', [' ßÅ˚', ' Å˚ '], false, 'UTF-8'],
            ]);

        it('test contains', function () use ($data) {
            foreach ($data as $value) {
                @list($expected, $haystack, $needle, $caseSensitive, $encoding) = $value;
                $stringy = __('Stringy')::create($haystack, $encoding);
                $result = $stringy->contains($needle, $caseSensitive === true || $caseSensitive === null); // 默认值是　ＮＵＬｌ，　这里需要特殊处理
                \expect($result)->toBeA('bool');
                \expect((string) $stringy)->toBe($haystack);
                \expect((bool) $result)->toBe($expected);
            }
        });

        it('test containsAll', function () use ($dataContainsAll) {
            foreach ($dataContainsAll as $value) {
                @list($expected, $haystack, $needle, $caseSensitive, $encoding) = $value;

                $stringy = __('Stringy')::create($haystack, $encoding);
                $result = $stringy->containsAll($needle, $caseSensitive === true || $caseSensitive === null); // 默认值是　ＮＵＬｌ，　这里需要特殊处理
                \expect($result)->toBeA('bool');
                \expect((string) $stringy)->toBe($haystack);
                \expect((bool) $result)->toBe($expected);
            }
        });

        it('test containsAny', function () use ($dataContainsAny) {
            foreach ($dataContainsAny as $value) {
                @list($expected, $haystack, $needles, $caseSensitive, $encoding) = $value;
                $stringy = __('Stringy')::create($haystack, $encoding);

                $result = $stringy->containsAny($needles, $caseSensitive === true || $caseSensitive === null);
                \expect($result)->toBeA('bool');
                \expect((string) $stringy)->toBe($haystack);
                \expect((bool) $result)->toBe($expected);
            }
        });
    });

    it('test split', function () {
        $data = [
            [['foo,bar,baz'], 'foo,bar,baz', ''],
            [['foo,bar,baz'], 'foo,bar,baz', '-'],
            [['foo', 'bar', 'baz'], 'foo,bar,baz', ','],
            [['foo', 'bar', 'baz'], 'foo,bar,baz', ',', null],
            [['foo', 'bar', 'baz'], 'foo,bar,baz', ',', -1],
            [[], 'foo,bar,baz', ',', 0],
            [['foo'], 'foo,bar,baz', ',', 1],
            [['foo', 'bar'], 'foo,bar,baz', ',', 2],
            [['foo', 'bar', 'baz'], 'foo,bar,baz', ',', 3],
            [['foo', 'bar', 'baz'], 'foo,bar,baz', ',', 10],
            [['fòô,bàř,baz'], 'fòô,bàř,baz', '-', null, 'UTF-8'],
            [['fòô', 'bàř', 'baz'], 'fòô,bàř,baz', ',', null, 'UTF-8'],
            [['fòô', 'bàř', 'baz'], 'fòô,bàř,baz', ',', null, 'UTF-8'],
            [['fòô', 'bàř', 'baz'], 'fòô,bàř,baz', ',', -1, 'UTF-8'],
            [[], 'fòô,bàř,baz', ',', 0, 'UTF-8'],
            [['fòô'], 'fòô,bàř,baz', ',', 1, 'UTF-8'],
            [['fòô', 'bàř'], 'fòô,bàř,baz', ',', 2, 'UTF-8'],
            [['fòô', 'bàř', 'baz'], 'fòô,bàř,baz', ',', 3, 'UTF-8'],
            [['fòô', 'bàř', 'baz'], 'fòô,bàř,baz', ',', 10, 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $pattern, $limit, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->split($pattern, $limit);
            \expect($result)->toBeA('array');
            foreach ($result as $string) {
                \expect($string)->toBeAnInstanceOf(__('Stringy'));
            }
            for ($i = 0; $i < count($expected); $i++) {
                \expect((string) $result[$i])->toBe($expected[$i]);
            }
        }
    });

    it('test CollapseWhitespace', function () {
        $data = [
            ['foo bar', '  foo   bar  '],
            ['test string', 'test string'],
            ['Ο συγγραφέας', '   Ο     συγγραφέας  '],
            ['123', ' 123 '],
            ['', ' ', 'UTF-8'], // no-break space (U+00A0)
            ['', '           ', 'UTF-8'], // spaces U+2000 to U+200A
            ['', ' ', 'UTF-8'], // narrow no-break space (U+202F)
            ['', ' ', 'UTF-8'], // medium mathematical space (U+205F)
            ['', '　', 'UTF-8'], // ideographic space (U+3000)
            ['1 2 3', '  1  2  3　　', 'UTF-8'],
            ['', ' '],
            ['', ''],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            {
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->collapseWhitespace();
                \expect($result)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            }
        }
    });

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
        [12, 'fòô & bàř & fòô', 'fòô', 5, 'UTF-8'],
    ];

    it('test indexOf', function () use ($data) {
        foreach ($data as $value) {
            @list($expected, $str, $subStr, $offset, $encoding) = $value;
            $result = __('Stringy')::create($str, $encoding)->indexOf($subStr, $offset);
            \expect($result)->toBe($expected);
        }
    });

    it('test countSubStr', function () {
        $data = [
            [0, '', 'foo'],
            [0, 'foo', 'bar'],
            [1, 'foo bar', 'foo'],
            [2, 'foo bar', 'o'],
            [0, '', 'fòô', true, 'UTF-8'],
            [0, 'fòô', 'bàř', true, 'UTF-8'],
            [1, 'fòô bàř', 'fòô', true, 'UTF-8'],
            [2, 'fôòô bàř', 'ô', true, 'UTF-8'],
            [0, 'fÔÒÔ bàř', 'ô', true, 'UTF-8'],
            [0, 'foo', 'BAR', false],
            [1, 'foo bar', 'FOo', false],
            [2, 'foo bar', 'O', false],
            [1, 'fòô bàř', 'fÒÔ', false, 'UTF-8'],
            [2, 'fôòô bàř', 'Ô', false, 'UTF-8'],
            [2, 'συγγραφέας', 'Σ', false, 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $substring, $caseSensitive, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->countSubstr($substring, $caseSensitive === true || $caseSensitive === null);
            \expect($result)->toBeA('int');
            \expect($result)->toBe($expected);
            \expect((string) $stringy)->toBe($str);
        }
    });

    it('test delimit', function () {
        $data = [
            ['test*case', 'testCase', '*'],
            ['test&case', 'Test-Case', '&'],
            ['test#case', 'test case', '#'],
            ['test**case', 'test -case', '**'],
            ['~!~test~!~case', '-test - case', '~!~'],
            ['test*case', 'test_case', '*'],
            ['test%c%test', '  test c test', '%'],
            ['test+u+case', 'TestUCase', '+'],
            ['test=c=c=test', 'TestCCTest', '='],
            ['string#>with1number', 'string_with1number', '#>'],
            ['1test2case', '1test2case', '*'],
            ['test ύα σase', 'test Σase', ' ύα ', 'UTF-8'],
            ['στανιλαcase', 'Στανιλ case', 'α', 'UTF-8'],
            ['σashΘcase', 'Σash  Case', 'Θ', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $delimiter, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->delimit($delimiter);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test dasherize', function () {
        $data = [
            ['test-case', 'testCase'],
            ['test-case', 'Test-Case'],
            ['test-case', 'test case'],
            ['-test-case', '-test -case'],
            ['test-case', 'test - case'],
            ['test-case', 'test_case'],
            ['test-c-test', 'test c test'],
            ['test-d-case', 'TestDCase'],
            ['test-c-c-test', 'TestCCTest'],
            ['string-with1number', 'string_with1number'],
            ['string-with-2-2-numbers', 'String-with_2_2 numbers'],
            ['1test2case', '1test2case'],
            ['data-rate', 'dataRate'],
            ['car-speed', 'CarSpeed'],
            ['yes-we-can', 'yesWeCan'],
            ['background-color', 'backgroundColor'],
            ['dash-σase', 'dash Σase', 'UTF-8'],
            ['στανιλ-case', 'Στανιλ case', 'UTF-8'],
            ['σash-case', 'Σash  Case', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->dasherize();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test endsWith', function () {
        $data = [
            [true, 'foo bars', 'o bars'],
            [true, 'FOO bars', 'o bars', false],
            [true, 'FOO bars', 'o BARs', false],
            [true, 'FÒÔ bàřs', 'ô bàřs', false, 'UTF-8'],
            [true, 'fòô bàřs', 'ô BÀŘs', false, 'UTF-8'],
            [false, 'foo bar', 'foo'],
            [false, 'foo bar', 'foo bars'],
            [false, 'FOO bar', 'foo bars'],
            [false, 'FOO bars', 'foo BARS'],
            [false, 'FÒÔ bàřs', 'fòô bàřs', true, 'UTF-8'],
            [false, 'fòô bàřs', 'fòô BÀŘS', true, 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $substring, $caseSensitive, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->endsWith($substring, $caseSensitive === true || $caseSensitive === null);

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });

    it('test endWithAny', function () {
        $data = [
            [true, 'foo bars', ['foo', 'o bars']],
            [true, 'FOO bars', ['foo', 'o bars'], false],
            [true, 'FOO bars', ['foo', 'o BARs'], false],
            [true, 'FÒÔ bàřs', ['foo', 'ô bàřs'], false, 'UTF-8'],
            [true, 'fòô bàřs', ['foo', 'ô BÀŘs'], false, 'UTF-8'],
            [false, 'foo bar', ['foo']],
            [false, 'foo bar', ['foo', 'foo bars']],
            [false, 'FOO bar', ['foo', 'foo bars']],
            [false, 'FOO bars', ['foo', 'foo BARS']],
            [false, 'FÒÔ bàřs', ['fòô', 'fòô bàřs'], true, 'UTF-8'],
            [false, 'fòô bàřs', ['fòô', 'fòô BÀŘS'], true, 'UTF-8'],
            [false, 'fòô bàřs', [], true, 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $substrings, $caseSensitive, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->endsWithAny($substrings, $caseSensitive === true || $caseSensitive === null);

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });

    it('test ensureLeft', function () {
        $data = [
            ['foobar', 'foobar', 'f'],
            ['foobar', 'foobar', 'foo'],
            ['foo/foobar', 'foobar', 'foo/'],
            ['http://foobar', 'foobar', 'http://'],
            ['http://foobar', 'http://foobar', 'http://'],
            ['fòôbàř', 'fòôbàř', 'f', 'UTF-8'],
            ['fòôbàř', 'fòôbàř', 'fòô', 'UTF-8'],
            ['fòô/fòôbàř', 'fòôbàř', 'fòô/', 'UTF-8'],
            ['http://fòôbàř', 'fòôbàř', 'http://', 'UTF-8'],
            ['http://fòôbàř', 'http://fòôbàř', 'http://', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $substring, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->ensureLeft($substring);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test startWith', function () {
        $data = [
            [true, 'foo bars', 'foo bar'],
            [true, 'FOO bars', 'foo bar', false],
            [true, 'FOO bars', 'foo BAR', false],
            [true, 'FÒÔ bàřs', 'fòô bàř', false, 'UTF-8'],
            [true, 'fòô bàřs', 'fòô BÀŘ', false, 'UTF-8'],
            [false, 'foo bar', 'bar'],
            [false, 'foo bar', 'foo bars'],
            [false, 'FOO bar', 'foo bars'],
            [false, 'FOO bars', 'foo BAR'],
            [false, 'FÒÔ bàřs', 'fòô bàř', true, 'UTF-8'],
            [false, 'fòô bàřs', 'fòô BÀŘ', true, 'UTF-8'],
            ];
        foreach ($data as $value) {
            @list($expected, $str, $substring, $caseSensitive, $encoding) = $value;
            $stringy = _('Stringy')::create($str, $encoding);
            $result = $stringy->startsWith($substring, $caseSensitive === true || $caseSensitive === null);

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });

    it('test ensureRight', function () {
        $data = [
            ['foobar', 'foobar', 'r'],
            ['foobar', 'foobar', 'bar'],
            ['foobar/bar', 'foobar', '/bar'],
            ['foobar.com/', 'foobar', '.com/'],
            ['foobar.com/', 'foobar.com/', '.com/'],
            ['fòôbàř', 'fòôbàř', 'ř', 'UTF-8'],
            ['fòôbàř', 'fòôbàř', 'bàř', 'UTF-8'],
            ['fòôbàř/bàř', 'fòôbàř', '/bàř', 'UTF-8'],
            ['fòôbàř.com/', 'fòôbàř', '.com/', 'UTF-8'],
            ['fòôbàř.com/', 'fòôbàř.com/', '.com/', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $substring, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->ensureRight($substring);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test first', function () {
        $data = [
            ['', 'foo bar', -5],
            ['', 'foo bar', 0],
            ['f', 'foo bar', 1],
            ['foo', 'foo bar', 3],
            ['foo bar', 'foo bar', 7],
            ['foo bar', 'foo bar', 8],
            ['', 'fòô bàř', -5, 'UTF-8'],
            ['', 'fòô bàř', 0, 'UTF-8'],
            ['f', 'fòô bàř', 1, 'UTF-8'],
            ['fòô', 'fòô bàř', 3, 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 7, 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 8, 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $n, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->first($n);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test isAlpha', function () {
        $data = [
            [true, ''],
            [true, 'foobar'],
            [false, 'foo bar'],
            [false, 'foobar2'],
            [true, 'fòôbàř', 'UTF-8'],
            [false, 'fòô bàř', 'UTF-8'],
            [false, 'fòôbàř2', 'UTF-8'],
            [true, 'ҠѨњфгШ', 'UTF-8'],
            [false, 'ҠѨњ¨ˆфгШ', 'UTF-8'],
            [true, '丹尼爾', 'UTF-8'],
        ];
        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isAlpha();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });
    
    it('test isBlank', function () {
        $data = [
            [true, ''],
            [true, ' '],
            [true, "\n\t "],
            [true, "\n\t  \v\f"],
            [false, "\n\t a \v\f"],
            [false, "\n\t ' \v\f"],
            [false, "\n\t 2 \v\f"],
            [true, '', 'UTF-8'],
            [true, ' ', 'UTF-8'], // no-break space (U+00A0)
            [true, '           ', 'UTF-8'], // spaces U+2000 to U+200A
            [true, ' ', 'UTF-8'], // narrow no-break space (U+202F)
            [true, ' ', 'UTF-8'], // medium mathematical space (U+205F)
            [true, '　', 'UTF-8'], // ideographic space (U+3000)
            [false, '　z', 'UTF-8'],
            [false, '　1', 'UTF-8'],
        ];
        foreach ($data as $value)
        {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isBlank();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });

    it('test hasLowerCase', function () {
        $data = [
            [false, ''],
            [true, 'foobar'],
            [false, 'FOO BAR'],
            [true, 'fOO BAR'],
            [true, 'foO BAR'],
            [true, 'FOO BAr'],
            [true, 'Foobar'],
            [false, 'FÒÔBÀŘ', 'UTF-8'],
            [true, 'fòôbàř', 'UTF-8'],
            [true, 'fòôbàř2', 'UTF-8'],
            [true, 'Fòô bàř', 'UTF-8'],
            [true, 'fòôbÀŘ', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->hasLowerCase();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });


    it('test hasUpperCase', function () {
        $data = [
            [false, ''],
            [true, 'FOOBAR'],
            [false, 'foo bar'],
            [true, 'Foo bar'],
            [true, 'FOo bar'],
            [true, 'foo baR'],
            [true, 'fOOBAR'],
            [false, 'fòôbàř', 'UTF-8'],
            [true, 'FÒÔBÀŘ', 'UTF-8'],
            [true, 'FÒÔBÀŘ2', 'UTF-8'],
            [true, 'fÒÔ BÀŘ', 'UTF-8'],
            [true, 'FÒÔBàř', 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->hasUpperCase();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }
    });

    it('test htmlDecode', function () {
        $data = [
            ['&', '&amp;'],
            ['"', '&quot;'],
            ["'", '&#039;', ENT_QUOTES],
            ['<', '&lt;'],
            ['>', '&gt;'],
        ];
        foreach($data as $value){
            @list($expected, $str, $flags, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->htmlDecode($flags ?? ENT_COMPAT);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test htmlEncode', function () {
        $data = [
            ['&amp;', '&'],
            ['&quot;', '"'],
            ['&#039;', "'", ENT_QUOTES],
            ['&lt;', '<'],
            ['&gt;', '>'],
        ];
        foreach($data as $value){
            @list($expected, $str, $flags, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->htmlEncode($flags ?? ENT_COMPAT);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test substr', function () {
        $data = [
            ['foo bar', 'foo bar', 0],
            ['bar', 'foo bar', 4],
            ['bar', 'foo bar', 4, null],
            ['o b', 'foo bar', 2, 3],
            ['', 'foo bar', 4, 0],
            ['fòô bàř', 'fòô bàř', 0, null, 'UTF-8'],
            ['bàř', 'fòô bàř', 4, null, 'UTF-8'],
            ['ô b', 'fòô bàř', 2, 3, 'UTF-8'],
            ['', 'fòô bàř', 4, 0, 'UTF-8'],
            ['r', 'foo bar', -1],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $start, $length, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->substr($start, $length);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }

    });

    it('test last', function () {
        $data = [
            ['', 'foo bar', -5],
            ['', 'foo bar', 0],
            ['r', 'foo bar', 1],
            ['bar', 'foo bar', 3],
            ['foo bar', 'foo bar', 7],
            ['foo bar', 'foo bar', 8],
            ['', 'fòô bàř', -5, 'UTF-8'],
            ['', 'fòô bàř', 0, 'UTF-8'],
            ['ř', 'fòô bàř', 1, 'UTF-8'],
            ['bàř', 'fòô bàř', 3, 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 7, 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 8, 'UTF-8'],
        ];

        foreach ($data as $value) {
            @list($expected, $str, $n, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->last($n);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test removeLeft', function () {
        $data = [
            ['foo bar', 'foo bar', ''],
            ['oo bar', 'foo bar', 'f'],
            ['bar', 'foo bar', 'foo '],
            ['foo bar', 'foo bar', 'oo'],
            ['foo bar', 'foo bar', 'oo bar'],
            ['oo bar', 'foo bar', __('Stringy')::create('foo bar')->first(1), 'UTF-8'],
            ['oo bar', 'foo bar', __('Stringy')::create('foo bar')->at(0), 'UTF-8'],
            ['fòô bàř', 'fòô bàř', '', 'UTF-8'],
            ['òô bàř', 'fòô bàř', 'f', 'UTF-8'],
            ['bàř', 'fòô bàř', 'fòô ', 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 'òô', 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 'òô bàř', 'UTF-8']
        ];
        foreach($data as $value)
        {
            @list($expected, $str, $substring, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->removeLeft((string) $substring);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test removeRight', function () {
        $data = [
            ['foo bar', 'foo bar', ''],
            ['foo ba', 'foo bar', 'r'],
            ['foo', 'foo bar', ' bar'],
            ['foo bar', 'foo bar', 'ba'],
            ['foo bar', 'foo bar', 'foo ba'],
            ['foo ba', 'foo bar', __('Stringy')::create('foo bar')->last(1), 'UTF-8'],
            ['foo ba', 'foo bar', __('Stringy')::create('foo bar')->at(6), 'UTF-8'],
            ['fòô bàř', 'fòô bàř', '', 'UTF-8'],
            ['fòô bà', 'fòô bàř', 'ř', 'UTF-8'],
            ['fòô', 'fòô bàř', ' bàř', 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 'bà', 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 'fòô bà', 'UTF-8']
        ];
        foreach($data as $value)
        {
            @list($expected, $str, $substring, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->removeRight((string) $substring);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test humanize', function () {
        $data = [
            ['Author', 'author_id'],
            ['Test user', ' _test_user_'],
            ['Συγγραφέας', ' συγγραφέας_id ', 'UTF-8']
        ];
        
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->humanize();

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
        }
    });

    it('test insert', function () {
        $data = [
            ['foo bar', 'oo bar', 'f', 0],
            ['foo bar', 'f bar', 'oo', 1],
            ['f bar', 'f bar', 'oo', 20],
            ['foo bar', 'foo ba', 'r', 6],
            ['fòôbàř', 'fòôbř', 'à', 4, 'UTF-8'],
            ['fòô bàř', 'òô bàř', 'f', 0, 'UTF-8'],
            ['fòô bàř', 'f bàř', 'òô', 1, 'UTF-8'],
            ['fòô bàř', 'fòô bà', 'ř', 6, 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $substring, $index, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->insert($substring, $index);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);

        }
    });

    it('test isAlphanumeric', function () {
        $data = [
            [true, ''],
            [true, 'foobar1'],
            [false, 'foo bar'],
            [false, 'foobar2"'],
            [false, "\nfoobar\n"],
            [true, 'fòôbàř1', 'UTF-8'],
            [false, 'fòô bàř', 'UTF-8'],
            [false, 'fòôbàř2"', 'UTF-8'],
            [true, 'ҠѨњфгШ', 'UTF-8'],
            [false, 'ҠѨњ¨ˆфгШ', 'UTF-8'],
            [true, '丹尼爾111', 'UTF-8'],
            [true, 'دانيال1', 'UTF-8'],
            [false, 'دانيال1 ', 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isAlphanumeric();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect((bool) $result)->toBe($expected);
        }
    });

    it('test isHexadecimal', function () {
        $data = [
            [true, ''],
            [true, 'abcdef'],
            [true, 'ABCDEF'],
            [true, '0123456789'],
            [true, '0123456789AbCdEf'],
            [false, '0123456789x'],
            [false, 'ABCDEFx'],
            [true, 'abcdef', 'UTF-8'],
            [true, 'ABCDEF', 'UTF-8'],
            [true, '0123456789', 'UTF-8'],
            [true, '0123456789AbCdEf', 'UTF-8'],
            [false, '0123456789x', 'UTF-8'],
            [false, 'ABCDEFx', 'UTF-8'],
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isHexadecimal();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect((bool) $result)->toBe($expected);
        }
    });


    it('test isLowerCase', function () {
        $data = [
            [true, ''],
            [true, 'foobar'],
            [false, 'foo bar'],
            [false, 'Foobar'],
            [true, 'fòôbàř', 'UTF-8'],
            [false, 'fòôbàř2', 'UTF-8'],
            [false, 'fòô bàř', 'UTF-8'],
            [false, 'fòôbÀŘ', 'UTF-8'],
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isLowerCase();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect((bool) $result)->toBe($expected);
        }
    });

    it('test isUpperCase', function () {
        $data = [
            [true, ''],
            [true, 'FOOBAR'],
            [false, 'FOO BAR'],
            [false, 'fOOBAR'],
            [true, 'FÒÔBÀŘ', 'UTF-8'],
            [false, 'FÒÔBÀŘ2', 'UTF-8'],
            [false, 'FÒÔ BÀŘ', 'UTF-8'],
            [false, 'FÒÔBàř', 'UTF-8'],
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isUpperCase();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect((bool) $result)->toBe($expected);
        }
    });

    it('test length', function () {
        $data = [
            [11, '  foo bar  '],
            [1, 'f'],
            [0, ''],
            [7, 'fòô bàř', 'UTF-8']
        ];

        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->length();

            \expect($result)->toBeA('int');
            \expect((string) $stringy)->toBe($str);
            \expect((int) $result)->toBe($expected);
        }
    });

    it('test isJson', function () {
        $data = [
            [false, ''],
            [false, '  '],
            [true, 'null'],
            [true, 'true'],
            [true, 'false'],
            [true, '[]'],
            [true, '{}'],
            [true, '123'],
            [true, '{"foo": "bar"}'],
            [false, '{"foo":"bar",}'],
            [false, '{"foo"}'],
            [true, '["foo"]'],
            [false, '{"foo": "bar"]'],
            [true, '123', 'UTF-8'],
            [true, '{"fòô": "bàř"}', 'UTF-8'],
            [false, '{"fòô":"bàř",}', 'UTF-8'],
            [false, '{"fòô"}', 'UTF-8'],
            [false, '["fòô": "bàř"]', 'UTF-8'],
            [true, '["fòô"]', 'UTF-8'],
            [false, '{"fòô": "bàř"]', 'UTF-8'],
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->isJson();

            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect((bool) $result)->toBe($expected);
        }
 
    });

    context('test isSerialized', function () {
        $data = [
            [false, ''],
            [true, 'a:1:{s:3:"foo";s:3:"bar";}'],
            [false, 'a:1:{s:3:"foo";s:3:"bar"}'],
            [true, serialize(['foo' => 'bar'])],
            [true, 'a:1:{s:5:"fòô";s:5:"bàř";}', 'UTF-8'],
            [false, 'a:1:{s:5:"fòô";s:5:"bàř"}', 'UTF-8'],
            [true, serialize(['fòô' => 'bár']), 'UTF-8'],
        ];
        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $encoding) = $value;
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->isSerialized();

                \expect($result)->toBeA('boolean');
                \expect((string) $stringy)->toBe($str);
                \expect((bool) $result)->toBe($expected);
            });
        }
    });

    context('test isBase64', function () {
        $data = [
            [false, ' '],
            [true, ''],
            [true, base64_encode('FooBar') ],
            [true, base64_encode(' ') ],
            [true, base64_encode('FÒÔBÀŘ') ],
            [true, base64_encode('συγγραφέας') ],
            [false, 'Foobar'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                list($expected, $str) = $value;

                $stringy = __('Stringy')::create($str);
                $result = $stringy->isBase64();

                \expect($result)->toBeA('boolean');
                \expect((string) $stringy)->toBe($str);
                \expect((bool) $result)->toBe($expected);
            });
        }
    });

    context('test longestCommonPrefix', function () {
        $data = [
            ['foo', 'foobar', 'foo bar'],
            ['foo bar', 'foo bar', 'foo bar'],
            ['f', 'foo bar', 'far boo'],
            ['', 'toy car', 'foo bar'],
            ['', 'foo bar', ''],
            ['fòô', 'fòôbar', 'fòô bar', 'UTF-8'],
            ['fòô bar', 'fòô bar', 'fòô bar', 'UTF-8'],
            ['fò', 'fòô bar', 'fòr bar', 'UTF-8'],
            ['', 'toy car', 'fòô bar', 'UTF-8'],
            ['', 'fòô bar', '', 'UTF-8'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $otherStr, $encoding) = $value;

                $stringy = __('Stringy')::create($str);
                $result = $stringy->longestCommonPrefix($otherStr);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test longestCommonSuffix', function () {
        $data = [
            ['bar', 'foobar', 'foo bar'],
            ['foo bar', 'foo bar', 'foo bar'],
            ['ar', 'foo bar', 'boo far'],
            ['', 'foo bad', 'foo bar'],
            ['', 'foo bar', ''],
            ['bàř', 'fòôbàř', 'fòô bàř', 'UTF-8'],
            ['fòô bàř', 'fòô bàř', 'fòô bàř', 'UTF-8'],
            [' bàř', 'fòô bàř', 'fòr bàř', 'UTF-8'],
            ['', 'toy car', 'fòô bàř', 'UTF-8'],
            ['', 'fòô bàř', '', 'UTF-8'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $otherStr, $encoding) = $value;

                $stringy = __('Stringy')::create($str);
                $result = $stringy->longestCommonSuffix($otherStr);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    xcontext('test longestCommonSubstring', function () {
        $data = [
            // ['ff', 'aff', 'ff'],
            // ['foo', 'foobar', 'foo bar'],
            // ['foo bar', 'foo bar', 'foo bar'],
            // ['oo ', 'foo bar', 'boo far'],
            // ['foo ba', 'foo bad', 'foo bar'],
            // ['', 'foo bar', ''],
            // ['fòô', 'fòôbàř', 'fòô bàř', 'UTF-8'],
            // ['fòô bàř', 'fòô bàř', 'fòô bàř', 'UTF-8'],
            [' bàř', 'fòô bàř', 'fòr bàř', 'UTF-8'],
            [' ', 'toy car', 'fòô bàř', 'UTF-8'],
            ['', 'fòô bàř', '', 'UTF-8'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $otherStr, $encoding) = $value;

                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->longestCommonSubstring($otherStr);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test pad', function () {
        $data = [
            // length <= str
            ['foo bar', 'foo bar', -1],
            ['foo bar', 'foo bar', 7],
            ['fòô bàř', 'fòô bàř', 7, ' ', 'right', 'UTF-8'],
            // right
            ['foo bar  ', 'foo bar', 9],
            ['foo bar_*', 'foo bar', 9, '_*', 'right'],
            ['fòô bàř¬ø¬', 'fòô bàř', 10, '¬ø', 'right', 'UTF-8'],
            // left
            ['  foo bar', 'foo bar', 9, ' ', 'left'],
            ['_*foo bar', 'foo bar', 9, '_*', 'left'],
            ['¬ø¬fòô bàř', 'fòô bàř', 10, '¬ø', 'left', 'UTF-8'],
            // both
            ['foo bar ', 'foo bar', 8, ' ', 'both'],
            ['¬fòô bàř¬ø', 'fòô bàř', 10, '¬ø', 'both', 'UTF-8'],
            ['¬øfòô bàř¬øÿ', 'fòô bàř', 12, '¬øÿ', 'both', 'UTF-8']
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $length, $padStr, $padType, $encoding) = $value;
                
                $padType = $padType ?? 'right';

                $padStr = $padStr ?? ' ';

                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->pad($length, $padStr, $padType);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test padLeft', function () {
        $data = [
            ['  foo bar', 'foo bar', 9],
            ['_*foo bar', 'foo bar', 9, '_*'],
            ['_*_foo bar', 'foo bar', 10, '_*'],
            ['  fòô bàř', 'fòô bàř', 9, ' ', 'UTF-8'],
            ['¬øfòô bàř', 'fòô bàř', 9, '¬ø', 'UTF-8'],
            ['¬ø¬fòô bàř', 'fòô bàř', 10, '¬ø', 'UTF-8'],
            ['¬ø¬øfòô bàř', 'fòô bàř', 11, '¬ø', 'UTF-8'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $length, $padStr, $encoding) = $value;

                $padStr = $padStr ?? ' ';

                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->padLeft($length, $padStr);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });
    context('test padRight', function () {
        $data = [
            ['foo bar  ', 'foo bar', 9],
            ['foo bar_*', 'foo bar', 9, '_*'],
            ['foo bar_*_', 'foo bar', 10, '_*'],
            ['fòô bàř  ', 'fòô bàř', 9, ' ', 'UTF-8'],
            ['fòô bàř¬ø', 'fòô bàř', 9, '¬ø', 'UTF-8'],
            ['fòô bàř¬ø¬', 'fòô bàř', 10, '¬ø', 'UTF-8'],
            ['fòô bàř¬ø¬ø', 'fòô bàř', 11, '¬ø', 'UTF-8'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $length, $padStr, $encoding) = $value;

                $padStr = $padStr ?? ' ';

                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->padRight($length, $padStr);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });
    context('test padBoth', function () {
        $data = [
            ['foo bar ', 'foo bar', 8],
            [' foo bar ', 'foo bar', 9, ' '],
            ['fòô bàř ', 'fòô bàř', 8, ' ', 'UTF-8'],
            [' fòô bàř ', 'fòô bàř', 9, ' ', 'UTF-8'],
            ['fòô bàř¬', 'fòô bàř', 8, '¬ø', 'UTF-8'],
            ['¬fòô bàř¬', 'fòô bàř', 9, '¬ø', 'UTF-8'],
            ['¬fòô bàř¬ø', 'fòô bàř', 10, '¬ø', 'UTF-8'],
            ['¬øfòô bàř¬ø', 'fòô bàř', 11, '¬ø', 'UTF-8'],
            ['¬fòô bàř¬ø', 'fòô bàř', 10, '¬øÿ', 'UTF-8'],
            ['¬øfòô bàř¬ø', 'fòô bàř', 11, '¬øÿ', 'UTF-8'],
            ['¬øfòô bàř¬øÿ', 'fòô bàř', 12, '¬øÿ', 'UTF-8']
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $length, $padStr, $encoding) = $value;

                $padStr = $padStr ?? ' ';

                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->padBoth($length, $padStr);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test repeat', function () {
        $data = [
            ['', 'foo', 0],
            ['foo', 'foo', 1],
            ['foofoo', 'foo', 2],
            ['foofoofoo', 'foo', 3],
            ['fòô', 'fòô', 1, 'UTF-8'],
            ['fòôfòô', 'fòô', 2, 'UTF-8'],
            ['fòôfòôfòô', 'fòô', 3, 'UTF-8']
        ];
        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $multiplier, $encoding) = $value;
                
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->repeat($multiplier);
                
                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test replace', function () {
        $data = [
            ['', '', '', ''],
            ['foo', '', '', 'foo'],
            ['foo', '\s', '\s', 'foo'],
            ['foo bar', 'foo bar', '', ''],
            ['foo bar', 'foo bar', 'f(o)o', '\1'],
            ['\1 bar', 'foo bar', 'foo', '\1'],
            ['bar', 'foo bar', 'foo ', ''],
            ['far bar', 'foo bar', 'foo', 'far'],
            ['bar bar', 'foo bar foo bar', 'foo ', ''],
            ['', '', '', '', 'UTF-8'],
            ['fòô', '', '', 'fòô', 'UTF-8'],
            ['fòô', '\s', '\s', 'fòô', 'UTF-8'],
            ['fòô bàř', 'fòô bàř', '', '', 'UTF-8'],
            ['bàř', 'fòô bàř', 'fòô ', '', 'UTF-8'],
            ['far bàř', 'fòô bàř', 'fòô', 'far', 'UTF-8'],
            ['bàř bàř', 'fòô bàř fòô bàř', 'fòô ', '', 'UTF-8'],
        ];

        foreach($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
                @list($expected, $str, $search, $replacement, $encoding) = $value;
                
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->replace($search, $replacement);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toAsiic', function ()  {
        $data = [
            ['foo bar', 'fòô bàř'],
            [' TEST ', ' ŤÉŚŢ '],
            ['f = z = 3', 'φ = ź = 3'],
            ['perevirka', 'перевірка'],
            ['lysaya gora', 'лысая гора'],
            ['user@host', 'user@host'],
            ['shchuka', 'щука'],
            ['', '漢字'],
            ['xin chao the gioi', 'xin chào thế giới'],
            ['XIN CHAO THE GIOI', 'XIN CHÀO THẾ GIỚI'],
            ['dam phat chet luon', 'đấm phát chết luôn'],
            [' ', ' '], // no-break space (U+00A0)
            ['           ', '           '], // spaces U+2000 to U+200A
            [' ', ' '], // narrow no-break space (U+202F)
            [' ', ' '], // medium mathematical space (U+205F)
            [' ', '　'], // ideographic space (U+3000)
            ['', '𐍉'], // some uncommon, unsupported character (U+10349)
            ['𐍉', '𐍉', 'en', false],
            ['aouAOU', 'äöüÄÖÜ'],
            ['aeoeueAEOEUE', 'äöüÄÖÜ', 'de'],
            ['aeoeueAEOEUE', 'äöüÄÖÜ', 'de_DE']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $language, $removeUnsupported) = $value;
            $language = $language ?? 'en';
            $removeUnsupported = $removeUnsupported ?? true;

            $stringy = __('Stringy')::create($str);
            $result = $stringy->toAscii($language, $removeUnsupported);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toSpaces', function ()  {
        $data = [
            ['    foo    bar    ', '	foo	bar	'],
            ['     foo     bar     ', '	foo	bar	', 5],
            ['    foo  bar  ', '		foo	bar	', 2],
            ['foobar', '	foo	bar	', 0],
            ["    foo\n    bar", "	foo\n	bar"],
            ["    fòô\n    bàř", "	fòô\n	bàř"]
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $tabLength) = $value;
            $tabLength = $tabLength ?? 4;

            $stringy = __('Stringy')::create($str);
            $result = $stringy->toSpaces($tabLength);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test underscored', function () {
        $data = [
            ['test_case', 'testCase'],
            ['test_case', 'Test-Case'],
            ['test_case', 'test case'],
            ['test_case', 'test -case'],
            ['_test_case', '-test - case'],
            ['test_case', 'test_case'],
            ['test_c_test', '  test c test'],
            ['test_u_case', 'TestUCase'],
            ['test_c_c_test', 'TestCCTest'],
            ['string_with1number', 'string_with1number'],
            ['string_with_2_2_numbers', 'String-with_2_2 numbers'],
            ['1test2case', '1test2case'],
            ['yes_we_can', 'yesWeCan'],
            ['test_σase', 'test Σase', 'UTF-8'],
            ['στανιλ_case', 'Στανιλ case', 'UTF-8'],
            ['σash_case', 'Σash  Case', 'UTF-8']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $encoding) = $value;

            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->underscored();

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toUpperCase', function () {
        $data = [
            ['FOO BAR', 'foo bar'],
            [' FOO_BAR ', ' FOO_bar '],
            ['FÒÔ BÀŘ', 'fòô bàř', 'UTF-8'],
            [' FÒÔ_BÀŘ ', ' FÒÔ_bàř ', 'UTF-8'],
            ['ΑΥΤΟΚΊΝΗΤΟ', 'αυτοκίνητο', 'UTF-8'],
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $encoding) = $value;

            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->toUpperCase();

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test truncate', function () {
        $data = [
            ['Test foo bar', 'Test foo bar', 12],
            ['Test foo ba', 'Test foo bar', 11],
            ['Test foo', 'Test foo bar', 8],
            ['Test fo', 'Test foo bar', 7],
            ['Test', 'Test foo bar', 4],
            ['Test foo bar', 'Test foo bar', 12, '...'],
            ['Test foo...', 'Test foo bar', 11, '...'],
            ['Test ...', 'Test foo bar', 8, '...'],
            ['Test...', 'Test foo bar', 7, '...'],
            ['T...', 'Test foo bar', 4, '...'],
            ['Test fo....', 'Test foo bar', 11, '....'],
            ['Test fòô bàř', 'Test fòô bàř', 12, '', 'UTF-8'],
            ['Test fòô bà', 'Test fòô bàř', 11, '', 'UTF-8'],
            ['Test fòô', 'Test fòô bàř', 8, '', 'UTF-8'],
            ['Test fò', 'Test fòô bàř', 7, '', 'UTF-8'],
            ['Test', 'Test fòô bàř', 4, '', 'UTF-8'],
            ['Test fòô bàř', 'Test fòô bàř', 12, 'ϰϰ', 'UTF-8'],
            ['Test fòô ϰϰ', 'Test fòô bàř', 11, 'ϰϰ', 'UTF-8'],
            ['Test fϰϰ', 'Test fòô bàř', 8, 'ϰϰ', 'UTF-8'],
            ['Test ϰϰ', 'Test fòô bàř', 7, 'ϰϰ', 'UTF-8'],
            ['Teϰϰ', 'Test fòô bàř', 4, 'ϰϰ', 'UTF-8'],
            ['What are your pl...', 'What are your plans today?', 19, '...']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $length, $substring, $encoding) = $value;

            $substring = $substring ?? '';

            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->truncate($length, $substring);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toTitleCase', function () {
        $data = [
            ['Foo Bar', 'foo bar'],
            [' Foo_Bar ', ' foo_bar '],
            ['Fòô Bàř', 'fòô bàř', 'UTF-8'],
            [' Fòô_Bàř ', ' fòô_bàř ', 'UTF-8'],
            ['Αυτοκίνητο Αυτοκίνητο', 'αυτοκίνητο αυτοκίνητο', 'UTF-8'],
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $encoding) = $value;

            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->toTitleCase();

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toTabs', function () {
        $data = [
            ['	foo	bar	', '    foo    bar    '],
            ['	foo	bar	', '     foo     bar     ', 5],
            ['		foo	bar	', '    foo  bar  ', 2],
            ["	foo\n	bar", "    foo\n    bar"],
            ["	fòô\n	bàř", "    fòô\n    bàř"]
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $tabLength) = $value;
            $tabLength = $tabLength ?? 4;

            $stringy = __('Stringy')::create($str);
            $result = $stringy->toTabs($tabLength);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toLowerCase', function () {
        $data = [
            ['foo bar', 'FOO BAR'],
            [' foo_bar ', ' FOO_bar '],
            ['fòô bàř', 'FÒÔ BÀŘ', 'UTF-8'],
            [' fòô_bàř ', ' FÒÔ_bàř ', 'UTF-8'],
            ['αυτοκίνητο', 'ΑΥΤΟΚΊΝΗΤΟ', 'UTF-8'],
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $tabLength) = $value;
            $tabLength = $tabLength ?? 4;

            $stringy = __('Stringy')::create($str);
            $result = $stringy->toLowerCase($tabLength);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test toBoolean', function () {
        $data = [
            [true, 'true'],
            [true, '1'],
            [true, 'on'],
            [true, 'ON'],
            [true, 'yes'],
            [true, '999'],
            [false, 'false'],
            [false, '0'],
            [false, 'off'],
            [false, 'OFF'],
            [false, 'no'],
            [false, '-999'],
            [false, ''],
            [false, ' '],
            [false, '  ', 'UTF-8'] // narrow no-break space (U+202F)
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $encoding) = $value;

            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->toBoolean();

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((bool) $result)->toBe($expected);
            });
        }
    });

    context('test titleize', function () {
        $ignore = ['at', 'by', 'for', 'in', 'of', 'on', 'out', 'to', 'the'];

        $data = [
            ['Title Case', 'TITLE CASE'],
            ['Testing The Method', 'testing the method'],
            ['Testing the Method', 'testing the method', $ignore],
            ['I Like to Watch Dvds at Home', 'i like to watch DVDs at home',
                $ignore],
            ['Θα Ήθελα Να Φύγει', '  Θα ήθελα να φύγει  ', null, 'UTF-8']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {
            @list($expected, $str, $ignore, $encoding) = $value;

            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->titleize($ignore);

            \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
            \expect((string) $stringy)->toBe($str);
            \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test reverse', function () {
        $data = [
            ['', ''],
            ['raboof', 'foobar'],
            ['řàbôòf', 'fòôbàř', 'UTF-8'],
            ['řàb ôòf', 'fòô bàř', 'UTF-8'],
            ['∂∆ ˚åß', 'ßå˚ ∆∂', 'UTF-8']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {

                @list($str, $expected, $encoding) = $value;

                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->reverse();

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test safeTruncate', function () {
        $data = [
            ['Test foo bar', 'Test foo bar', 12],
            ['Test foo', 'Test foo bar', 11],
            ['Test foo', 'Test foo bar', 8],
            ['Test', 'Test foo bar', 7],
            ['Test', 'Test foo bar', 4],
            ['Test foo bar', 'Test foo bar', 12, '...'],
            ['Test foo...', 'Test foo bar', 11, '...'],
            ['Test...', 'Test foo bar', 8, '...'],
            ['Test...', 'Test foo bar', 7, '...'],
            ['T...', 'Test foo bar', 4, '...'],
            ['Test....', 'Test foo bar', 11, '....'],
            ['Tëst fòô bàř', 'Tëst fòô bàř', 12, '', 'UTF-8'],
            ['Tëst fòô', 'Tëst fòô bàř', 11, '', 'UTF-8'],
            ['Tëst fòô', 'Tëst fòô bàř', 8, '', 'UTF-8'],
            ['Tëst', 'Tëst fòô bàř', 7, '', 'UTF-8'],
            ['Tëst', 'Tëst fòô bàř', 4, '', 'UTF-8'],
            ['Tëst fòô bàř', 'Tëst fòô bàř', 12, 'ϰϰ', 'UTF-8'],
            ['Tëst fòôϰϰ', 'Tëst fòô bàř', 11, 'ϰϰ', 'UTF-8'],
            ['Tëstϰϰ', 'Tëst fòô bàř', 8, 'ϰϰ', 'UTF-8'],
            ['Tëstϰϰ', 'Tëst fòô bàř', 7, 'ϰϰ', 'UTF-8'],
            ['Tëϰϰ', 'Tëst fòô bàř', 4, 'ϰϰ', 'UTF-8'],
            ['What are your plans...', 'What are your plans today?', 22, '...']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {

                @list($expected, $str, $length, $substring, $encoding) = $value;
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->safeTruncate($length, $substring);

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect((string) $result)->toBe($expected);
            });
        }
    });

    context('test shuffle', function () {
        $data = [
            ['foo bar'],
            ['∂∆ ˚åß', 'UTF-8'],
            ['å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', 'UTF-8']
        ];

        foreach ($data as $key => $value) {
            it(__formatMessage($key, $value), function () use ($value) {

                @list($str, $encoding) = $value;
                $encoding = $encoding ?: mb_internal_encoding();
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->shuffle();

                \expect($stringy)->toBeAnInstanceOf(__('Stringy'));
                \expect((string) $stringy)->toBe($str);
                \expect(mb_strlen($str, $encoding))->toBe(mb_strlen((string) $result, $encoding));

                for ($i = 0; $i < mb_strlen($str, $encoding); $i++) {
                    $char = mb_substr($str, $i, 1, $encoding);
                    $countBefore = mb_substr_count($str, $char, $encoding);
                    $countAfter = mb_substr_count($result, $char, $encoding);
                    \expect($countBefore)->toBe($countAfter);
                }
            });
        }
    });
});
