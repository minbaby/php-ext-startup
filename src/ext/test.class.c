#include "test.cass.h"

zend_class_entry *test_ce


PHP_METHOD(Test, 'helloworld')
{
	return SUCCESS;
}

void php_start_register_test()
{
	zend_class_entry ce;

	zend_function_entry methods[] = {
		ZEND_ME(Test, helloworld, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
		PHP_FE_END
	};

	INIT_CLASS_ENTRY(ce, PHP_STARTUP_NS("test"), methods);
	test_ce = zend_register_internal_class(&ce);
}
