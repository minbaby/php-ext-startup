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
    zval rv;
    zval *value = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    RETURN_ZVAL(value, 1, 0);
}

PHP_METHOD(Stringy, __construct)
{
    char *encoding = NULL;
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

    RETURN_ZVAL(&instance, 1, 0);
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

    RETURN_ZVAL(&instance, 1, 0);
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

    ZVAL_STRING(&func, "trim");
    call_user_function(NULL, &retval, &func, return_value, 0, NULL);
}

PHP_METHOD(Stringy, regexReplace)
{
    zval *pattern, *replacement, *options = NULL, rv;
    zval func;

    ZEND_PARSE_PARAMETERS_START(2, 3)
    Z_PARAM_ZVAL(pattern)
    Z_PARAM_ZVAL(replacement)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(options)
    ZEND_PARSE_PARAMETERS_END();

    if (options == NULL) {
        options = malloc(sizeof(zval));
        ZVAL_STRING(options, "msr");
    }

    zval *encoding = zend_read_property(stringy_ce, getThis(), "encoding", strlen("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), "str", strlen("str"), 1, &rv);

    zval regexEncoding;
    ZVAL_STRING(&func, "regexEncoding");
    call_user_function(NULL, getThis(), &func, &regexEncoding, 0, NULL);

    zval args[1];
    ZVAL_STRING(&func, "regexEncoding");
    args[0] = *encoding;
    call_user_function(NULL, getThis(), &func, return_value, 1, args);

    zval retStr;
    ZVAL_STRING(&func, "eregReplace");

    zval args_eregReplace[] = {
        *pattern,
        *replacement,
        *str,
        *options,
    };
    call_user_function(NULL, getThis(), &func, &retStr, 4, args_eregReplace);

    ZVAL_STRING(&func, "regexEncoding");
    args[0] = regexEncoding;
    call_user_function(NULL, getThis(), &func, return_value, 1, args);

    zval this;
    object_init_ex(&this, stringy_ce);
    zend_call_method(&this, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, &retStr, encoding);

    RETURN_ZVAL(&this, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_regexReplace, 3)
ZEND_ARG_INFO(0, pattern)
ZEND_ARG_INFO(0, replacement)
ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, trim)
{
    zval *chars = NULL;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(chars)
    ZEND_PARSE_PARAMETERS_END();

    if (chars != NULL && Z_TYPE_P(chars) != IS_NULL)
    {
        convert_to_string(chars);
        if (Z_STRLEN_P(chars) == 0) {
            chars = malloc(sizeof(zval));
            ZVAL_STRING(chars, "[:space:]");
        } else {
            zval func, delimiter;
            ZVAL_STRING(&func, "preg_quote");
            zval args[] =  {
                *chars,
            };
            call_user_function(NULL, NULL, &func, return_value, 1, args);
            ZVAL_STRING(chars, Z_STRVAL_P(return_value));
        }
    }
    else
    {
        chars = malloc(sizeof(zval));
        ZVAL_STRING(chars, "[:space:]");

    }

    zval func_regexReplace, pattern, replacement, options;
    ZVAL_STRING(&func_regexReplace, "regexReplace");
    zend_string *p = strpprintf(0, "^[%s]+|[%s]+$", Z_STRVAL_P(chars), Z_STRVAL_P(chars));
    ZVAL_STR(&pattern, p);
    ZVAL_STRING(&replacement, "");
    zval args[] = {
        pattern,
        replacement,
    };
    call_user_function(NULL, getThis(), &func_regexReplace, return_value, 2, args);
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
    zval *pattern, *replace, *string, *option = NULL;
    ZEND_PARSE_PARAMETERS_START(3, 4)
    Z_PARAM_ZVAL(pattern)
    Z_PARAM_ZVAL(replace)
    Z_PARAM_ZVAL(string)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(option)
    ZEND_PARSE_PARAMETERS_END();

    zval func;
    ZVAL_STRING(&func, "mb_ereg_replace");

    if (option == NULL) {
        option = malloc(sizeof(zval));
        ZVAL_EMPTY_STRING(option);
    }

    convert_to_string(replace);
    convert_to_string(pattern);
    convert_to_string(string);
    convert_to_string(option);

    zval args[] = {
        *pattern,
        *replace,
        *string,
        *option,
    };
    call_user_function(NULL, NULL, &func, return_value, 4, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_eregReplace, 4)
ZEND_ARG_INFO(0, pattern)
ZEND_ARG_INFO(0, replacement)
ZEND_ARG_INFO(0, string)
ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO();

static void swap_case_handler(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *arr;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(arr)
    ZEND_PARSE_PARAMETERS_END();

    zval *first = zend_hash_index_find(Z_ARRVAL_P(arr), 0);

    if (first == NULL) {
        RETURN_EMPTY_STRING();
    }

    zval func, ret;
    ZVAL_STRING(&func, "mb_strtoupper");
    zval args[] = {
        *first,
    };
    call_user_function(NULL, NULL, &func, &ret, 1, args);

    if (zend_string_equals(Z_STR_P(&ret), Z_STR_P(first)) == 1) {
        ZVAL_STRING(&func, "mb_strtolower");
        zval args[] = {
            *first,
        };
        call_user_function(NULL, NULL, &func, &ret, 1, args);
    }

    RETURN_ZVAL(&ret, 0, 1);
}

PHP_METHOD(Stringy, swapCase)
{
    zval rv, func, pattern={}, subject, limit_str, count, ret;
    
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
    zendFunction.internal_function.handler = swap_case_handler;
    zendFunction.internal_function.type = ZEND_INTERNAL_FUNCTION;
    zendFunction.internal_function.fn_flags = ZEND_ACC_CLOSURE;
    zendFunction.internal_function.arg_info = zai;
    zendFunction.internal_function.required_num_args =1;
    zendFunction.internal_function.num_args = 1;
    
    zval callback;
    zend_create_closure(&callback, &zendFunction, NULL, NULL, NULL);

    ZVAL_STRING(&func, "preg_replace_callback");
    zval args[] ={
        pattern,
        callback,
        subject,
    };
    call_user_function(NULL, NULL, &func, &ret, 3, args);

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

    RETURN_ZVAL(&this, 1, 0);
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

    zval pattern_zval, str_zval, rv;

    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    str_zval = *zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    zval arr;
    array_init(&arr);

    if (Z_TYPE_P(limit_zval) == IS_LONG && Z_LVAL_P(limit_zval) == 0) {
        RETURN_ZVAL(&arr, 0, 1);
    }
    
    if (zend_string_equals(pattern, empty)) {
        zval instance;
        zval tmp;
        ZVAL_EMPTY_STRING(&tmp);

        object_init_ex(&instance, stringy_ce);
        zend_call_method(&instance, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, &str_zval, encoding);

        add_index_zval(&arr, 0, &instance);
        RETURN_ZVAL(&arr, 1, 0);
    }

    convert_to_long(limit_zval);
    zend_long limit_long = Z_LVAL_P(limit_zval);

    zend_long limit = (limit_long > 0) ? limit_long += 1 : -1;

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

static void preg_replace_callback_handler(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *arr = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(arr)
    ZEND_PARSE_PARAMETERS_END();

    if (arr == NULL || zend_array_count(Z_ARRVAL_P(arr)) <= 1) {
        RETURN_EMPTY_STRING();
    } 

    zval *first = zend_hash_index_find(Z_ARRVAL_P(arr), 1);
    // php_var_dump(arr, 1);
    // php_var_dump(first, 1);
    //TODO: 不知道这里如何模拟 php 里边的use， 这里先写死了
    zval encoding;
    ZVAL_STRING(&encoding, "UTF-8");
    zval func, args[] = {
        *first,
        encoding,
    };
    ZVAL_STRING(&func, "mb_strtoupper");
    call_user_function(NULL, NULL, &func, return_value, 2, args);
}


static void preg_replace_callback_2_handler(INTERNAL_FUNCTION_PARAMETERS)
{
    zval *arr = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(arr)
    ZEND_PARSE_PARAMETERS_END();

    if (arr == NULL || zend_array_count(Z_ARRVAL_P(arr)) < 1) {
        RETURN_EMPTY_STRING();
    } 

    zval *first = zend_hash_index_find(Z_ARRVAL_P(arr), 0);

    //TODO: 不知道这里如何模拟 php 里边的use， 这里先写死了
    zval encoding;
    ZVAL_STRING(&encoding, "UTF-8");
    zval func, args[] = {
        *first,
        encoding,
    };
    ZVAL_STRING(&func, "mb_strtoupper");
    call_user_function(NULL, NULL, &func, return_value, 2, args);
}

PHP_METHOD(Stringy, camelize)
{
    zval rv, func;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 1, &rv);

    zval instance;

    ZVAL_STRING(&func, "trim");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    ZVAL_STRING(&func, "lowerCaseFirst");
    call_user_function(NULL, return_value, &func, &instance, 0, NULL);

    zval *str_zval = zend_read_property(stringy_ce, &instance, ZEND_STRL("str"), 1, &rv);
    
    zval pattern, replacement;
    ZVAL_STRING(&pattern, "/^[-_]+/");
    ZVAL_EMPTY_STRING(&replacement);
    zval args[] = {
        pattern,
        replacement,
        *str_zval,
    };
    ZVAL_STRING(&func, "preg_replace");
    call_user_function(NULL, NULL, &func, return_value, 3, args);
    zend_update_property(stringy_ce, &instance, ZEND_STRL("str"), return_value);

    zend_function zendFunction;

    zend_internal_arg_info zai[] = {
        ZEND_ARG_ARRAY_INFO(0, "matches", 0)
    };
#pragma region first
    zend_string *f = zend_string_init(ZEND_STRL("callback"), 0);

    zendFunction.type = ZEND_INTERNAL_FUNCTION;
    zendFunction.common.num_args = 1;
    zendFunction.common.required_num_args = 1;
    zendFunction.common.arg_info = zai;
    zendFunction.common.prototype = NULL;
    zendFunction.common.scope = NULL;
    zendFunction.common.fn_flags = ZEND_ACC_CLOSURE;
    zendFunction.common.function_name = f;
    zendFunction.internal_function.handler = preg_replace_callback_handler;
    zendFunction.internal_function.type = ZEND_INTERNAL_FUNCTION;
    zendFunction.internal_function.fn_flags = ZEND_ACC_CLOSURE;
    zendFunction.internal_function.arg_info = zai;
    zendFunction.internal_function.required_num_args =1;
    zendFunction.internal_function.num_args = 1;
    
    zval callback;
    zend_create_closure(&callback, &zendFunction, NULL, NULL, NULL);

    str_zval = zend_read_property(stringy_ce, &instance, ZEND_STRL("str"), 1, &rv);
    ZVAL_STRING(&pattern, "/[-_\\s]+(.)?/u");
    ZVAL_EMPTY_STRING(&replacement);
    zval args_callback[] = {
        pattern,
        callback,
        *str_zval,
    };
    ZVAL_STRING(&func, "preg_replace_callback");
    call_user_function(NULL, NULL, &func, return_value, 3, args_callback);
#pragma endregion

    zend_update_property(stringy_ce, &instance, ZEND_STRL("str"),  return_value);

#pragma region second

    zendFunction.internal_function.handler = preg_replace_callback_2_handler;
    
    zend_create_closure(&callback, &zendFunction, NULL, NULL, NULL);

    str_zval = zend_read_property(stringy_ce, &instance, ZEND_STRL("str"), 1, &rv);
    ZVAL_STRING(&pattern, "/[\\d]+(.)?/u");
    ZVAL_EMPTY_STRING(&replacement);
    args_callback[0] = pattern;
    args_callback[1] = callback;
    args_callback[2] = *str_zval;

    ZVAL_STRING(&func, "preg_replace_callback");
    call_user_function(NULL, NULL, &func, return_value, 3, args_callback);
#pragma endregion

    zend_update_property(stringy_ce, &instance, ZEND_STRL("str"),  return_value);

    RETURN_ZVAL(&instance, 1, 0);
}

PHP_METHOD(Stringy, trimLeft)
{
    zval *chars = NULL;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(chars)
    ZEND_PARSE_PARAMETERS_END();

    if (chars != NULL && Z_TYPE_P(chars) != IS_NULL)
    {
        convert_to_string(chars);
        if (Z_STRLEN_P(chars) == 0) {
            chars = malloc(sizeof(zval));
            ZVAL_STRING(chars, "[:space:]");
        } else {
            zval func, delimiter;
            ZVAL_STRING(&func, "preg_quote");
            zval args[] =  {
                *chars,
            };
            call_user_function(NULL, NULL, &func, return_value, 1, args);
            ZVAL_STRING(chars, Z_STRVAL_P(return_value));
        }
    }
    else
    {
        chars = malloc(sizeof(zval));
        ZVAL_STRING(chars, "[:space:]");

    }

    zval func_regexReplace, pattern, replacement, options;
    ZVAL_STRING(&func_regexReplace, "regexReplace");
    zend_string *p = strpprintf(0, "^[%s]+", Z_STRVAL_P(chars));
    ZVAL_STR(&pattern, p);
    ZVAL_STRING(&replacement, "");
    zval args[] = {
        pattern,
        replacement,
    };
    call_user_function(NULL, getThis(), &func_regexReplace, return_value, 2, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_trimLeft, 1)
ZEND_ARG_INFO(0, chars)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, trimRight)
{
    zval *chars = NULL;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(chars)
    ZEND_PARSE_PARAMETERS_END();

    if (chars != NULL && Z_TYPE_P(chars) != IS_NULL)
    {
        convert_to_string(chars);
        if (Z_STRLEN_P(chars) == 0) {
            chars = malloc(sizeof(zval));
            ZVAL_STRING(chars, "[:space:]");
        } else {
            zval func, delimiter;
            ZVAL_STRING(&func, "preg_quote");
            zval args[] =  {
                *chars,
            };
            call_user_function(NULL, NULL, &func, return_value, 1, args);
            ZVAL_STRING(chars, Z_STRVAL_P(return_value));
        }
    }
    else
    {
        chars = malloc(sizeof(zval));
        ZVAL_STRING(chars, "[:space:]");

    }

    zval func_regexReplace, pattern, replacement, options;
    ZVAL_STRING(&func_regexReplace, "regexReplace");
    zend_string *p = strpprintf(0, "[%s]+$", Z_STRVAL_P(chars));
    ZVAL_STR(&pattern, p);
    ZVAL_STRING(&replacement, "");
    zval args[] = {
        pattern,
        replacement,
    };
    call_user_function(NULL, getThis(), &func_regexReplace, return_value, 2, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_trimRight, 1)
ZEND_ARG_INFO(0, chars)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, between)
{
    zval *start_zval, *end_zval, *offset_zval = NULL;
    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_ZVAL(start_zval)
        Z_PARAM_ZVAL(end_zval)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(offset_zval)
    ZEND_PARSE_PARAMETERS_END();

    convert_to_string(start_zval);
    convert_to_string(start_zval);
    convert_to_long(offset_zval);

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    
    zval empty;
    ZVAL_EMPTY_STRING(&empty);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zend_call_method(&instance, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, &empty, encoding);

    zval func, args[] = {
        *start_zval,
        *offset_zval,
    };
    ZVAL_STRING(&func, "indexOf");
    call_user_function(NULL, getThis(), &func, return_value, 2, args);
    if (Z_TYPE_P(return_value) == IS_FALSE) {
        RETURN_ZVAL(&instance, 0, 1); 
    }
    size_t start_index_l = Z_LVAL_P(return_value);

    zval args_strlen[] = {
        *start_zval,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strlen");
    call_user_function(NULL, NULL, &func, return_value, 2, args_strlen);

    size_t substrIndex = start_index_l + Z_LVAL_P(return_value);
    zval substrIndex_zval;
    ZVAL_LONG(&substrIndex_zval, substrIndex);

    zval args_indexOf[] = {
        *end_zval,
        substrIndex_zval,
    };
    ZVAL_STRING(&func, "indexOf");
    call_user_function(NULL, getThis(), &func, return_value, 2, args_indexOf);
    if (Z_TYPE_P(return_value) == IS_FALSE) {
        RETURN_ZVAL(&instance, 0, 1); 
    }
    size_t x_l = Z_LVAL_P(return_value) - substrIndex;
    zval x_zval;
    ZVAL_LONG(&x_zval, x_l);

    zval args_substr[] = {
        substrIndex_zval,
        x_zval,
    };
    ZVAL_STRING(&func, "substr");
    call_user_function(NULL, getThis(), &func, return_value, 2, args_substr);
}
ZEND_BEGIN_ARG_INFO(arginfo_between, 3)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, contains)
{
    zval *needle, *caseSensitive = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(needle)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(caseSensitive)
    ZEND_PARSE_PARAMETERS_END();

    convert_to_boolean(caseSensitive);

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    zval z, func;
    ZVAL_LONG(&z, 0);

    if (Z_TYPE_P(caseSensitive) == IS_TRUE) {
        zval args[] = {
            *str,
            *needle,
            z,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strpos");
        call_user_function(NULL, NULL, &func, return_value, 4, args);
        ZVAL_BOOL(return_value, Z_TYPE_P(return_value) != IS_FALSE);
        return;
    }
    
    zval args_i[] = {
        *str,
        *needle,
        z,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_stripos");
    call_user_function(NULL, NULL, &func, return_value, 4, args_i);
    ZVAL_BOOL(return_value, Z_TYPE_P(return_value) != IS_FALSE);
}
ZEND_BEGIN_ARG_INFO(arginfo_contains, 2)
    ZEND_ARG_INFO(0, needle)
    ZEND_ARG_INFO(0, caseSensitive)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, containsAll)
{
    zval *needles = NULL, *caseSensitive = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(needles)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(caseSensitive)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    convert_to_array(needles);

    if (zend_array_count(Z_ARRVAL_P(needles)) == 0) {
        RETURN_BOOL(0);
    }

    zval *needle;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(needles), needle){
        zval func, args[] = {
            *needle,
            *caseSensitive
        };
        ZVAL_STRING(&func, "contains");
        call_user_function(NULL, getThis(), &func, return_value, 2, args);
        if (Z_TYPE_P(return_value) == IS_FALSE) {
            RETURN_BOOL(0);
        }
    }ZEND_HASH_FOREACH_END();

    RETURN_BOOL(1);
}
ZEND_BEGIN_ARG_INFO(arginfo_containsAll, 2)
    ZEND_ARG_INFO(0, needle)
    ZEND_ARG_INFO(0, caseSensitive)
ZEND_END_ARG_INFO();


PHP_METHOD(Stringy, containsAny)
{
    zval *needles = NULL, *caseSensitive = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(needles)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(caseSensitive)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    convert_to_array(needles);

    if (zend_array_count(Z_ARRVAL_P(needles)) == 0) {
        RETURN_BOOL(0);
    }

    zval *needle;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(needles), needle){
        zval func, args[] = {
            *needle,
            *caseSensitive
        };
        ZVAL_STRING(&func, "contains");
        call_user_function(NULL, getThis(), &func, return_value, 2, args);
        if (Z_TYPE_P(return_value) == IS_TRUE) {
            RETURN_BOOL(1);
        }
    }ZEND_HASH_FOREACH_END();

    RETURN_BOOL(0);
}
ZEND_BEGIN_ARG_INFO(arginfo_containsAny, 2)
    ZEND_ARG_INFO(0, needle)
    ZEND_ARG_INFO(0, caseSensitive)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, countSubstr)
{
    zval *substring = NULL, *caseSensitive = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(substring)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(caseSensitive)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    if (Z_TYPE_P(caseSensitive) == IS_TRUE) {
        zval func, args[] = {
            *str,
            *substring,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr_count");
        call_user_function(NULL, NULL, &func, return_value, 3, args);
        return;
    }
    
    zval str_zval, substring_zval;
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strtoupper");
    call_user_function(NULL, NULL, &func, &str_zval, 2, args);

    zval args_substring[] = {
        *substring,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strtoupper");
    call_user_function(NULL, NULL, &func, &substring_zval, 2, args_substring);

    zval args_count[] = {
        str_zval,
        substring_zval,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_substr_count");
    call_user_function(NULL, NULL, &func, return_value, 3, args_count);
}
ZEND_BEGIN_ARG_INFO(arginfo_countSubstr, 2)
    ZEND_ARG_INFO(0, substring)
    ZEND_ARG_INFO(0, caseSensitive)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, delimit)
{
    zval *delimiter = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(delimiter)
    ZEND_PARSE_PARAMETERS_END();

    zval rv, ret;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval func;
    ZVAL_STRING(&func, "trim");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    zval pattern, replacement;
    ZVAL_STRING(&pattern, "\\B([A-Z])");
    ZVAL_STRING(&replacement, "-\\1");
    zval args[] = {
        pattern,
        replacement,
        *return_value
    };
    ZVAL_STRING(&func, "eregReplace");
    call_user_function(NULL, getThis(), &func, &ret, 3, args);

    zval args_strtolower[] = {
        ret,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strtolower");
    call_user_function(NULL, NULL, &func, return_value, 2, args_strtolower);

    ZVAL_STRING(&pattern, "[-_\\s]+");
    zval args_eregReplace[] = {
        pattern,
        *delimiter,
        *return_value
    };
    ZVAL_STRING(&func, "eregReplace");
    call_user_function(NULL, getThis(), &func, &ret, 3, args_eregReplace);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zend_call_method(&instance, stringy_ce, NULL, ZEND_STRL("__construct"), return_value, 2, &ret, encoding);
    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_delimit, 0)
    ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, dasherize)
{
    zval delimit;
    ZVAL_STRING(&delimit, "-");
    zval func, args[] = {
        delimit,
    };
    ZVAL_STRING(&func, "delimit");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}
 
PHP_METHOD(Stringy, endsWith)
{
    zval *substring, *caseSensitive = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(substring)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(caseSensitive)
    ZEND_PARSE_PARAMETERS_END();

    if (caseSensitive == NULL) {
        caseSensitive = malloc(sizeof(zval));
        ZVAL_BOOL(caseSensitive, IS_TRUE);
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval substring_len;
    zval func, args[] = {
        *substring,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strlen");
    call_user_function(NULL, NULL, &func, &substring_len, 2, args);

    zval str_len;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, &str_len, 0, NULL);

    zval start;
    size_t str_length = Z_LVAL(str_len);
    size_t substring_length = Z_LVAL(substring_len);
    ZVAL_LONG(&start, str_length - substring_length);
    zval endOfStr;
    //\mb_substr($this->str, $strLength - $substringLength, $substringLength, $this->encoding);
    zval args_substr[] = {
        *str,
        start,
        substring_len,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &endOfStr, 4, args_substr);

    if (Z_TYPE_P(caseSensitive) == IS_FALSE) {
        zval args_tolower_substring[] = {
            *substring,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strtolower");
        call_user_function(NULL, NULL, &func, substring, 2, args_tolower_substring);

        zval args_tolower_endOfStr[] = {
            endOfStr,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strtolower");
        call_user_function(NULL, NULL, &func, &endOfStr, 2, args_tolower_endOfStr);
    }

    if (zend_string_equals(Z_STR_P(substring), Z_STR(endOfStr))) {
        RETURN_BOOL(1);
    }

    RETURN_BOOL(0);
}
ZEND_BEGIN_ARG_INFO(arginfo_endsWith, 0)
    ZEND_ARG_INFO(0, substring)
    ZEND_ARG_INFO(0, caseSensitive)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, endsWithAny)
{
    zval *substring, *caseSensitive = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(substring)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(caseSensitive)
    ZEND_PARSE_PARAMETERS_END();

    if (caseSensitive == NULL) {
        caseSensitive = malloc(sizeof(zval));
        ZVAL_BOOL(caseSensitive, IS_TRUE);
    }

    convert_to_array(substring);

    zval *val;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(substring), val) {
        zval func, args[] = {
            *val,
            *caseSensitive,
        };
        ZVAL_STRING(&func, "endsWith");
        call_user_function(NULL, getThis(), &func, return_value, 2, args);
        if (Z_TYPE_P(return_value) == IS_TRUE) {
            RETURN_BOOL(1);
        }
    } ZEND_HASH_FOREACH_END();
    
    RETURN_BOOL(0);
}
ZEND_BEGIN_ARG_INFO(arginfo_endsWithAny, 0)
    ZEND_ARG_INFO(0, substring)
    ZEND_ARG_INFO(0, caseSensitive)
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
    PHP_ME(Stringy, camelize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, trim, arginfo_trim, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, trimLeft, arginfo_trimLeft, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, trimRight, arginfo_trimRight, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, between, arginfo_between, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, contains, arginfo_contains, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, containsAll, arginfo_containsAll, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, containsAny, arginfo_containsAny, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, countSubstr, arginfo_countSubstr, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, delimit, arginfo_delimit, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, dasherize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, endsWith, arginfo_endsWith, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, endsWithAny, arginfo_endsWithAny, ZEND_ACC_PUBLIC)
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
