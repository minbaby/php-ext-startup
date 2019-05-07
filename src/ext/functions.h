#ifndef STARTUP_FUNCTIONS_H
#define STARTUP_FUNCTIONS_H

#include "stdbool.h"
#include "php.h"
#include "common.h"
#include "ext/standard/php_standard.h"
//#include "log.h"

int strsplit (const char *, char *[], const char *);

bool zval_str_equal(zval *first, zval *second);

zval m_array_get(zval *array, const char *key);

bool m_array_set(zval *array, const char *key, zval *value);
#endif
