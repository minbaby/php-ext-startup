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

	// ZVAL_EMPTY_STRING(tmp);
	// php_var_dump(tmp, 1);

	// php_implode(delim, params, tmp);

	// zend_string_release(delim);

	// php_var_dump(tmp, 1);

	// 似乎发现这里问题，但是问题怎么解决

	zend_string *str = ZSTR_EMPTY_ALLOC();

	zval *tmp;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(params), tmp) {
		zend_string *t = ZSTR_EMPTY_ALLOC();

		zend_string *tmp_str = zval_get_string(tmp);

		t = zend_string_extend(str, ZSTR_LEN(str) + ZSTR_LEN(tmp_str) + 2, 0);
		ZSTR_VAL(t)[ZSTR_LEN(str)+1] = ' ';

		memcpy(ZSTR_VAL(str) + ZSTR_LEN(str) + 2, ZSTR_VAL(tmp_str), ZSTR_LEN(tmp_str));

		str = t;

		zend_string_release(t);
		zend_string_release(tmp_str);
	} ZEND_HASH_FOREACH_END();


	strg = strpprintf(0, "method:%s,args:-=-=-=- @@ %d @@ %s @@\n", method, ZSTR_LEN(str), ZSTR_VAL(str));

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

void php_start_register_test()
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, PHP_STARTUP_NS(Test), methods);

	test_ce = zend_register_internal_class(&ce);

	zend_declare_property_string(test_ce, "publicProperty", strlen("publicProperty"), "hello world +property", ZEND_ACC_PUBLIC);

	zend_declare_property_string(test_ce, "publicPropertyStatic", strlen("publicPropertyStatic"), "hello world +property +static", ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

	zend_declare_class_constant_string(test_ce, "PUBLIC_CONST", strlen("PUBLIC_CONST"), "hello world +const");
}
