<?php

namespace Minbaby\Startup\Php\Stringy;

use InvalidArgumentException;

class Stringy implements \Countable, \IteratorAggregate, \ArrayAccess
{

    protected $str;

    protected $encoding;

    public function __construct($str = '', $encoding = null)
    {
        if (is_array($str)) {
            throw new InvalidArgumentException(
                'Passed value cannot be an array'
            );
        } elseif (is_object($str) && !method_exists($str, '__toString')) {
            throw new InvalidArgumentException(
                'Passed object must have a __toString method'
            );
        }

        $this->str = (string) $str;
        $this->encoding = $encoding ?: \mb_internal_encoding();
    }

    public function __toString()
    {
        return $this->str;
    }

    public function getEncoding()
    {
        return $this->encoding;
    }

    public static function create($str, $encoding)
    {
        return new static($str, $encoding);
    }

    public function collapseWhiteSpace()
    {
        return $this->regexReplace('[[:space:]]+', ' ')->trim();
    }

    public function regexReplace($pattern, $replacement, $options = 'msr')
    {
        $regexEncoding = $this->regexEncoding();
        $this->regexEncoding($this->encoding);

        $str = $this->eregReplace($pattern, $replacement, $this->str, $options);
        $this->regexEncoding($regexEncoding);

        return static::create($str, $this->encoding);
    }

    protected function regexEncoding()
    {
        static $functionExists;

        if ($functionExists === null) {
            $functionExists = function_exists('\mb_regex_encoding');
        }

        if ($functionExists) {
            $args = func_get_args();
            return \call_user_func_array('\mb_regex_encoding', $args);
        }
    }

    protected function eregReplace($pattern, $replacement, $string, $option = 'msr')
    {
        static $functionExists;
        if ($functionExists === null) {
            $functionExists = function_exists('\mb_split');
        }
        if ($functionExists) {
            return \mb_ereg_replace($pattern, $replacement, $string, $option);
        } else if ($this->supportsEncoding()) {
            $option = str_replace('r', '', $option);
            return \preg_replace("/$pattern/u$option", $replacement, $string);
        }
    }

    public function trim($chars = null)
    {
        $chars = ($chars) ? preg_quote($chars) : '[:space:]';
        return $this->regexReplace("^[$chars]+|[$chars]+\$", '');
    }

    public function swapCase()
    {
        $stringy = static::create($this->str, $this->encoding);
        $encoding = $stringy->encoding;
        $stringy->str = preg_replace_callback(
            '/[\S]/u',
            function ($match) use ($encoding) {
                if ($match[0] == \mb_strtoupper($match[0], $encoding)) {
                    return \mb_strtolower($match[0], $encoding);
                }
                return \mb_strtoupper($match[0], $encoding);
            },
            $stringy->str
        );
        return $stringy;
    }

    public function upperCaseFirst()
    {
        $first = \mb_substr($this->str, 0, 1, $this->encoding);
        $rest = \mb_substr($this->str, 1, $this->length() - 1, $this->encoding);
        $str = \mb_strtoupper($first, $this->encoding) . $rest;
        return static::create($str, $this->encoding);
    }
    
    public function length()
    {
        return \mb_strlen($this->str, $this->encoding);
    }

    public function count()
    {
        return $this->length();
    }

    public function getIterator()
    {
        return new \ArrayIterator($this->chars());
    }

    public function chars()
    {
        $chars = [];
        for($i = 0, $l = $this->length();  $i < $l; $i++)
        {
            $chars[] = $this->at($i)->str;
        }

        return $chars;
    }

    public function at($index)
    {
        return $this->substr($index, 1);
    }

    public function substr($start, $length = null)
    {
        $length = $length === null ? $this->length() : $length;
        $str = \mb_substr($this->str, $start, $length, $this->encoding);
        return static::create($str, $this->encoding);
    }

    public function offsetExists($offset)
    {
        $length = $this->length();
        $offset = (int) $offset;

        if ($offset >= 0) {
            return $length > $offset;
        }

        return $length >= abs($offset);
    }
    
    public function offsetGet($offset)
    {
        $offset = (int) $offset;
        $length = $this->length();
        if (($offset >= 0 && $length <= $offset) || $length < abs($offset)) {
            throw new \OutOfBoundsException('No character exists at the index');
        }
        return \mb_substr($this->str, $offset, 1, $this->encoding);
    }

    public function offsetSet($offset, $value)
    {
        throw new \Exception('Stringy object is immutable, cannot modify char');
    }

    public function offsetUnset($offset)
    {
        throw new \Exception('Stringy object is immutable, cannot unset char');
    }

    public function indexOf($needle, $offset = 0)
    {
        return \mb_strpos(
            $this->str, 
            (string) $needle,
            (int) $offset, 
            $this->encoding
        );
    }

    public function indexOfLast($needle, $offset = 0)
    {
        return \mb_strrpos(
            $this->str, 
            (string) $needle,
            (int) $offset, 
            $this->encoding
        );
    }

    public function append($string) 
    {
        return static::create($this->str . $string, $this->encoding);
    }
}
