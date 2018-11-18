#include "stringy.h"

zend_class_entry *stringy_ce;

PHP_METHOD(Stringy, __toString)
{
    // zval rv;
    // zend_read_property(stringy_ce, getThis(), "str", sizeof("str") - 1, 0, &rv);
    // php_var_dump(&rv, 1);
    // RETURN_STRING(Z_STRVAL_P(rv));
    RETURN_EMPTY_STRING();
}


static zend_function_entry methods[] = {
    PHP_FE_END
};

void php_startup_register_stringy()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, PHP_STARTUP_STRINGY_NS(Stringy), methods);

    stringy_ce = zend_register_internal_class(&ce);

    zend_declare_property_string(&ce, "str", sizeof("str"), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(&ce, "encoding", sizeof("encoding"), "", ZEND_ACC_PROTECTED);
}
