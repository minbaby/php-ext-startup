/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

// [> $Id$ <]
//
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_types.h"
#include "php_startup.h"
#include "src/ext/test.class.h"
#include "src/ext/stringy/stringy.h"
#include "src/ext/functions.h"
#include "zend_closures.h"

/* If you declare any globals in php_startup.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(startup)
*/

/* True global resources - no need for thread safety here */
static int le_startup;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("startup.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_startup_globals, startup_globals)
    STD_PHP_INI_ENTRY("startup.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_startup_globals, startup_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_startup_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_startup_compiled)
{
    char *arg = NULL;
    size_t arg_len, len;
    zend_string *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "startup", arg);

    RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/* {{{ php_startup_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_startup_init_globals(zend_startup_globals *startup_globals)
{
    startup_globals->global_value = 0;
    startup_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(startup)
{
    /* If you have INI entries, uncomment these lines
    REGISTER_INI_ENTRIES();
    */
    php_startup_register_test();
    php_startup_register_stringy();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(startup)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(startup)
{
#if defined(COMPILE_DL_STARTUP) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(startup)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(startup)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "startup support", "enabled");
    php_info_print_table_row(2, "version", PHP_STARTUP_VERSION);
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */

// zval* check_ext_load(char* name)
// {
//     zval return_value, z_name;
//     ZVAL_STRING(&z_name, name);
//     zval func, args[] = {
//         z_name,
//     };
//     ZVAL_STRING(&func, "extension_loaded");
//     call_user_function(NULL, NULL, &func, &return_value, 1, args);
//     // convert_to_boolean(&return_value);
//     // return zval_is_true(&return_value);
//     return &return_value;
// }

/* {{{ startup_functions[]
 *
 * Every user visible function must have an entry in startup_functions[].
 */
zend_function_entry startup_functions[] = {
    PHP_FE(confirm_startup_compiled,	NULL)		/* For testing, remove later. */
    PHP_FE_END	/* Must be the last line in startup_functions[] */
};
/* }}} */

/* {{{ startup_module_entry
 */
zend_module_entry startup_module_entry = {
    STANDARD_MODULE_HEADER,
    "startup",
    startup_functions,
    PHP_MINIT(startup),
    PHP_MSHUTDOWN(startup),
    PHP_RINIT(startup),		/* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(startup),	/* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(startup),
    PHP_STARTUP_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_STARTUP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(startup)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


// =========================================================
