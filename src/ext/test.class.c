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

	RETVAL_NULL();
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

	php_implode(delim, params, return_value);

	zend_string_release(delim);

	// char *msg;

	// spprintf(&msg, 0, "method:%s,args:%s", method, Z_STRVAL_P(return_value));

	zend_string *str = zval_get_string(return_value);
	strg = strpprintf(0, "method:%s,args:%s", method, str->val);

	if (0 >= strcmp(, "abab-cdcd-efef")) {
		php_error_docref("FUCK THE WORLD", E_ERROR, "eee");
	}

	// PHPWRITE(strcmp(str->val, "abab-cdcd-efef"), 1);

	// zend_string *args = Z_STRVAL_P(return_value);

	// zend_string *newStr = zend_string_extend(strg, args->len, 0);

    /* concatenates "bar" after the newly reallocated large enough "FOO" */
    // memcpy(ZSTR_VAL(args) + ZSTR_LEN(args), ZSTR_VAL(strg), ZSTR_LEN(strg));

	// php_var_dump(return_value, 2);

	PHPWRITE(str->val, str->len);

	RETURN_STR(strg);
	// RETURN_STRING(msg);
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

void php_start_register_test()
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, PHP_STARTUP_NS(Test), methods);

	test_ce = zend_register_internal_class(&ce);

	zend_declare_property_string(test_ce, "publicProperty", strlen("publicProperty"), "hello world +property", ZEND_ACC_PUBLIC);

	zend_declare_property_string(test_ce, "publicPropertyStatic", strlen("publicPropertyStatic"), "hello world +property +static", ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

	zend_declare_class_constant_string(test_ce, "PUBLIC_CONST", strlen("PUBLIC_CONST"), "hello world +const");
}
