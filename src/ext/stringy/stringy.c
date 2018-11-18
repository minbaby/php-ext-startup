#include "stringy.h"

zend_class_entry *stringy_ce;

PHP_METHOD(Stringy, __toString)
{
    zval rv, *value;
    value = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 0, &rv);
    RETURN_STRING(Z_STRVAL_P(value));
}

PHP_METHOD(Stringy, __construct)
{
    char *str, *encoding;
    size_t str_len, encoding_len;

    if (EX_NUM_ARGS() == 0) {
        RETURN_NULL();
    }

     ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(str, str_len)
        Z_PARAM_STRING(encoding, encoding_len)
     ZEND_PARSE_PARAMETERS_END();

    if (str == NULL) {
        str = "";
    }

    if (encoding == NULL) {
        encoding = "";
    }

    zend_update_property_string(stringy_ce, getThis(), "str", strlen("str"), str);
    zend_update_property_string(stringy_ce, getThis(), "encoding", strlen("encoding"), encoding);
}
ZEND_BEGIN_ARG_INFO_EX(arginfo_construct, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

PHP_METHOD(Stringy, getEncoding)
{
    zval rv, *value;
    value = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 0, &rv);
    RETURN_STRING(Z_STRVAL_P(value));
}


static zend_function_entry methods[] = {
    PHP_ME(Stringy, __construct, arginfo_construct, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, __toString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, getEncoding, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_startup_register_stringy()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, PHP_STARTUP_STRINGY_NS(Stringy), methods);

    stringy_ce = zend_register_internal_class(&ce);

    zend_declare_property_string(stringy_ce, "str", strlen("str"), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(stringy_ce, "encoding", strlen("encoding"), "", ZEND_ACC_PROTECTED);
}
