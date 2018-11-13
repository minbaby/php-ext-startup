<?php

namespace Minbaby\Ext\Stringy;

use InvalidArgumentException;

class Stringy
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
}
