#ifndef STARTUP_STRINGY_STRINGY_H
#define STARTUP_STRINGY_STRINGY_H

#include "php.h"

#define PHP_STARTUP_STRINGY_NS(cls) PHP_STARTUP_NS_NAME "Stringy\\" #cls

extern  zend_class_entry *stringy_ce;

void php_startup_register_stringy();

#endif
