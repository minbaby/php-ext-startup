<?php

namespace Minbaby\Startup\Spec\Stringy;

use function Kahlan\context;


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
        \expect((string) $stringy->swapCase())->toBe("X    Y");
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
            ['σamelCase', 'σamel  Case', 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->camelize();
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
            ['fòô', '           fòô', null, 'UTF-8'] // spaces U+2000 to U+200A
        ];

        foreach($data as $value) {
            @list($expected, $str, $chars, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->trim($chars);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
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
            ['fòô', '           fòô', null, 'UTF-8'] // spaces U+2000 to U+200A
        ];

        foreach($data as $value) {
            @list($expected, $str, $chars, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->trimLeft($chars);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
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
            ['fòô', 'fòô           ', null, 'UTF-8'] // spaces U+2000 to U+200A
        ];

        foreach($data as $value) {
            @list($expected, $str, $chars, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->trimRight($chars);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
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

        foreach($data as $value) {
            @list($expected, $str, $index, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->at($index);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
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
            ['bàř', '{fòô} and {bàř}', '{', '}', 1, 'UTF-8']
        ];
        foreach($data as $value) {
            @list($expected, $str, $start, $end, $offset, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->between($start, $end, $offset);
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
        }
    });

    \context('contains', function(){
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
            [false, 'å´¥©¨ˆßå˚ ∆∂˙©å∑¥øœ¬', ' ßÅ˚', false, 'UTF-8']
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
            foreach($data as $value) {
                @list($expected, $haystack, $needle, $caseSensitive, $encoding) = $value;
                $stringy = __('Stringy')::create($haystack, $encoding);
                $result = $stringy->contains($needle, $caseSensitive === true || $caseSensitive === NULL); // 默认值是　ＮＵＬｌ，　这里需要特殊处理
                \expect($result)->toBeA('bool');
                \expect((string)$stringy)->toBe($haystack);
                \expect((bool)$result)->toBe($expected);
            }
        });

        it('test containsAll', function () use ($dataContainsAll) {
            foreach($dataContainsAll as $value) {
                @list($expected, $haystack, $needle, $caseSensitive, $encoding) = $value;
                
                $stringy = __('Stringy')::create($haystack, $encoding);
                $result = $stringy->containsAll($needle, $caseSensitive === true || $caseSensitive === NULL); // 默认值是　ＮＵＬｌ，　这里需要特殊处理
                \expect($result)->toBeA('bool');
                \expect((string)$stringy)->toBe($haystack);
                \expect((bool)$result)->toBe($expected);
            }
        });

        it('test containsAny', function () use ($dataContainsAny) {
            foreach($dataContainsAny as $value) {
                @list($expected, $haystack, $needles, $caseSensitive, $encoding) = $value;
                $stringy = __('Stringy')::create($haystack, $encoding);
                
                $result = $stringy->containsAny($needles, $caseSensitive === true || $caseSensitive === NULL);
                \expect($result)->toBeA('bool');
                \expect((string)$stringy)->toBe($haystack);
                \expect((bool)$result)->toBe($expected);
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
            [['fòô', 'bàř', 'baz'], 'fòô,bàř,baz', ',', 10, 'UTF-8']
        ];

        foreach($data as $value) {
            @list($expected, $str, $pattern, $limit, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->split($pattern, $limit);
            \expect($result)->toBeA('array');
            foreach ($result as $string) {
                \expect($string)->toBeAnInstanceOf(__('Stringy'));
            }
            for ($i = 0; $i < count($expected); $i++) {
                \expect((string)$result[$i])->toBe($expected[$i]);
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

        foreach($data as $value) {
            @list($expected, $str, $encoding) = $value;
            {
                $stringy = __('Stringy')::create($str, $encoding);
                $result = $stringy->collapseWhitespace();
                \expect($result)->toBeAnInstanceOf(__('Stringy'));
                \expect((string)$stringy)->toBe($str);
                \expect((string)$result)->toBe($expected);
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
        foreach($data as $value) {
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
            [2, 'συγγραφέας', 'Σ', false, 'UTF-8']
        ];

        foreach($data as $value)
        {
            @list($expected, $str, $substring, $caseSensitive, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->countSubstr($substring, $caseSensitive === true || $caseSensitive === NULL);
            \expect($result)->toBeA('int');
            \expect($result)->toBe($expected);
            \expect((string)$stringy)->toBe($str);
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
            ['test ύα σase', 'test Σase', ' ύα ', 'UTF-8',],
            ['στανιλαcase', 'Στανιλ case', 'α', 'UTF-8',],
            ['σashΘcase', 'Σash  Case', 'Θ', 'UTF-8']
        ];
    
        foreach($data as $value) {
            @list($expected, $str, $delimiter, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->delimit($delimiter);

            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
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
            ['σash-case', 'Σash  Case', 'UTF-8']
        ];

        foreach($data as $value)
        {
            @list($expected, $str, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->dasherize();
            \expect($result)->toBeAnInstanceOf(__('Stringy'));
            \expect((string)$stringy)->toBe($str);
            \expect((string)$result)->toBe($expected);
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
        foreach($data as $value) {
            @list($expected, $str, $substring, $caseSensitive, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->endsWith($substring, $caseSensitive === true || $caseSensitive === NULL);

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
        ];

        foreach($data as $value) {
            @list($expected, $str, $substrings, $caseSensitive, $encoding) = $value;
            $stringy = __('Stringy')::create($str, $encoding);
            $result = $stringy->endsWithAny($substrings, $caseSensitive === true || $caseSensitive === NULL);
            
            \expect($result)->toBeA('boolean');
            \expect((string) $stringy)->toBe($str);
            \expect($result)->toBe($expected);
        }

    });
});
