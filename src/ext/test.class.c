#include "test.class.h"

zend_class_entry *test_ce;

PHP_METHOD(Test, helloworld)
{
    if  (zend_parse_parameters_none() ==  FAILURE ) {
        return;
    }
    
    php_printf("hello world!");
    RETVAL_NULL();
}

PHP_METHOD(Test, echoHelloWorld)
{
    if  (zend_parse_parameters_none() ==  FAILURE ) {
        return;
    }

    php_printf("hello world!!");

    RETURN_NULL();
}

PHP_METHOD(Test, __call)
{
    zval *params;
    char *method;
    size_t method_len;
    zend_string *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &method, &method_len, &params) == FAILURE)
    {
        return;
    }

    zend_string *delim = zend_string_init("-", sizeof("-"), 0);

    zval *tmp;
    zval result = {};
    zval delimPtr = {};
    
    ZVAL_STRINGL(&delimPtr, "-", 1);
    ZVAL_EMPTY_STRING(&result);

    // 初学者不知道这里有什么其他实现方式。这里以能用为准

    uint32_t count = zend_array_count(params->value.arr);
    uint32_t index = 0;

    if (count > 0) {
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(params), tmp) {
            concat_function(&result, &result, tmp);

            if (index == count-1) {
                break;
            }

            concat_function(&result, &result, &delimPtr);
            index ++;
        } ZEND_HASH_FOREACH_END();
    }

    strg = strpprintf(0, "method:%s,count:%d,args:%s", method, count, Z_STRVAL_P(&result));

    RETURN_STR(strg);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_call, 0, 0, 2)
    ZEND_ARG_INFO(0, method)
    ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

static zend_function_entry methods[] = {
    PHP_ME(Test, helloworld, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(Test, echoHelloWorld, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Test, __call, arginfo_call, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_startup_register_test()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, PHP_STARTUP_TEST_NS(Test), methods);

    test_ce = zend_register_internal_class(&ce);

    zend_declare_property_string(test_ce, "publicProperty", strlen("publicProperty"), "hello world +property", ZEND_ACC_PUBLIC);

    zend_declare_property_string(test_ce, "publicPropertyStatic", strlen("publicPropertyStatic"), "hello world +property +static", ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

    zend_declare_class_constant_string(test_ce, "PUBLIC_CONST", strlen("PUBLIC_CONST"), "hello world +const");
}
