<?php
include __DIR__ . '/vendor/autoload.php';

_ns(NS_STRINGY);

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

    // var_dump((string)$result);
}
