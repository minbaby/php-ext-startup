#ifndef STARTUP_TEST_H
#define STARTUP_TEST_H

#include "php.h"
#include "common.h"

extern zend_class_entry *test_ce;

void php_start_register_test();

#endif
