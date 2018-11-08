#include "test.class.h"

zend_class_entry *test_ce;

PHP_METHOD(Test, helloworld)
{
	php_printf("test\n");
}

static zend_function_entry methods[] = {
	PHP_ME(Test, helloworld, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

void php_start_register_test()
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, PHP_STARTUP_NS(Test), methods);

	test_ce = zend_register_internal_class(&ce);

	zend_declare_property_null(test_ce, "pub_var", strlen("pub_var"), ZEND_ACC_PUBLIC TSRMLS_CC);
}
