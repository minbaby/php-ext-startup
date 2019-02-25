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

    public static function create($str = '', $encoding = null)
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
        } elseif ($this->supportsEncoding()) {
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
        $str = \mb_strtoupper($first, $this->encoding).$rest;

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
        for ($i = 0, $l = $this->length(); $i < $l; $i++) {
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
        return static::create($this->str.$string, $this->encoding);
    }

    public function prepend($string)
    {
        return static::create($string.$this->str, $this->encoding);
    }

    public function lines()
    {
        $array = $this->split('[\r\n]{1,2}', $this->str);
        for ($i = 0; $i < count($array); $i++) {
            $array[$i] = static::create($array[$i], $this->encoding);
        }

        return $array;
    }

    public function split($pattern, $limit = null)
    {
        if ($limit === 0) {
            return [];
        }
        // mb_split errors when supplied an empty pattern in < PHP 5.4.13
        // and HHVM < 3.8
        if ($pattern === '') {
            return [static::create($this->str, $this->encoding)];
        }
        $regexEncoding = $this->regexEncoding();
        $this->regexEncoding($this->encoding);
        // mb_split returns the remaining unsplit string in the last index when
        // supplying a limit
        $limit = ($limit > 0) ? $limit += 1 : -1;
        static $functionExists;
        if ($functionExists === null) {
            $functionExists = function_exists('\mb_split');
        }
        if ($functionExists) {
            $array = \mb_split($pattern, $this->str, $limit);
        } elseif ($this->supportsEncoding()) {
            $array = \preg_split("/$pattern/", $this->str, $limit);
        }
        $this->regexEncoding($regexEncoding);
        if ($limit > 0 && count($array) === $limit) {
            array_pop($array);
        }
        for ($i = 0; $i < count($array); $i++) {
            $array[$i] = static::create($array[$i], $this->encoding);
        }

        return $array;
    }

    public function lowerCaseFirst()
    {
        $first = \mb_substr($this->str, 0, 1, $this->encoding);
        $rest = \mb_substr($this->str, 1, $this->length() - 1, $this->encoding);
        $str = \mb_strtolower($first, $this->encoding).$rest;

        return static::create($str, $this->encoding);
    }

    public function camelize()
    {
        $encoding = $this->encoding;
        $stringy = $this->trim()->lowerCaseFirst();
        $stringy->str = preg_replace('/^[-_]+/', '', $stringy->str);
        $stringy->str = preg_replace_callback(
            '/[-_\s]+(.)?/u',
            function ($match) use ($encoding) {
                if (isset($match[1])) {
                    return \mb_strtoupper($match[1], $encoding);
                }

                return '';
            },
            $stringy->str
        );
        $stringy->str = preg_replace_callback(
            '/[\d]+(.)?/u',
            function ($match) use ($encoding) {
                return \mb_strtoupper($match[0], $encoding);
            },
            $stringy->str
        );

        return $stringy;
    }

    public function trimLeft($chars = null)
    {
        $chars = ($chars) ? preg_quote($chars) : '[:space:]';

        return $this->regexReplace("^[$chars]+", '');
    }

    public function trimRight($chars = null)
    {
        $chars = ($chars) ? preg_quote($chars) : '[:space:]';

        return $this->regexReplace("[$chars]+\$", '');
    }

    public function between($start, $end, $offset = 0)
    {
        $startIndex = $this->indexOf($start, $offset);
        if ($startIndex === false) {
            return static::create('', $this->encoding);
        }
        $substrIndex = $startIndex + \mb_strlen($start, $this->encoding);
        $endIndex = $this->indexOf($end, $substrIndex);
        if ($endIndex === false) {
            return static::create('', $this->encoding);
        }

        return $this->substr($substrIndex, $endIndex - $substrIndex);
    }

    public function contains($needle, $caseSensitive = true)
    {
        $encoding = $this->encoding;
        if ($caseSensitive) {
            return \mb_strpos($this->str, $needle, 0, $encoding) !== false;
        }

        return \mb_stripos($this->str, $needle, 0, $encoding) !== false;
    }

    public function containsAll($needles, $caseSensitive = true)
    {
        if (empty($needles)) {
            return false;
        }
        foreach ($needles as $needle) {
            if (!$this->contains($needle, $caseSensitive)) {
                return false;
            }
        }

        return true;
    }

    public function containsAny($needles, $caseSensitive = true)
    {
        if (empty($needles)) {
            return false;
        }
        foreach ($needles as $needle) {
            if ($this->contains($needle, $caseSensitive)) {
                return true;
            }
        }

        return false;
    }

    public function countSubstr($substring, $caseSensitive = true)
    {
        if ($caseSensitive) {
            return \mb_substr_count($this->str, $substring, $this->encoding);
        }
        $str = \mb_strtoupper($this->str, $this->encoding);
        $substring = \mb_strtoupper($substring, $this->encoding);

        return \mb_substr_count($str, $substring, $this->encoding);
    }

    public function delimit($delimiter)
    {
        $regexEncoding = $this->regexEncoding();
        $this->regexEncoding($this->encoding);
        $str = $this->eregReplace('\B([A-Z])', '-\1', $this->trim());
        $str = \mb_strtolower($str, $this->encoding);
        $str = $this->eregReplace('[-_\s]+', $delimiter, $str);
        $this->regexEncoding($regexEncoding);

        return static::create($str, $this->encoding);
    }

    public function dasherize()
    {
        return $this->delimit('-');
    }

    // protected function supportsEncoding()
    // {
    //     $supported = ['UTF-8' => true, 'ASCII' => true];
    //     if (isset($supported[$this->encoding])) {
    //         return true;
    //     } else {
    //         throw new \RuntimeException('Stringy method requires the ' .
    //             'mbstring module for encodings other than ASCII and UTF-8. ' .
    //             'Encoding used: ' . $this->encoding);
    //     }
    // }

    public function endsWith($substring, $caseSensitive = true)
    {
        $substringLength = \mb_strlen($substring, $this->encoding);
        $strLength = $this->length();
        $endOfStr = \mb_substr($this->str, $strLength - $substringLength, $substringLength, $this->encoding);
        if (!$caseSensitive) {
            $substring = \mb_strtolower($substring, $this->encoding);
            $endOfStr = \mb_strtolower($endOfStr, $this->encoding);
        }

        return (string) $substring === $endOfStr;
    }

    public function endsWithAny($substrings, $caseSensitive = true)
    {
        if (empty($substrings)) {
            return false;
        }
        foreach ($substrings as $substring) {
            if ($this->endsWith($substring, $caseSensitive)) {
                return true;
            }
        }

        return false;
    }

    public function ensureLeft($substring)
    {
        $stringy = static::create($this->str, $this->encoding);
        if (!$stringy->startsWith($substring)) {
            $stringy->str = $substring.$stringy->str;
        }

        return $stringy;
    }

    public function startsWith($substring, $caseSensitive = true)
    {
        $substringLength = \mb_strlen($substring, $this->encoding);
        $startOfStr = \mb_substr($this->str, 0, $substringLength, $this->encoding);
        if (!$caseSensitive) {
            $substring = \mb_strtolower($substring, $this->encoding);
            $startOfStr = \mb_strtolower($startOfStr, $this->encoding);
        }

        return (string) $substring === $startOfStr;
    }

    public function ensureRight($substring)
    {
        $stringy = static::create($this->str, $this->encoding);
        if (!$stringy->endsWith($substring)) {
            $stringy->str .= $substring;
        }

        return $stringy;
    }

    public function first($n)
    {
        $stringy = static::create($this->str, $this->encoding);
        if ($n < 0) {
            $stringy->str = '';

            return $stringy;
        }

        return $stringy->substr(0, $n);
    }

    protected function matchesPattern($pattern)
    {
        $regexEncoding = $this->regexEncoding();
        $this->regexEncoding($this->encoding);
        $match = \mb_ereg_match($pattern, $this->str);
        $this->regexEncoding($regexEncoding);

        return $match;
    }

    public function isAlpha()
    {
        return $this->matchesPattern('^[[:alpha:]]*$');
    }
    
    public function isBlank()
    {
        return $this->matchesPattern('^[[:space:]]*$');
    }
    
    public function hasLowerCase()
    {
        return $this->matchesPattern('.*[[:lower:]]');
    }

    public function hasUpperCase()
    {
        return $this->matchesPattern('.*[[:upper:]]');
    }

    public function htmlDecode($flags = ENT_COMPAT)
    {
        $str = html_entity_decode($this->str, $flags, $this->encoding);
        return static::create($str, $this->encoding);
    }

    public function htmlEncode($flags = ENT_COMPAT)
    {
        $str = htmlentities($this->str, $flags, $this->encoding);
        return static::create($str, $this->encoding);
    }

    public function removeLeft($substring)
    {
        $stringy = static::create($this->str, $this->encoding);
        if ($stringy->startsWith($substring)) {
            $substringLength = \mb_strlen($substring, $stringy->encoding);
            return $stringy->substr($substringLength);
        }
        return $stringy;
    }

    public function removeRight($substring)
    {
        $stringy = static::create($this->str, $this->encoding);
        if ($stringy->endsWith($substring)) {
            $substringLength = \mb_strlen($substring, $stringy->encoding);
            return $stringy->substr(0, $stringy->length() - $substringLength);
        }
        return $stringy;
    }

    public function last($n)
    {
        $stringy = static::create($this->str, $this->encoding);
        if ($n <= 0) {
            $stringy->str = '';
            return $stringy;
        }
        return $stringy->substr(-$n);
    }

    public function humanize()
    {
        $str = str_replace(['_id', '_'], ['', ' '], $this->str);
        return static::create($str, $this->encoding)->trim()->upperCaseFirst();
    }

    public function insert($substring, $index)
    {
        $stringy = static::create($this->str, $this->encoding);
        if ($index > $stringy->length()) {
            return $stringy;
        }
        $start = \mb_substr($stringy->str, 0, $index, $stringy->encoding);
        $end = \mb_substr($stringy->str, $index, $stringy->length(), $stringy->encoding);
        $stringy->str = $start . $substring . $end;
        return $stringy;
    }

    public function isAlphanumeric()
    {
        return $this->matchesPattern('^[[:alnum:]]*$');
    }

    public function isHexadecimal()
    {
        return $this->matchesPattern('^[[:xdigit:]]*$');
    }

    public function isLowerCase()
    {
        return $this->matchesPattern('^[[:lower:]]*$');
    }

    public function isUpperCase()
    {
        return $this->matchesPattern('^[[:upper:]]*$');
    }

    public function isJson()
    {
        if (!$this->length()) {
            return false;
        }
        json_decode($this->str);
        return (json_last_error() === JSON_ERROR_NONE);
    }

    public function isSerialized()
    {
        return $this->str === 'b:0;' || @unserialize($this->str) !== false;
    }

    public function isBase64()
    {
        return (base64_encode(base64_decode($this->str, true)) === $this->str);
    }

    public function longestCommonPrefix($otherStr)
    {
        $encoding = $this->encoding;
        $maxLength = min($this->length(), \mb_strlen($otherStr, $encoding));
        $longestCommonPrefix = '';
        for ($i = 0; $i < $maxLength; $i++) {
            $char = \mb_substr($this->str, $i, 1, $encoding);
            if ($char == \mb_substr($otherStr, $i, 1, $encoding)) {
                $longestCommonPrefix .= $char;
            } else {
                break;
            }
        }
        return static::create($longestCommonPrefix, $encoding);
    }
    
    public function longestCommonSuffix($otherStr)
    {
        $encoding = $this->encoding;
        $maxLength = min($this->length(), \mb_strlen($otherStr, $encoding));
        $longestCommonSuffix = '';
        for ($i = 1; $i <= $maxLength; $i++) {
            $char = \mb_substr($this->str, -$i, 1, $encoding);
            if ($char == \mb_substr($otherStr, -$i, 1, $encoding)) {
                $longestCommonSuffix = $char . $longestCommonSuffix;
            } else {
                break;
            }
        }
        return static::create($longestCommonSuffix, $encoding);
    }
    
    public function longestCommonSubstring($otherStr)
    {
        // Uses dynamic programming to solve
        // http://en.wikipedia.org/wiki/Longest_common_substring_problem
        $encoding = $this->encoding;
        $stringy = static::create($this->str, $encoding);
        $strLength = $stringy->length();
        $otherLength = \mb_strlen($otherStr, $encoding);
        // Return if either string is empty
        if ($strLength == 0 || $otherLength == 0) {
            $stringy->str = '';
        return $stringy;
        }
        $len = 0;
        $end = 0;
        $table = array_fill(0, $strLength + 1,
            array_fill(0, $otherLength + 1, 0));
        for ($i = 1; $i <= $strLength; $i++) {
            for ($j = 1; $j <= $otherLength; $j++) {
                $strChar = \mb_substr($stringy->str, $i - 1, 1, $encoding);
                $otherChar = \mb_substr($otherStr, $j - 1, 1, $encoding);
                if ($strChar == $otherChar) {
                    $table[$i][$j] = $table[$i - 1][$j - 1] + 1;
                    if ($table[$i][$j] > $len) {
                        $len = $table[$i][$j];
                        $end = $i;
                    }
                } else {
                    $table[$i][$j] = 0;
                }
            }
        }
        $stringy->str = \mb_substr($stringy->str, $end - $len, $len, $encoding);
        return $stringy;
    }
}
