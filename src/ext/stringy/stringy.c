#include "stringy.h"
#include "php.h"
#include "../common.h"
#include "ext/standard/php_standard.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/mbstring/mbstring.h"
#include "zend_interfaces.h"
#include "ext/spl/spl_array.h"

zend_class_entry *stringy_ce;

// @TODO: 这个地方似乎用法有问题，全是动态调用本类的方法，这个思路是是面向对象的用法，理论上这个地方应该把公用的地方提取成公共方法。
// @TODO: 先把这个写完，然后去学习一下其他项目，然后再来优化这个地方的代码，先跑起来

// zval* substr(zval *str, zval *start, zval *length, zval *encoding)
// {
//     zval func = {}, args[4] = {}, return_value = {};
//     ZVAL_STRING(&func, "mb_substr");
//     args[0] = *str;
//     args[1] = *start;
//     args[2] = *length;
//     args[3] = *encoding;
//     call_user_function(NULL, NULL, &func, &return_value, 4, args);

//     return &return_value;
// }

//=================================================

PHP_METHOD(Stringy, __toString)
{
    zval rv = {}, *value;
    value = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 0, &rv);
    RETURN_STRING(Z_STRVAL_P(value));
}

PHP_METHOD(Stringy, __construct)
{
    char *encoding;
    zval *str;
    char *string;
    size_t str_len, encoding_len;
    zval func;

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

    string = Z_STRVAL_P(str);

    if (string == NULL) {
        string = "";
    }

    if (encoding == NULL) {
        encoding = "";
    }

    // 这里需要加一下了逻辑， 如果这里需要判断 encoding 是否支持, 不支持话，就用默认的编码
    ZVAL_STRING(&func, "mb_list_encodings");  
    call_user_function(NULL, NULL, &func, return_value, 0, NULL);

    zval *tmp;
    size_t count = 0;
    zend_string *encoding_str = zend_string_init(encoding, strlen(encoding), 0);
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(return_value), tmp) {
        if (encoding == Z_STRVAL_P(tmp)) {
            count++;
            break;
        }
    }ZEND_HASH_FOREACH_END();

    if (count == 0) {
        ZVAL_STRING(&func, "mb_internal_encoding");  
        call_user_function(NULL, NULL, &func, return_value, 0, NULL);

        encoding = Z_STRVAL_P(return_value);
    } 

    zend_update_property_string(stringy_ce, getThis(), "str", strlen("str"), string);
    zend_update_property_string(stringy_ce, getThis(), "encoding", strlen("encoding"), encoding);
}
ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, getEncoding)
{
    zval rv, *value;
    value = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 0, &rv);
    RETURN_STRING(Z_STRVAL_P(value));
}

PHP_METHOD(Stringy, create)
{
    zval instance = {}, retval = {}, func = {}, args[2] = {};
    zval *str, *encoding;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(str)
        Z_PARAM_ZVAL(encoding)
    ZEND_PARSE_PARAMETERS_END();

    object_init_ex(&instance, stringy_ce);
    
    args[0] = *str;
    args[1] = *encoding;

    ZVAL_STRING(&func, "__construct");

    call_user_function(NULL, &instance, &func, &retval, 2, args);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO_EX(arginfo_create, 0, 0, 2)
    ZEND_ARG_INFO(0, str)
    ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

/* {{{ int length(): int
}}} */
PHP_METHOD(Stringy, count)
{
    zval instance = {}, func = {}, rv = {};

    ZVAL_STRING(&func, "length");

    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
}

/* {{{ int length(): int
    todo: 这里依赖 mb_string 扩展
}}} */
PHP_METHOD(Stringy, length)
{
    zval func = {}, args[1] = {}, rv = {};
    zval *value;
    ZVAL_STRING(&func, "mb_strlen");

    value = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 0, &rv);

    args[0] = *value;

    call_user_function(NULL, NULL, &func, return_value, 1, args);
}

/*{{{ int indexOfLast($needle, $offset = 0)
}}}*/
PHP_METHOD(Stringy, indexOfLast)
{
    zval func = {}, args[4] = {}, rv = {};
    zval *value, *encoding, *needle, *offset;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(needle)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(offset)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_STRING(&func, "mb_strrpos");

    value = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 0, &rv);
    encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 0, &rv);

    args[0] = *value;
    args[1] = *needle;
    args[2] = *offset;
    args[3] = *encoding;

    call_user_function_ex(NULL, NULL, &func, return_value, 4, args, 0, NULL);
}
ZEND_BEGIN_ARG_INFO(args_indexOfLast, 2)
    ZEND_ARG_INFO(0, needle)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

/*{{{ int indexOf($needle, $offset = 0)
    TODO: 这个和上个重复率太高，暂时不知道怎么优化
}}}*/
PHP_METHOD(Stringy, indexOf)
{
    zval func = {}, args[4] = {}, rv = {};
    zval *value, *encoding, *needle, *offset;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(needle)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(offset)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_STRING(&func, "mb_strpos");

    value = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 0, &rv);
    encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 0, &rv);

    args[0] = *value;
    args[1] = *needle;
    args[2] = *offset;
    args[3] = *encoding;

    call_user_function_ex(NULL, NULL, &func, return_value, 4, args, 0, NULL);
}
ZEND_BEGIN_ARG_INFO(args_indexOf, 2)
    ZEND_ARG_INFO(0, needle)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

PHP_METHOD(Stringy, getIterator)
{
    zval instance = {};
    object_init_ex(&instance, spl_ce_ArrayIterator);

    zval func = {}, ret = {};
    ZVAL_STRING(&func, "chars");
    call_user_function(NULL, getThis(), &func, &ret, 0, NULL);

    zval x = {}, flags = {};
    
    ZVAL_STRING(&func, "__construct");
    zval args[2] = {};
    ZVAL_ARR(&x, Z_ARRVAL(ret));
    ZVAL_LONG(&flags, 0);
    args[0] = x;
    args[1] = flags;
    call_user_function(NULL, &instance, &func, return_value, 2, args);
    
    RETURN_ZVAL(&instance, 0, 1);
}

PHP_METHOD(Stringy, chars)
{
    size_t str_len;
    zval func = {};

    ZVAL_STRING(&func, "length");

    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    str_len = (size_t) Z_LVAL_P(return_value);
    
    zval chars = {}, index = {}, rv = {}, *str;

    array_init(&chars);
    
    for(int i = 0, l = str_len; i < l; i++)
    {
        ZVAL_LONG(&index, i);

        zval args[1] = {};

        ZVAL_STRING(&func, "at");

        args[0] = index;

        call_user_function(NULL, getThis(), &func, return_value, 1, args);

        str = zend_read_property(stringy_ce, return_value, "str", strlen("str"), 1, &rv);

        add_next_index_zval(&chars, str);
    }

    RETURN_ARR(Z_ARRVAL(chars));
}

PHP_METHOD(Stringy, at)
{
    zval *start, length;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(start)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_LONG(&length, 1);

    zval func = {}, args[2] = {};
    ZVAL_STRING(&func, "substr");
    args[0] = *start;
    args[1] = length;
    call_user_function(NULL, getThis(), &func, return_value, 2, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_at, 1)
    ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, substr)
{
    zval *start, *length;
    zval func = {};

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(start)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(length)
    ZEND_PARSE_PARAMETERS_END();

    if (Z_TYPE_P(length) == IS_NULL) {
        zval func = {};
        ZVAL_STRING(&func, "length");
        call_user_function(NULL, NULL, &func, return_value, 0, NULL);
        length = return_value;
    }

    zval rv = {};
    zval *string = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 1, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 1, &rv);

    zval args[4] = {};
    ZVAL_STRING(&func, "mb_substr");
    args[0] = *string;
    args[1] = *start;
    args[2] = *length;
    args[3] = *encoding;
    call_user_function(NULL, NULL, &func, return_value, 4, args);

    zval newStr = {};
    ZVAL_STRING(&newStr, Z_STRVAL_P(return_value));
    zval argsCreate[2] = {};
    ZVAL_STRING(&func, "create");
    argsCreate[0] = newStr;
    argsCreate[1] = *encoding;
    call_user_function(NULL, getThis(), &func, return_value, 2, argsCreate);
}
ZEND_BEGIN_ARG_INFO(arginfo_substr, 2)
    ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO();

static zend_function_entry methods[] = {
    PHP_ME(Stringy, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Stringy, __toString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, getEncoding, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, create, arginfo_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Stringy, count, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, length, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, indexOfLast, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, indexOf, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, getIterator, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, chars, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, at, arginfo_at, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, substr, arginfo_substr, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_startup_register_stringy()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, PHP_STARTUP_STRINGY_NS(Stringy), methods);

    stringy_ce = zend_register_internal_class(&ce);

    zend_declare_property_string(stringy_ce, "str", strlen("str"), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(stringy_ce, "encoding", strlen("encoding"), "", ZEND_ACC_PROTECTED);
    
    zend_class_implements(
        stringy_ce, 
        2, 
        spl_ce_Countable, 
        spl_ce_Aggregate
    );
}
