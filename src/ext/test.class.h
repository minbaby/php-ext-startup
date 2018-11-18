#ifndef STARTUP_TEST_H
#define STARTUP_TEST_H

#include "php.h"
#include "common.h"
#include "ext/standard/php_standard.h"

#define PHP_STARTUP_TEST_NS(cls) PHP_STARTUP_NS_NAME #cls

extern zend_class_entry *test_ce;

void php_startup_register_test();

#endif
