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
    char *encoding;
    zval *str;
    size_t str_len, encoding_len;

    if (EX_NUM_ARGS() == 0) {
        RETURN_NULL();
    }

    ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(str)
    Z_PARAM_STRING(encoding, encoding_len)
    ZEND_PARSE_PARAMETERS_END();

    if (Z_TYPE_P(str) == IS_ARRAY) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "%s", "Passed value cannot be an array");
        return;
    }

    if (Z_TYPE_P(str) == IS_OBJECT) {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "%s", "Passed object must have a __toString method");
        return;
    }

    convert_to_string(str)

    str = Z_STRVAL_P(str);

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

PHP_METHOD(Stringy, create)
{
    zval instance = {};
    zval *retval, *str, *encoding;

    ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(str)
    Z_PARAM_ZVAL(encoding)
    ZEND_PARSE_PARAMETERS_END();

    object_init_ex(&instance, stringy_ce);

    zval func, args[2];
    ZVAL_ZVAL(args, str, 0, 1);
    ZVAL_ZVAL(args, encoding, 0, 1);

    ZVAL_STRING(&func, "__construct");

    call_user_function(EG(function_table), &instance, &func, retval, 2, args);

    RETURN_ZVAL(retval, 0, 1);
}
ZEND_BEGIN_ARG_INFO_EX(arginfo_create, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

static zend_function_entry methods[] = {
    PHP_ME(Stringy, __construct, arginfo_construct, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, __toString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, getEncoding, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, create, arginfo_create, ZEND_ACC_PUBLIC)
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
