#include "functions.h"

#define XXX(zend_function_entry, len) {len = sizeof(zend_function_entry) / sizeof(zend_function_entry[0]);}

// void php_startup_register_functions(zend_function_entry *entry)
// {
// }

bool zval_str_equal(zval *first, zval *second)
{
    
    if (Z_TYPE_P(first) == Z_TYPE_P(second) && zend_string_equals(Z_STR_P(first), Z_STR_P(second))) {
        return true;
    }

    return false;
}
