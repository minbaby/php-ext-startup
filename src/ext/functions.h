#ifndef STARTUP_FUNCTIONS_H
#define STARTUP_FUNCTIONS_H

#include "stdbool.h"
#include "php.h"
#include "common.h"
#include "ext/standard/php_standard.h"

// void php_startup_register_functions(zend_function_entry *zend_function_entry);
bool zval_equal(zval *first, zval *second);
#endif
