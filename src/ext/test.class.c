#include "test.class.h"

zend_class_entry *test_ce;

PHP_METHOD(Test, helloworld)
{
	php_printf("hello world!");
}

PHP_METHOD(Test, echoHelloWorld)
{
	php_printf("hello world!!");
}

static zend_function_entry methods[] = {
	PHP_ME(Test, helloworld, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Test, echoHelloWorld, NULL, ZEND_ACC_PUBLIC)
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
