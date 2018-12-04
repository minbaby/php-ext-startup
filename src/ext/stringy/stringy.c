#include "stringy.h"
#include "php.h"
#include "../common.h"
#include "ext/standard/php_standard.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/mbstring/mbstring.h"
#include "zend_interfaces.h"
#include "zend_closures.h"
#include "ext/spl/spl_array.h"

zend_class_entry *stringy_ce;

// @TODO: 这个地方似乎用法有问题，全是动态调用本类的方法，这个思路是是面向对象的用法，理论上这个地方应该把公用的地方提取成公共方法。
// @TODO: 先把这个写完，然后去学习一下其他项目，然后再来优化这个地方的代码，先跑起来
///* don't forget to free the zvals */
//zval_ptr_dtor(&retval_ptr);
//zval_dtor(&function_name);

// zval* substr(zval *str, zval *start, zval *length, zval *encoding)
// {
//     zval func, args[4], return_value;
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
    zval rv, *value;
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

    if (EX_NUM_ARGS() == 0)
    {
        RETURN_NULL();
    }

    ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(str)
    Z_PARAM_STRING(encoding, encoding_len)
    ZEND_PARSE_PARAMETERS_END();

    if (Z_TYPE_P(str) == IS_ARRAY)
    {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "%s", "Passed value cannot be an array");
        return;
    }

    if (Z_TYPE_P(str) == IS_OBJECT)
    {
        zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "%s", "Passed object must have a __toString method");
        return;
    }

    convert_to_string(str)

    string = Z_STRVAL_P(str);

    if (string == NULL)
    {
        string = "";
    }

    if (encoding == NULL)
    {
        encoding = "";
    }

    // 这里需要加一下了逻辑， 如果这里需要判断 encoding 是否支持, 不支持话，就用默认的编码
    ZVAL_STRING(&func, "mb_list_encodings");
    call_user_function(NULL, NULL, &func, return_value, 0, NULL);

    zval *tmp;
    size_t count = 0;
    zend_string *encoding_str = zend_string_init(encoding, strlen(encoding), 0);
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(return_value), tmp) {
        if (encoding == Z_STRVAL_P(tmp))
        {
            count++;
            break;
        }
    } ZEND_HASH_FOREACH_END();

    if (count == 0)
    {
        ZVAL_STRING(&func, "mb_internal_encoding");
        call_user_function(NULL, NULL, &func, return_value, 0, NULL);

        encoding = Z_STRVAL_P(return_value);
    }

    // PHPWRITE(encoding, strlen(encoding));
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
    zval instance, *str, *encoding;

    ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(str)
    Z_PARAM_ZVAL(encoding)
    ZEND_PARSE_PARAMETERS_END();

    object_init_ex(&instance, stringy_ce);

    zend_call_method(&instance, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, str, encoding);

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
    zval instance, func, rv;

    ZVAL_STRING(&func, "length");

    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
}

/* {{{ int length(): int
    todo: 这里依赖 mb_string 扩展
}}} */
PHP_METHOD(Stringy, length)
{
    zval func, args[1], rv;
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
    zval func, args[4], rv;
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
    zval func, args[4], rv;
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
    zval instance;
    object_init_ex(&instance, spl_ce_ArrayIterator);

    zval func, ret;
    ZVAL_STRING(&func, "chars");
    call_user_function(NULL, getThis(), &func, &ret, 0, NULL);

    zval zendFunction, flags;

    ZVAL_STRING(&func, "__construct");
    zval args[2];
    ZVAL_ARR(&zendFunction, Z_ARRVAL(ret));
    ZVAL_LONG(&flags, 0);
    args[0] = zendFunction;
    args[1] = flags;
    call_user_function(NULL, &instance, &func, return_value, 2, args);

    RETURN_ZVAL(&instance, 0, 1);
}

PHP_METHOD(Stringy, chars)
{
    size_t str_len;
    zval func;

    ZVAL_STRING(&func, "length");

    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    str_len = (size_t)Z_LVAL_P(return_value);

    zval chars, index, rv, *str;

    array_init(&chars);

    for (int i = 0, l = str_len; i < l; i++)
    {
        ZVAL_LONG(&index, i);

        zval args[1];

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

    zval func, args[2];
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
    zval func;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_ZVAL(start)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(length)
    ZEND_PARSE_PARAMETERS_END();

    if (Z_TYPE_P(length) == IS_NULL)
    {
        zval func;
        ZVAL_STRING(&func, "length");
        call_user_function(NULL, NULL, &func, return_value, 0, NULL);
        length = return_value;
    }

    zval rv;
    zval *string = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 1, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 1, &rv);

    zval args[4];
    ZVAL_STRING(&func, "mb_substr");
    args[0] = *string;
    args[1] = *start;
    args[2] = *length;
    args[3] = *encoding;
    call_user_function(NULL, NULL, &func, return_value, 4, args);

    zval newStr;
    ZVAL_STRING(&newStr, Z_STRVAL_P(return_value));
    zval argsCreate[2];
    ZVAL_STRING(&func, "create");
    argsCreate[0] = newStr;
    argsCreate[1] = *encoding;
    call_user_function(NULL, getThis(), &func, return_value, 2, argsCreate);
}
ZEND_BEGIN_ARG_INFO(arginfo_substr, 2)
ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, offsetExists)
{
    zval *offset;
    int offset_int = 0, length = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(offset)
    ZEND_PARSE_PARAMETERS_END();

    convert_to_long(offset);

    offset_int = Z_LVAL_P(offset);

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    length = Z_LVAL_P(return_value);

    if (offset_int >= 0)
    {
        RETURN_BOOL(length > offset_int);
    }

    RETURN_BOOL(length >= abs(offset_int));
}
ZEND_BEGIN_ARG_INFO(arginfo_offsetExists, 1)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, offsetGet)
{
    zval *offset;
    int offset_int = 0, length = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(offset)
    ZEND_PARSE_PARAMETERS_END();

    convert_to_long(offset);

    offset_int = Z_LVAL_P(offset);

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    length = Z_LVAL_P(return_value);

    if ((offset_int >= 0 && length <= offset_int) || length < abs(offset_int))
    {
        zend_throw_exception(spl_ce_OutOfBoundsException, "No character exists at the index", 0);
        return;
    }

    zval args[4], substrLen, rv;
    ZVAL_LONG(&substrLen, 1);
    ZVAL_STRING(&func, "mb_substr");
    zval *string = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 1, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 1, &rv);
    args[0] = *string;
    args[1] = *offset;
    args[2] = substrLen;
    args[3] = *encoding;
    call_user_function(NULL, NULL, &func, return_value, 4, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_offsetGet, 1)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, offsetSet)
{
    zend_throw_exception_ex(zend_ce_exception, 0, "%s", "Stringy object is immutable, cannot modify char");
}
ZEND_BEGIN_ARG_INFO(arginfo_offsetSet, 2)
ZEND_ARG_INFO(0, offset)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, offsetUnset)
{
    zend_throw_exception_ex(zend_ce_exception, 0, "%s", "Stringy object is immutable, cannot unset char");
}
ZEND_BEGIN_ARG_INFO(arginfo_offsetUnset, 1)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, collapseWhiteSpace)
{
    zval retval, func, args[3], args_trim[1];
    zval pattern, replacement, options, chars;

    ZVAL_STRING(&pattern, "[[:space:]]+");
    ZVAL_STRING(&replacement, " ");

    args[0] = pattern;
    args[1] = replacement;
    args[2] = options;

    ZVAL_STRING(&func, "regexReplace");
    call_user_function(NULL, getThis(), &func, &retval, 3, args);

    // call trim()

    ZVAL_STRING(&func, "trim");
    args_trim[0] = retval;
    call_user_function(NULL, &retval, &func, return_value, 1, args_trim);

    RETURN_ZVAL(&retval, 0, 1);
}

PHP_METHOD(Stringy, regexReplace)
{
    zval *pattern, *replacement, *options, rv;
    zval func;

    ZEND_PARSE_PARAMETERS_START(2, 3)
    Z_PARAM_ZVAL(pattern)
    Z_PARAM_ZVAL(replacement)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(options)
    ZEND_PARSE_PARAMETERS_END();

    zval *encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 1, &rv);

    zval regexEncoding;
    ZVAL_STRING(&func, "regexEncoding");
    call_user_function(NULL, getThis(), &func, &regexEncoding, 0, NULL);

    zval args[1];
    ZVAL_STRING(&func, "regexEncoding");
    args[0] = *encoding;
    call_user_function(NULL, getThis(), &func, return_value, 1, args);

    zval args_eregReplace[4], retStr;
    ZVAL_STRING(&func, "eregReplace");
    args_eregReplace[0] = *pattern;
    args_eregReplace[1] = *replacement;
    args_eregReplace[2] = *str;
    args_eregReplace[3] = *options;
    call_user_function(NULL, getThis(), &func, &retStr, 4, args_eregReplace);

    ZVAL_STRING(&func, "regexEncoding");
    args[0] = regexEncoding;
    call_user_function(NULL, getThis(), &func, return_value, 1, args);

    RETURN_ZVAL(getThis(), 1, 0);
}
ZEND_BEGIN_ARG_INFO(arginfo_regexReplace, 3)
ZEND_ARG_INFO(0, pattern)
ZEND_ARG_INFO(0, replacement)
ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, trim)
{
    zval *chars;
    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_ZVAL(chars)
    ZEND_PARSE_PARAMETERS_END();

    if (Z_TYPE_P(chars) != IS_NULL)
    {
        zval func, args[2], delimiter;
        ZVAL_STRING(&func, "preg_quote");
        args[0] = *chars;
        args[1] = delimiter;
        call_user_function(NULL, NULL, &func, return_value, 2, args);
    }
    else
    {
        ZVAL_STRING(chars, "[:space:]");
    }

    zval func_regexReplace, args[3], pattern, replacement, options;
    ZVAL_STRING(&func_regexReplace, "regexReplace");
    ZVAL_STRING(&pattern, "^[$chars]+|[$chars]+$");
    ZVAL_STRING(&replacement, "");
    args[0] = pattern;
    args[1] = replacement;
    args[2] = options;
    call_user_function(NULL, getThis(), &func_regexReplace, return_value, 3, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_trim, 1)
ZEND_ARG_INFO(0, chars)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, regexEncoding)
{
    zval *encoding;
    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(encoding)
    ZEND_PARSE_PARAMETERS_END();

    zval func, args[1];
    ZVAL_STRING(&func, "mb_regex_encoding");
    args[0] = *encoding;
    call_user_function(NULL, NULL, &func, return_value, 0, NULL);
}
ZEND_BEGIN_ARG_INFO(arginfo_regexEncoding, 1)
ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, eregReplace)
{
    zval *pattern, *replace, *string, *option;
    ZEND_PARSE_PARAMETERS_START(3, 4)
    Z_PARAM_ZVAL(pattern)
    Z_PARAM_ZVAL(replace)
    Z_PARAM_ZVAL(string)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(option)
    ZEND_PARSE_PARAMETERS_END();

    convert_to_string(replace);
    convert_to_string(pattern);
    convert_to_string(string);
    convert_to_string(option);

    zval func, args[4];
    ZVAL_STRING(&func, "mb_ereg_replace");
    args[0] = *pattern;
    args[1] = *replace;
    args[2] = *string;
    args[3] = *option;
    call_user_function(NULL, NULL, &func, return_value, 4, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_eregReplace, 4)
ZEND_ARG_INFO(0, pattern)
ZEND_ARG_INFO(0, replacement)
ZEND_ARG_INFO(0, string)
ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO();

static void once_listener_handler(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *arr;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(arr)
    ZEND_PARSE_PARAMETERS_END();

    zval *ret = zend_hash_index_find(Z_ARRVAL_P(arr), 0);

    if (ret == NULL) {
        RETURN_EMPTY_STRING();
    }

    zval func = {};
    ZVAL_STRING(&func, "mb_strtoupper");
    zval args[] = {
        *ret,
    };
    call_user_function(NULL, NULL, &func, return_value, 1, args);

    if (zend_string_equals(Z_STR_P(return_value), Z_STR_P(ret)) == 1) {
        ZVAL_STRING(&func, "mb_strtolower");
        zval args[] = {
            *ret,
        };
        call_user_function(NULL, NULL, &func, return_value, 1, args);
    }

    RETURN_ZVAL(return_value, 0, 1);
}

PHP_METHOD(Stringy, swapCase)
{
    zval rv, func, pattern={}, subject, limit_str, count, ret, encoding;
    
    ZVAL_STRING(&pattern, "/[\\S]/u");
    
    subject = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    zend_function zendFunction;

    zend_internal_arg_info zai[] = {
        ZEND_ARG_ARRAY_INFO(0, "matches", 0)
    };

    zend_string *f = zend_string_init(ZEND_STRL("callback"), 0);

    zendFunction.type = ZEND_INTERNAL_FUNCTION;
    zendFunction.common.num_args = 1;
    zendFunction.common.required_num_args = 1;
    zendFunction.common.arg_info = zai;
    zendFunction.common.prototype = NULL;
    zendFunction.common.scope = NULL;
    zendFunction.common.fn_flags = ZEND_ACC_CLOSURE;
    zendFunction.common.function_name = f;
    zendFunction.internal_function.handler = once_listener_handler;
    zendFunction.internal_function.type = ZEND_INTERNAL_FUNCTION;
    zendFunction.internal_function.fn_flags = ZEND_ACC_CLOSURE;
    zendFunction.internal_function.arg_info = zai;
    zendFunction.internal_function.required_num_args =1;
    zendFunction.internal_function.num_args = 1;
    
    zval callback;
    zend_create_closure(&callback, &zendFunction, NULL, NULL, NULL);

    zval arr;
    array_init(&arr);
    add_assoc_string(&arr, "1", "b");
    zval args1[] = {
        arr
    };

    call_user_function(NULL, NULL, &callback, return_value, 1, args1);


#if ZEND_DEBUG
        ZEND_ASSERT(1);
#endif

    ZVAL_MAKE_REF(&count);
    ZVAL_STRING(&func, "preg_replace_callback");
    ZVAL_LONG(&limit_str, -1);
    
    zval args[] ={
        pattern,
        callback,
        subject,
        limit_str,
        count,
    };

    call_user_function(NULL, NULL, &func, &ret, 5, args);

    convert_to_string(&ret);
    zend_update_property_string(stringy_ce, getThis(), ZEND_STRL("str"), Z_STRVAL(ret));

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Stringy, upperCaseFirst)
{
    zval instance, encoding, str, func, ret, start, len, first, rest, rv;

    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    size_t length = Z_LVAL_P(return_value);

    str = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    encoding = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    ZVAL_LONG(&start, 0);
    ZVAL_LONG(&len, 1);
    zval args[] = {
        str,
        start,
        len,
        encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &first, 4, args);

    ZVAL_LONG(&start, 1);
    ZVAL_LONG(&len, length-1);
    zval args_rest[] = {
        str,
        start,
        len,
        encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &rest, 4, args_rest);

    zval args_upper[] = {
        first,
        encoding,
    };
    ZVAL_STRING(&func, "mb_strtoupper");
    call_user_function(NULL, NULL, &func, &first, 2, args_upper);

    concat_function(&ret, &first,  &rest);

    zend_string *str_string = Z_STR(ret);
    zend_update_property_str(stringy_ce, getThis(), ZEND_STRL("str"), str_string);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Stringy, lowerCaseFirst)
{
    zval instance, encoding, str, func, ret, start, len, first, rest, rv;

    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    size_t length = Z_LVAL_P(return_value);

    str = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    encoding = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    ZVAL_LONG(&start, 0);
    ZVAL_LONG(&len, 1);
    zval args[] = {
        str,
        start,
        len,
        encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &first, 4, args);

    ZVAL_LONG(&start, 1);
    ZVAL_LONG(&len, length-1);
    zval args_rest[] = {
        str,
        start,
        len,
        encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &rest, 4, args_rest);

    zval args_upper[] = {
        first,
        encoding,
    };
    ZVAL_STRING(&func, "mb_strtolower");
    call_user_function(NULL, NULL, &func, &first, 2, args_upper);

    concat_function(&ret, &first,  &rest);

    zval this;
    object_init_ex(&this, stringy_ce);
    zend_call_method(&this, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, &ret, &encoding);

    RETURN_ZVAL(&this, 0, 1);
}

PHP_METHOD(Stringy, append)
{
    zval *str_param;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(str_param)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 1, &rv);

    concat_function(return_value, str, str_param);

    zend_update_property_string(stringy_ce, getThis(), ZEND_STRL("str"), Z_STRVAL_P(return_value));

    RETURN_ZVAL(getThis(), 1, 0);
}
ZEND_BEGIN_ARG_INFO(arginfo_append, 1)
    ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, prepend)
{
    zval *str_param;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(str_param)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 1, &rv);

    concat_function(return_value, str_param, str);

    zend_update_property_string(stringy_ce, getThis(), ZEND_STRL("str"), Z_STRVAL_P(return_value));

    RETURN_ZVAL(getThis(), 1, 0);
}
ZEND_BEGIN_ARG_INFO(arginfo_prepend, 1)
    ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, lines)
{
    zval array, pattern, *str, rv;

    ZVAL_STRING(&pattern, "[\r\n]{1,2}");
    str =zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 1, &rv);

    array_init(&array);

    zval func, args[] = {
        pattern,
        *str,
    };
    ZVAL_STRING(&func, "split");
    call_user_function(NULL, getThis(), &func, &array, 2, args); 

    zval ret_arr;
    array_init(&ret_arr);

    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval *tmp;
    
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(array), tmp){
        zval instance;
        object_init_ex(&instance, stringy_ce);
        convert_to_string(tmp);
        zend_call_method(&instance, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, tmp, encoding);
        add_next_index_zval(&ret_arr, &instance);
    }ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(&ret_arr, 0, 1);   
}

PHP_METHOD(Stringy, split)
{
    zend_string *empty = zend_string_init(ZEND_STRL(""), 0);
    zend_string *pattern;
    zval *limit_zval;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(pattern)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(limit_zval)
    ZEND_PARSE_PARAMETERS_END();

    zval arr;
    array_init(&arr);

    if (Z_TYPE_P(limit_zval) == IS_LONG && Z_LVAL_P(limit_zval) == 0) {
        RETURN_ZVAL(&arr, 1, 0);
    }
    
    if (zend_string_equals(pattern, empty)) {
        zval *this;
        ZVAL_COPY(this, getThis());
        add_index_zval(&arr, 0, this);
        RETURN_ZVAL(&arr, 1, 0);
    }

    convert_to_long(limit_zval);
    zend_long limit_long = Z_LVAL_P(limit_zval);

    zend_long limit = (limit_long > 0) ? limit_long += 1 : -1;

    zval pattern_zval, str_zval, rv;

    str_zval = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    ZVAL_STR(&pattern_zval, pattern);

    ZVAL_LONG(limit_zval, limit);
    zval func, args[] = {
        pattern_zval,
        str_zval,
        *limit_zval,
    };
    ZVAL_STRING(&func, "mb_split");
    call_user_function(NULL, NULL, &func, &arr, 3, args);

    zend_long array_len = zend_array_count(Z_ARRVAL(arr));

    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval *tmp;
    zend_long index;
    zval ret_arr;
    array_init(&ret_arr);
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(arr), tmp){
        zval instance;
        object_init_ex(&instance, stringy_ce);
        zend_call_method(&instance, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, tmp, encoding);
        add_next_index_zval(&ret_arr, &instance);
    }ZEND_HASH_FOREACH_END();


    RETURN_ZVAL(&ret_arr, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_split, 2)
    ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
    ZEND_ARG_INFO(0, limit_str)
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
    PHP_ME(Stringy, offsetExists, arginfo_offsetExists, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, offsetGet, arginfo_offsetGet, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, offsetSet, arginfo_offsetSet, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, offsetUnset, arginfo_offsetUnset, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, collapseWhiteSpace, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, regexReplace, arginfo_regexReplace, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, regexEncoding, arginfo_regexEncoding, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, eregReplace, arginfo_eregReplace, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, swapCase, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, upperCaseFirst, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, lowerCaseFirst, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, append, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, prepend, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, lines, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, split, arginfo_split, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_startup_register_stringy()
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, PHP_STARTUP_STRINGY_NS(Stringy), methods);

    stringy_ce = zend_register_internal_class(&ce);

    zend_declare_property_string(stringy_ce, ZEND_STRL("str"), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(stringy_ce, ZEND_STRL("str"), "", ZEND_ACC_PROTECTED);

    zend_class_implements(
        stringy_ce,
        3,
        spl_ce_Countable,
        spl_ce_Aggregate,
        spl_ce_ArrayAccess);
}
