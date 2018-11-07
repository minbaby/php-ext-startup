#include "test.class.h"

zend_class_entry *test_ce;


PHP_METHOD(A, helloworld)
{
	php_printf("afdsaf\n");
}



void php_start_register_test()
{
	zend_class_entry ce;

	static zend_function_entry methods[] = {
		PHP_ME(A, helloworld, NULL, ZEND_ACC_PUBLIC)
		PHP_FE_END
	};

	INIT_CLASS_ENTRY(ce, "A", methods);

	test_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(test_ce, "pub_var", strlen("pub_var"), ZEND_ACC_PUBLIC TSRMLS_CC);
}
