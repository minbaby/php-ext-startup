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

    public function pad($length, $padStr = ' ', $padType = 'right')
    {
        if (!in_array($padType, ['left', 'right', 'both'])) {
            throw new InvalidArgumentException('Pad expects $padType ' .
                "to be one of 'left', 'right' or 'both'");
        }
        switch ($padType) {
            case 'left':
                return $this->padLeft($length, $padStr);
            case 'right':
                return $this->padRight($length, $padStr);
            default:
                return $this->padBoth($length, $padStr);
        }
    }

    public function padBoth($length, $padStr = ' ')
    {
        $padding = $length - $this->length();
        return $this->applyPadding(floor($padding / 2), ceil($padding / 2),
            $padStr);
    }

    public function padLeft($length, $padStr = ' ')
    {
        return $this->applyPadding($length - $this->length(), 0, $padStr);
    }

    public function padRight($length, $padStr = ' ')
    {
        return $this->applyPadding(0, $length - $this->length(), $padStr);
    }

    protected function applyPadding($left = 0, $right = 0, $padStr = ' ')
    {
        $stringy = static::create($this->str, $this->encoding);
        $length = \mb_strlen($padStr, $stringy->encoding);
        $strLength = $stringy->length();
        $paddedLength = $strLength + $left + $right;
        if (!$length || $paddedLength <= $strLength) {
            return $stringy;
        }
        $leftPadding = \mb_substr(str_repeat($padStr, ceil($left / $length)), 0,
            $left, $stringy->encoding);
        $rightPadding = \mb_substr(str_repeat($padStr, ceil($right / $length)),
            0, $right, $stringy->encoding);
        $stringy->str = $leftPadding . $stringy->str . $rightPadding;
        return $stringy;
    }

    public function repeat($multiplier)
    {
        $repeated = str_repeat($this->str, $multiplier);
        return static::create($repeated, $this->encoding);
    }

    public function replace($search, $replacement)
    {
        return $this->regexReplace(preg_quote($search), $replacement);
    }

    public function toAscii($language = 'en', $removeUnsupported = true)
    {
        $str = $this->str;
        $langSpecific = $this->langSpecificCharsArray($language);
        if (!empty($langSpecific)) {
            $str = str_replace($langSpecific[0], $langSpecific[1], $str);
        }
        foreach ($this->charsArray() as $key => $value) {
            $str = str_replace($value, $key, $str);
        }
        if ($removeUnsupported) {
            $str = preg_replace('/[^\x20-\x7E]/u', '', $str);
        }
        return static::create($str, $this->encoding);
    }

    protected static function langSpecificCharsArray($language = 'en')
    {
        $split = preg_split('/[-_]/', $language);
        $language = strtolower($split[0]);
        static $charsArray = [];
        if (isset($charsArray[$language])) {
            return $charsArray[$language];
        }
        $languageSpecific = [
            'de' => [
                ['ä',  'ö',  'ü',  'Ä',  'Ö',  'Ü' ],
                ['ae', 'oe', 'ue', 'AE', 'OE', 'UE'],
            ],
            'bg' => [
                ['х', 'Х', 'щ', 'Щ', 'ъ', 'Ъ', 'ь', 'Ь'],
                ['h', 'H', 'sht', 'SHT', 'a', 'А', 'y', 'Y']
            ]
        ];
        if (isset($languageSpecific[$language])) {
            $charsArray[$language] = $languageSpecific[$language];
        } else {
            $charsArray[$language] = [];
        }
        return $charsArray[$language];
    }

    protected function charsArray()
    {
        static $charsArray;
        if (isset($charsArray)) return $charsArray;
        return $charsArray = [
            '0'     => ['°', '₀', '۰', '０'],
            '1'     => ['¹', '₁', '۱', '１'],
            '2'     => ['²', '₂', '۲', '２'],
            '3'     => ['³', '₃', '۳', '３'],
            '4'     => ['⁴', '₄', '۴', '٤', '４'],
            '5'     => ['⁵', '₅', '۵', '٥', '５'],
            '6'     => ['⁶', '₆', '۶', '٦', '６'],
            '7'     => ['⁷', '₇', '۷', '７'],
            '8'     => ['⁸', '₈', '۸', '８'],
            '9'     => ['⁹', '₉', '۹', '９'],
            'a'     => ['à', 'á', 'ả', 'ã', 'ạ', 'ă', 'ắ', 'ằ', 'ẳ', 'ẵ',
                        'ặ', 'â', 'ấ', 'ầ', 'ẩ', 'ẫ', 'ậ', 'ā', 'ą', 'å',
                        'α', 'ά', 'ἀ', 'ἁ', 'ἂ', 'ἃ', 'ἄ', 'ἅ', 'ἆ', 'ἇ',
                        'ᾀ', 'ᾁ', 'ᾂ', 'ᾃ', 'ᾄ', 'ᾅ', 'ᾆ', 'ᾇ', 'ὰ', 'ά',
                        'ᾰ', 'ᾱ', 'ᾲ', 'ᾳ', 'ᾴ', 'ᾶ', 'ᾷ', 'а', 'أ', 'အ',
                        'ာ', 'ါ', 'ǻ', 'ǎ', 'ª', 'ა', 'अ', 'ا', 'ａ', 'ä'],
            'b'     => ['б', 'β', 'ب', 'ဗ', 'ბ', 'ｂ'],
            'c'     => ['ç', 'ć', 'č', 'ĉ', 'ċ', 'ｃ'],
            'd'     => ['ď', 'ð', 'đ', 'ƌ', 'ȡ', 'ɖ', 'ɗ', 'ᵭ', 'ᶁ', 'ᶑ',
                        'д', 'δ', 'د', 'ض', 'ဍ', 'ဒ', 'დ', 'ｄ'],
            'e'     => ['é', 'è', 'ẻ', 'ẽ', 'ẹ', 'ê', 'ế', 'ề', 'ể', 'ễ',
                        'ệ', 'ë', 'ē', 'ę', 'ě', 'ĕ', 'ė', 'ε', 'έ', 'ἐ',
                        'ἑ', 'ἒ', 'ἓ', 'ἔ', 'ἕ', 'ὲ', 'έ', 'е', 'ё', 'э',
                        'є', 'ə', 'ဧ', 'ေ', 'ဲ', 'ე', 'ए', 'إ', 'ئ', 'ｅ'],
            'f'     => ['ф', 'φ', 'ف', 'ƒ', 'ფ', 'ｆ'],
            'g'     => ['ĝ', 'ğ', 'ġ', 'ģ', 'г', 'ґ', 'γ', 'ဂ', 'გ', 'گ',
                        'ｇ'],
            'h'     => ['ĥ', 'ħ', 'η', 'ή', 'ح', 'ه', 'ဟ', 'ှ', 'ჰ', 'ｈ'],
            'i'     => ['í', 'ì', 'ỉ', 'ĩ', 'ị', 'î', 'ï', 'ī', 'ĭ', 'į',
                        'ı', 'ι', 'ί', 'ϊ', 'ΐ', 'ἰ', 'ἱ', 'ἲ', 'ἳ', 'ἴ',
                        'ἵ', 'ἶ', 'ἷ', 'ὶ', 'ί', 'ῐ', 'ῑ', 'ῒ', 'ΐ', 'ῖ',
                        'ῗ', 'і', 'ї', 'и', 'ဣ', 'ိ', 'ီ', 'ည်', 'ǐ', 'ი',
                        'इ', 'ی', 'ｉ'],
            'j'     => ['ĵ', 'ј', 'Ј', 'ჯ', 'ج', 'ｊ'],
            'k'     => ['ķ', 'ĸ', 'к', 'κ', 'Ķ', 'ق', 'ك', 'က', 'კ', 'ქ',
                        'ک', 'ｋ'],
            'l'     => ['ł', 'ľ', 'ĺ', 'ļ', 'ŀ', 'л', 'λ', 'ل', 'လ', 'ლ',
                        'ｌ'],
            'm'     => ['м', 'μ', 'م', 'မ', 'მ', 'ｍ'],
            'n'     => ['ñ', 'ń', 'ň', 'ņ', 'ŉ', 'ŋ', 'ν', 'н', 'ن', 'န',
                        'ნ', 'ｎ'],
            'o'     => ['ó', 'ò', 'ỏ', 'õ', 'ọ', 'ô', 'ố', 'ồ', 'ổ', 'ỗ',
                        'ộ', 'ơ', 'ớ', 'ờ', 'ở', 'ỡ', 'ợ', 'ø', 'ō', 'ő',
                        'ŏ', 'ο', 'ὀ', 'ὁ', 'ὂ', 'ὃ', 'ὄ', 'ὅ', 'ὸ', 'ό',
                        'о', 'و', 'θ', 'ို', 'ǒ', 'ǿ', 'º', 'ო', 'ओ', 'ｏ',
                        'ö'],
            'p'     => ['п', 'π', 'ပ', 'პ', 'پ', 'ｐ'],
            'q'     => ['ყ', 'ｑ'],
            'r'     => ['ŕ', 'ř', 'ŗ', 'р', 'ρ', 'ر', 'რ', 'ｒ'],
            's'     => ['ś', 'š', 'ş', 'с', 'σ', 'ș', 'ς', 'س', 'ص', 'စ',
                        'ſ', 'ს', 'ｓ'],
            't'     => ['ť', 'ţ', 'т', 'τ', 'ț', 'ت', 'ط', 'ဋ', 'တ', 'ŧ',
                        'თ', 'ტ', 'ｔ'],
            'u'     => ['ú', 'ù', 'ủ', 'ũ', 'ụ', 'ư', 'ứ', 'ừ', 'ử', 'ữ',
                        'ự', 'û', 'ū', 'ů', 'ű', 'ŭ', 'ų', 'µ', 'у', 'ဉ',
                        'ု', 'ူ', 'ǔ', 'ǖ', 'ǘ', 'ǚ', 'ǜ', 'უ', 'उ', 'ｕ',
                        'ў', 'ü'],
            'v'     => ['в', 'ვ', 'ϐ', 'ｖ'],
            'w'     => ['ŵ', 'ω', 'ώ', 'ဝ', 'ွ', 'ｗ'],
            'x'     => ['χ', 'ξ', 'ｘ'],
            'y'     => ['ý', 'ỳ', 'ỷ', 'ỹ', 'ỵ', 'ÿ', 'ŷ', 'й', 'ы', 'υ',
                        'ϋ', 'ύ', 'ΰ', 'ي', 'ယ', 'ｙ'],
            'z'     => ['ź', 'ž', 'ż', 'з', 'ζ', 'ز', 'ဇ', 'ზ', 'ｚ'],
            'aa'    => ['ع', 'आ', 'آ'],
            'ae'    => ['æ', 'ǽ'],
            'ai'    => ['ऐ'],
            'ch'    => ['ч', 'ჩ', 'ჭ', 'چ'],
            'dj'    => ['ђ', 'đ'],
            'dz'    => ['џ', 'ძ'],
            'ei'    => ['ऍ'],
            'gh'    => ['غ', 'ღ'],
            'ii'    => ['ई'],
            'ij'    => ['ĳ'],
            'kh'    => ['х', 'خ', 'ხ'],
            'lj'    => ['љ'],
            'nj'    => ['њ'],
            'oe'    => ['œ', 'ؤ'],
            'oi'    => ['ऑ'],
            'oii'   => ['ऒ'],
            'ps'    => ['ψ'],
            'sh'    => ['ш', 'შ', 'ش'],
            'shch'  => ['щ'],
            'ss'    => ['ß'],
            'sx'    => ['ŝ'],
            'th'    => ['þ', 'ϑ', 'ث', 'ذ', 'ظ'],
            'ts'    => ['ц', 'ც', 'წ'],
            'uu'    => ['ऊ'],
            'ya'    => ['я'],
            'yu'    => ['ю'],
            'zh'    => ['ж', 'ჟ', 'ژ'],
            '(c)'   => ['©'],
            'A'     => ['Á', 'À', 'Ả', 'Ã', 'Ạ', 'Ă', 'Ắ', 'Ằ', 'Ẳ', 'Ẵ',
                        'Ặ', 'Â', 'Ấ', 'Ầ', 'Ẩ', 'Ẫ', 'Ậ', 'Å', 'Ā', 'Ą',
                        'Α', 'Ά', 'Ἀ', 'Ἁ', 'Ἂ', 'Ἃ', 'Ἄ', 'Ἅ', 'Ἆ', 'Ἇ',
                        'ᾈ', 'ᾉ', 'ᾊ', 'ᾋ', 'ᾌ', 'ᾍ', 'ᾎ', 'ᾏ', 'Ᾰ', 'Ᾱ',
                        'Ὰ', 'Ά', 'ᾼ', 'А', 'Ǻ', 'Ǎ', 'Ａ', 'Ä'],
            'B'     => ['Б', 'Β', 'ब', 'Ｂ'],
            'C'     => ['Ç','Ć', 'Č', 'Ĉ', 'Ċ', 'Ｃ'],
            'D'     => ['Ď', 'Ð', 'Đ', 'Ɖ', 'Ɗ', 'Ƌ', 'ᴅ', 'ᴆ', 'Д', 'Δ',
                        'Ｄ'],
            'E'     => ['É', 'È', 'Ẻ', 'Ẽ', 'Ẹ', 'Ê', 'Ế', 'Ề', 'Ể', 'Ễ',
                        'Ệ', 'Ë', 'Ē', 'Ę', 'Ě', 'Ĕ', 'Ė', 'Ε', 'Έ', 'Ἐ',
                        'Ἑ', 'Ἒ', 'Ἓ', 'Ἔ', 'Ἕ', 'Έ', 'Ὲ', 'Е', 'Ё', 'Э',
                        'Є', 'Ə', 'Ｅ'],
            'F'     => ['Ф', 'Φ', 'Ｆ'],
            'G'     => ['Ğ', 'Ġ', 'Ģ', 'Г', 'Ґ', 'Γ', 'Ｇ'],
            'H'     => ['Η', 'Ή', 'Ħ', 'Ｈ'],
            'I'     => ['Í', 'Ì', 'Ỉ', 'Ĩ', 'Ị', 'Î', 'Ï', 'Ī', 'Ĭ', 'Į',
                        'İ', 'Ι', 'Ί', 'Ϊ', 'Ἰ', 'Ἱ', 'Ἳ', 'Ἴ', 'Ἵ', 'Ἶ',
                        'Ἷ', 'Ῐ', 'Ῑ', 'Ὶ', 'Ί', 'И', 'І', 'Ї', 'Ǐ', 'ϒ',
                        'Ｉ'],
            'J'     => ['Ｊ'],
            'K'     => ['К', 'Κ', 'Ｋ'],
            'L'     => ['Ĺ', 'Ł', 'Л', 'Λ', 'Ļ', 'Ľ', 'Ŀ', 'ल', 'Ｌ'],
            'M'     => ['М', 'Μ', 'Ｍ'],
            'N'     => ['Ń', 'Ñ', 'Ň', 'Ņ', 'Ŋ', 'Н', 'Ν', 'Ｎ'],
            'O'     => ['Ó', 'Ò', 'Ỏ', 'Õ', 'Ọ', 'Ô', 'Ố', 'Ồ', 'Ổ', 'Ỗ',
                        'Ộ', 'Ơ', 'Ớ', 'Ờ', 'Ở', 'Ỡ', 'Ợ', 'Ø', 'Ō', 'Ő',
                        'Ŏ', 'Ο', 'Ό', 'Ὀ', 'Ὁ', 'Ὂ', 'Ὃ', 'Ὄ', 'Ὅ', 'Ὸ',
                        'Ό', 'О', 'Θ', 'Ө', 'Ǒ', 'Ǿ', 'Ｏ', 'Ö'],
            'P'     => ['П', 'Π', 'Ｐ'],
            'Q'     => ['Ｑ'],
            'R'     => ['Ř', 'Ŕ', 'Р', 'Ρ', 'Ŗ', 'Ｒ'],
            'S'     => ['Ş', 'Ŝ', 'Ș', 'Š', 'Ś', 'С', 'Σ', 'Ｓ'],
            'T'     => ['Ť', 'Ţ', 'Ŧ', 'Ț', 'Т', 'Τ', 'Ｔ'],
            'U'     => ['Ú', 'Ù', 'Ủ', 'Ũ', 'Ụ', 'Ư', 'Ứ', 'Ừ', 'Ử', 'Ữ',
                        'Ự', 'Û', 'Ū', 'Ů', 'Ű', 'Ŭ', 'Ų', 'У', 'Ǔ', 'Ǖ',
                        'Ǘ', 'Ǚ', 'Ǜ', 'Ｕ', 'Ў', 'Ü'],
            'V'     => ['В', 'Ｖ'],
            'W'     => ['Ω', 'Ώ', 'Ŵ', 'Ｗ'],
            'X'     => ['Χ', 'Ξ', 'Ｘ'],
            'Y'     => ['Ý', 'Ỳ', 'Ỷ', 'Ỹ', 'Ỵ', 'Ÿ', 'Ῠ', 'Ῡ', 'Ὺ', 'Ύ',
                        'Ы', 'Й', 'Υ', 'Ϋ', 'Ŷ', 'Ｙ'],
            'Z'     => ['Ź', 'Ž', 'Ż', 'З', 'Ζ', 'Ｚ'],
            'AE'    => ['Æ', 'Ǽ'],
            'Ch'    => ['Ч'],
            'Dj'    => ['Ђ'],
            'Dz'    => ['Џ'],
            'Gx'    => ['Ĝ'],
            'Hx'    => ['Ĥ'],
            'Ij'    => ['Ĳ'],
            'Jx'    => ['Ĵ'],
            'Kh'    => ['Х'],
            'Lj'    => ['Љ'],
            'Nj'    => ['Њ'],
            'Oe'    => ['Œ'],
            'Ps'    => ['Ψ'],
            'Sh'    => ['Ш'],
            'Shch'  => ['Щ'],
            'Ss'    => ['ẞ'],
            'Th'    => ['Þ'],
            'Ts'    => ['Ц'],
            'Ya'    => ['Я'],
            'Yu'    => ['Ю'],
            'Zh'    => ['Ж'],
            ' '     => ["\xC2\xA0", "\xE2\x80\x80", "\xE2\x80\x81",
                        "\xE2\x80\x82", "\xE2\x80\x83", "\xE2\x80\x84",
                        "\xE2\x80\x85", "\xE2\x80\x86", "\xE2\x80\x87",
                        "\xE2\x80\x88", "\xE2\x80\x89", "\xE2\x80\x8A",
                        "\xE2\x80\xAF", "\xE2\x81\x9F", "\xE3\x80\x80",
                        "\xEF\xBE\xA0"],
        ];
    }

    public function toSpaces($tabLength = 4)
    {
        $spaces = str_repeat(' ', $tabLength);
        $str = str_replace("\t", $spaces, $this->str);
        return static::create($str, $this->encoding);
    }

    public function underscored()
    {
        return $this->delimit('_');
    }

    public function toUpperCase()
    {
        $str = \mb_strtoupper($this->str, $this->encoding);
        return static::create($str, $this->encoding);
    }

    public function truncate($length, $substring = '')
    {
        $stringy = static::create($this->str, $this->encoding);
        if ($length >= $stringy->length()) {
            return $stringy;
        }
        // Need to further trim the string so we can append the substring
        $substringLength = \mb_strlen($substring, $stringy->encoding);
        $length = $length - $substringLength;
        $truncated = \mb_substr($stringy->str, 0, $length, $stringy->encoding);
        $stringy->str = $truncated . $substring;
        return $stringy;
    }

    public function toTitleCase()
    {
        $str = \mb_convert_case($this->str, \MB_CASE_TITLE, $this->encoding);
        return static::create($str, $this->encoding);
    }

    public function toTabs($tabLength = 4)
    {
        $spaces = str_repeat(' ', $tabLength);
        $str = str_replace($spaces, "\t", $this->str);
        return static::create($str, $this->encoding);
    }

    public function toLowerCase()
    {
        $str = \mb_strtolower($this->str, $this->encoding);

        return static::create($str, $this->encoding);
    }

    public function toBoolean()
    {
        $key = $this->toLowerCase()->str;
        $map = [
            'true'  => true,
            '1'     => true,
            'on'    => true,
            'yes'   => true,
            'false' => false,
            '0'     => false,
            'off'   => false,
            'no'    => false
        ];

        if (array_key_exists($key, $map)) {
            return $map[$key];
        } elseif (is_numeric($this->str)) {
            return (intval($this->str) > 0);
        }

        return (bool) $this->regexReplace('[[:space:]]', '')->str;
    }
}
