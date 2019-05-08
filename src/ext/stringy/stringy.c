#include "stringy.h"

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
    zval instance, *str, *encoding = NULL;

    ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(str)
    Z_PARAM_ZVAL(encoding)
    ZEND_PARSE_PARAMETERS_END();

    object_init_ex(&instance, stringy_ce);

    if (encoding == NULL) {
        encoding = malloc(sizeof(zval));
        ZVAL_STRING(encoding, "UTF-8");
    }

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
    zval *start, *length = NULL;
    zval func;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_ZVAL(start)
    Z_PARAM_OPTIONAL
    Z_PARAM_ZVAL(length)
    ZEND_PARSE_PARAMETERS_END();

    if (length == NULL)
    {
        zval func;
        ZVAL_STRING(&func, "length");
        call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
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
    zval startOfStr;
    //\mb_substr($this->str, $strLength - $substringLength, $substringLength, $this->encoding);
    zval args_substr[] = {
        *str,
        start,
        substring_len,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &startOfStr, 4, args_substr);

    if (Z_TYPE_P(caseSensitive) == IS_FALSE) {
        zval args_tolower_substring[] = {
            *substring,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strtolower");
        call_user_function(NULL, NULL, &func, substring, 2, args_tolower_substring);

        zval args_tolower_startOfStr[] = {
            startOfStr,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strtolower");
        call_user_function(NULL, NULL, &func, &startOfStr, 2, args_tolower_startOfStr);
    }

    if (zend_string_equals(Z_STR_P(substring), Z_STR(startOfStr))) {
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

PHP_METHOD(Stringy, startsWith)
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

    zval start;
    ZVAL_LONG(&start, 0);
    zval startOfStr;
    zval args_substr[] = {
        *str,
        start,
        substring_len,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, &startOfStr, 4, args_substr);

    if (Z_TYPE_P(caseSensitive) == IS_FALSE) {
        zval args_tolower_substring[] = {
            *substring,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strtolower");
        call_user_function(NULL, NULL, &func, substring, 2, args_tolower_substring);

        zval args_tolower_startOfStr[] = {
            startOfStr,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_strtolower");
        call_user_function(NULL, NULL, &func, &startOfStr, 2, args_tolower_startOfStr);
    }

    if (zend_string_equals(Z_STR_P(substring), Z_STR(startOfStr))) {
        RETURN_BOOL(1);
    }

    RETURN_BOOL(0);
}
ZEND_BEGIN_ARG_INFO(arginfo_startsWith, 0)
    ZEND_ARG_INFO(0, substring)
    ZEND_ARG_INFO(0, caseSensitive)
ZEND_END_ARG_INFO();



PHP_METHOD(Stringy, ensureLeft)
{
    zval *substring = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(substring)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args);

    zval args_startWith[] = {
        *substring
    };
    ZVAL_STRING(&func, "startsWith");
    call_user_function(NULL, &instance, &func, return_value, 1, args_startWith);

    if (Z_TYPE_P(return_value) == IS_FALSE) {
        zval tmp;
        concat_function(&tmp, substring, str);
        zend_update_property(stringy_ce, &instance, ZEND_STRL("str"), &tmp);
    }

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_ensureLeft, 0)
    ZEND_ARG_INFO(0, substring)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, ensureRight)
{
    zval *substring = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(substring)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args);

    zval args_startWith[] = {
        *substring
    };
    ZVAL_STRING(&func, "endsWith");
    call_user_function(NULL, &instance, &func, return_value, 1, args_startWith);

    if (Z_TYPE_P(return_value) == IS_FALSE) {
        zval tmp;
        concat_function(&tmp, str, substring);
        zend_update_property(stringy_ce, &instance, ZEND_STRL("str"), &tmp);
    }

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_ensureRight, 0)
    ZEND_ARG_INFO(0, substring)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, first)
{
    zval *n = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(n)
    ZEND_PARSE_PARAMETERS_END();
    
    if (n == NULL) {
        n = malloc(sizeof(zval));
        ZVAL_LONG(n, 0);
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args);

    zend_long len = Z_LVAL_P(n);
    if (len < 0) {
        zend_update_property_string(stringy_ce, &instance, ZEND_STRL("str"), "");
        RETURN_ZVAL(&instance, 0, 1);
    }

    zval start;
    ZVAL_LONG(&start, 0);

    zval args_substr[] = {
        start,
        *n,
    };
    ZVAL_STRING(&func, "substr");
    call_user_function(NULL, &instance, &func, return_value, 2, args_substr);
}
ZEND_BEGIN_ARG_INFO(arginfo_first, 0)
    ZEND_ARG_INFO(0, substring)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, last)
{
    zval *n = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(n)
    ZEND_PARSE_PARAMETERS_END();
    
    if (n == NULL) {
        n = malloc(sizeof(zval));
        ZVAL_LONG(n, 0);
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args);

    zend_long len = Z_LVAL_P(n);
    if (len <= 0) {
        zend_update_property_string(stringy_ce, &instance, ZEND_STRL("str"), "");
        RETURN_ZVAL(&instance, 0, 1);
    }

    ZVAL_LONG(n, -Z_LVAL_P(n));

    zval args_substr[] = {
        *n,
    };
    ZVAL_STRING(&func, "substr");
    call_user_function(NULL, &instance, &func, return_value, 1, args_substr);
}
ZEND_BEGIN_ARG_INFO(arginfo_last, 0)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, isAlpha)
{
    zval str_val;
    ZVAL_STRING(&str_val, "^[[:alpha:]]*$");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, matchesPattern)
{
    zval *pattern = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(pattern)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    zval func, args[] = {
        *pattern,
        *str,
    };
    ZVAL_STRING(&func, "mb_ereg_match");
    call_user_function(NULL, NULL, &func, return_value, 2, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_matchesPattern, 0)
    ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, isBlank)
{
    zval str_val;
    ZVAL_STRING(&str_val, "^[[:space:]]*$");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, hasLowerCase)
{
    zval str_val;
    ZVAL_STRING(&str_val, ".*[[:lower:]]");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, hasUpperCase)
{
    zval str_val;
    ZVAL_STRING(&str_val, ".*[[:upper:]]");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, htmlDecode)
{
    zval *flags = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(flags);
    ZEND_PARSE_PARAMETERS_END();

    if (flags == NULL) {
        ZVAL_STRING(flags, "ENT_COMPAT");
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval func, args[] = {
        *str,
        *flags,
        *encoding,
    };
    ZVAL_STRING(&func, "html_entity_decode");
    call_user_function(NULL, NULL, &func, return_value, 3, args);


    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func_init, args_init[] = {
        *return_value,
        *encoding,
    };
    ZVAL_STRING(&func_init, "__construct");
    call_user_function(NULL, &instance, &func_init, return_value, 2, args_init);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_htmlDecode, 0)
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, htmlEncode)
{
    zval *flags = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(flags);
    ZEND_PARSE_PARAMETERS_END();

    if (flags == NULL) {
        ZVAL_STRING(flags, "ENT_COMPAT");
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval func, args[] = {
        *str,
        *flags,
        *encoding,
    };
    ZVAL_STRING(&func, "htmlentities");
    call_user_function(NULL, NULL, &func, return_value, 3, args);


    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func_init, args_init[] = {
        *return_value,
        *encoding,
    };
    ZVAL_STRING(&func_init, "__construct");
    call_user_function(NULL, &instance, &func_init, return_value, 2, args_init);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_htmlEncode, 0)
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, removeLeft)
{
    zval *substring = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(substring);
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func_init, args_init[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func_init, "__construct");
    call_user_function(NULL, &instance, &func_init, return_value, 2, args_init);

    zval func, args[] = {
        *substring,
    };
    ZVAL_STRING(&func, "startsWith");
    call_user_function(NULL, &instance, &func, return_value, 1, args);
    
    if (Z_TYPE_P(return_value) == IS_TRUE) {
        zval func_strlen, args_strlen[] = {
            *substring,
            *encoding,
        };
        ZVAL_STRING(&func_strlen, "mb_strlen");
        call_user_function(NULL, NULL, &func_strlen, return_value, 2, args_strlen);

        zval func_substr, args_substr[] = {
            *return_value,
        };
        ZVAL_STRING(&func_substr, "substr");
        call_user_function(NULL, &instance, &func_substr, return_value, 1, args_substr);
        return;
    }

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_removeLeft, 0)
    ZEND_ARG_INFO(0, substring)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, removeRight)
{
    zval *substring = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(substring);
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func_init, args_init[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func_init, "__construct");
    call_user_function(NULL, &instance, &func_init, return_value, 2, args_init);

    zval func, args[] = {
        *substring,
    };
    ZVAL_STRING(&func, "endsWith");
    call_user_function(NULL, &instance, &func, return_value, 1, args);
    
    if (Z_TYPE_P(return_value) == IS_TRUE) {
        zval func_strlen, args_strlen[] = {
            *substring,
            *encoding,
        };
        ZVAL_STRING(&func_strlen, "mb_strlen");
        call_user_function(NULL, NULL, &func_strlen, return_value, 2, args_strlen);
        size_t substing_len = Z_LVAL_P(return_value);

        zval func_len;
        ZVAL_STRING(&func_len, "length");
        call_user_function(NULL, getThis(), &func_len, return_value, 0, NULL);
        size_t len = Z_LVAL_P(return_value);

        
        zval zero;
        zval second;
        ZVAL_LONG(&zero, 0);
        ZVAL_LONG(&second, len - substing_len);
        zval func_substr, args_substr[] = {
            zero,
            second,
        };
        ZVAL_STRING(&func_substr, "substr");
        call_user_function(NULL, getThis(), &func_substr, return_value, 2, args_substr);
        return;
    }

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_removeRight, 0)
    ZEND_ARG_INFO(0, substring)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, humanize)
{
    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"),  0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"),  0, &rv);

    zval search;
    array_init(&search);
    add_next_index_string(&search, "_id");
    add_next_index_string(&search, "_");

    zval repalce;
    array_init(&repalce);
    add_next_index_string(&repalce, "");
    add_next_index_string(&repalce, " ");

    zval func, args[] = {
        search,
        repalce,
        *str,
    };
    ZVAL_STRING(&func, "str_replace");
    call_user_function(NULL, NULL, &func, return_value, 3, args);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func_construct, args_construct[] = {
        *return_value,
        *encoding,
    };
    ZVAL_STRING(&func_construct, "__construct");
    call_user_function(NULL, &instance, &func_construct, return_value, 2, args_construct);

    zval func_trim;
    ZVAL_STRING(&func_trim, "trim");
    call_user_function(NULL, &instance, &func_trim, return_value, 0, NULL);

    zval func_upperCaseFirst;
    ZVAL_STRING(&func_upperCaseFirst, "upperCaseFirst");
    call_user_function(NULL, return_value, &func_upperCaseFirst, return_value, 0, NULL);

    RETURN_ZVAL(return_value, 0, 1);
}

PHP_METHOD(Stringy, insert)
{
    zval *substring, *index;
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(substring)
        Z_PARAM_ZVAL(index)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"),  0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"),  0, &rv);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args);


    zval len;
    zval func_length;
    ZVAL_STRING(&func_length, "length");
    call_user_function(NULL, &instance, &func_length, &len, 0, NULL);

    if (Z_LVAL_P(index) > Z_LVAL(len)) {
        RETURN_ZVAL(&instance, 0, 1);
    }

    zval zero;
    ZVAL_LONG(&zero, 0);
    
    zval start;
    zval func_substr1, args_substr1[] = {
        *str,
        zero,
        *index,
        *encoding,
    };
    ZVAL_STRING(&func_substr1, "mb_substr");
    call_user_function(NULL, NULL, &func_substr1, &start, 4, args_substr1);

    zval end;
    zval func_substr2, args_substr2[] = {
        *str,
        *index,
        len,
        *encoding,
    };
    ZVAL_STRING(&func_substr2, "mb_substr");
    call_user_function(NULL, NULL, &func_substr2, &end, 4, args_substr2);

    zval ret;
    concat_function(&ret, &start, substring);
    concat_function(&ret, &ret, &end);

    zend_update_property(stringy_ce, &instance, ZEND_STRL("str"), &ret);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_insert, 0)
    ZEND_ARG_INFO(0, substring)
    ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, isAlphanumeric)
{
    zval str_val;
    ZVAL_STRING(&str_val, "^[[:alnum:]]*$");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, isLowerCase)
{
    zval str_val;
    ZVAL_STRING(&str_val, "^[[:lower:]]*$");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, isUpperCase)
{
    zval str_val;
    ZVAL_STRING(&str_val, "^[[:upper:]]*$");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, isHexadecimal)
{
    zval str_val;
    ZVAL_STRING(&str_val, "^[[:xdigit:]]*$");
    zval func, args[] = {
        str_val
    };
    ZVAL_STRING(&func, "matchesPattern");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);
}

PHP_METHOD(Stringy, isJson)
{
    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    if (Z_LVAL_P(return_value) <= 0) {
        RETURN_BOOL(0);
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    zval func_json_decode, args[] = {
        *str,
    };
    ZVAL_STRING(&func_json_decode, "json_decode");
    call_user_function(NULL, NULL, &func_json_decode, return_value, 1, args);

    ZVAL_STRING(&func, "json_last_error");
    call_user_function(NULL, NULL, &func, return_value, 0, NULL);

    zval *json_error_none = zend_get_constant_str(ZEND_STRL("JSON_ERROR_NONE"));
    
    if (Z_LVAL_P(json_error_none) == Z_LVAL_P(return_value)) {
        RETURN_BOOL(1);
    }
    RETURN_BOOL(0);
}

PHP_METHOD(Stringy, isSerialized)
{
    zval str_object_serialized;
    ZVAL_STRING(&str_object_serialized, "b:0;");

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    
    if (Z_TYPE_P(str) == Z_TYPE(str_object_serialized) && Z_STR_P(str) == Z_STR(str_object_serialized))
    {
        RETURN_TRUE;
    }
    zval func;

    zval error_level;
    ZVAL_STRING(&func, "error_reporting");
    zval new_error_level;
    ZVAL_LONG(&new_error_level, 0);
    zval args_error_reporting[] = {
        new_error_level,
    };
    call_user_function(NULL, NULL, &func, &error_level, 1, args_error_reporting);

    zval args[] = {
        *str,
    };
    ZVAL_STRING(&func, "unserialize");
    call_user_function(NULL, NULL, &func, return_value, 1, args);
    
    zval reset_error_level;
    zval args_reset[] = {
        error_level,
    };
    ZVAL_STRING(&func, "error_reporting");
    call_user_function(NULL, NULL, &func, &reset_error_level, 1, args_reset);

    if (Z_TYPE_P(return_value) != IS_FALSE) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Stringy, isBase64)
{
    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    
    zval z_true;
    ZVAL_BOOL(&z_true, IS_TRUE);
    zval func, args[] = {
        *str,
        z_true,
    };
    ZVAL_STRING(&func, "base64_decode");
    call_user_function(NULL, NULL, &func, return_value, 2, args);

    zval args_decode[] = {
        *return_value,
    };
    ZVAL_STRING(&func, "base64_encode");
    call_user_function(NULL, NULL, &func, return_value, 1, args_decode);

    if (zval_str_equal(return_value, str)) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Stringy, longestCommonPrefix)
{
    zval *otherStr;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(otherStr)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    size_t length = Z_LVAL_P(return_value);

    zval args_mb_strlen[] = {
        *otherStr,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strlen");
    call_user_function(NULL, NULL, &func, return_value, 2, args_mb_strlen);
    size_t mb_strlen_length = Z_LVAL_P(return_value);

    size_t maxLength = MIN(length, mb_strlen_length);
    
    zval longestCommonPrefix, i_tmp, const_substr_len, ret_first, ret_second;
    ZVAL_EMPTY_STRING(&longestCommonPrefix);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    ZVAL_LONG(&const_substr_len, 1);

    for(int i = 0; i < maxLength; i++) {
        ZVAL_LONG(&i_tmp, i);
        zval args_mb_substr[] = {
            *str,
            i_tmp,
            const_substr_len,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &ret_first, 4, args_mb_substr);

        zval args_mb_substr2[] = {
            *otherStr,
            i_tmp,
            const_substr_len,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &ret_second, 4, args_mb_substr2);
        
        if (zend_string_equals(Z_STR(ret_first), Z_STR(ret_second)) == false) {
            break;
        }

        concat_function(&longestCommonPrefix, &longestCommonPrefix, &ret_first);
    }

    zval instance;
    object_init_ex(&instance, stringy_ce);

    zval args_const[] = {
        longestCommonPrefix,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args_const);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_longestCommonPrefix, 0)
    ZEND_ARG_INFO(0, otherStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, longestCommonSuffix)
{
    zval *otherStr;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(otherStr)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);
    size_t length = Z_LVAL_P(return_value);

    zval args_mb_strlen[] = {
        *otherStr,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strlen");
    call_user_function(NULL, NULL, &func, return_value, 2, args_mb_strlen);
    size_t mb_strlen_length = Z_LVAL_P(return_value);

    size_t maxLength = MIN(length, mb_strlen_length);
    
    zval longestCommonSuffix, i_tmp, const_substr_len, ret_first, ret_second;
    ZVAL_EMPTY_STRING(&longestCommonSuffix);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    ZVAL_LONG(&const_substr_len, 1);

    for(int i = 1; i <= maxLength; i++) {
        ZVAL_LONG(&i_tmp, -i);
        zval args_mb_substr[] = {
            *str,
            i_tmp,
            const_substr_len,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &ret_first, 4, args_mb_substr);

        zval args_mb_substr2[] = {
            *otherStr,
            i_tmp,
            const_substr_len,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &ret_second, 4, args_mb_substr2);
        
        if (zend_string_equals(Z_STR(ret_first), Z_STR(ret_second)) == false) {
            break;
        }
        zval tmp;
        concat_function(&tmp, &ret_first, &longestCommonSuffix);
        longestCommonSuffix = tmp;
    }

    zval instance;
    object_init_ex(&instance, stringy_ce);

    zval args_const[] = {
        longestCommonSuffix,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args_const);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_longestCommonSuffix, 0)
    ZEND_ARG_INFO(0, otherStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, longestCommonSubstring)
{
    // Uses dynamic programming to solve
    // http://en.wikipedia.org/wiki/Longest_common_substring_problem
    
    zval *otherStr;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(otherStr)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);

    zval instance, func;
    object_init_ex(&instance, stringy_ce);

    zval args_const[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args_const);

    zval strLen;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, &instance, &func, &strLen, 0, NULL);

    zval otherLength, args_mb_strlen[] = {
        *otherStr,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strlen");
    call_user_function(NULL, NULL, &func, &otherLength, 2, args_mb_strlen);

    if (Z_LVAL(strLen) == 0 || Z_LVAL(otherLength) == 0) {
        zend_update_property_stringl(stringy_ce, &instance, ZEND_STRL("str"), ZEND_STRL(""));
        RETURN_ZVAL(&instance, 0, 1);
    }

    zval len, end, table, tmp, zero;
    ZVAL_LONG(&len, 0);
    ZVAL_LONG(&end, 0);
    ZVAL_LONG(&zero, 0);
    ZVAL_LONG(&otherLength, Z_LVAL(otherLength)+1);
    ZVAL_LONG(&strLen, Z_LVAL(strLen)+1);
    array_init(&table);

    zval args_array_fill[] = {
        zero,
        otherLength,
        zero,
    };
    ZVAL_STRING(&func, "array_fill");
    call_user_function(NULL, NULL, &func, return_value, 3, args_array_fill);

    zval args_array_fill2[] = {
        zero,
        strLen,
        *return_value,
    };
    call_user_function(NULL, NULL, &func, &table, 3, args_array_fill2);

    zval strChar, otherChar, i_tmp, j_tmp, const_substr_len;
    ZVAL_LONG(&const_substr_len, 1);
    for(int i = 1; i < Z_LVAL(strLen); i++) {
        ZVAL_LONG(&i_tmp, i - 1);
        zval args_mb_strlen[] = {
                *str,
                i_tmp,
                const_substr_len,
                *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &strChar, 4, args_mb_strlen);
        
        zend_array *ht = Z_ARRVAL(table);
        for(int j = 1; j < Z_LVAL(otherLength); j++) {
            ZVAL_LONG(&j_tmp, j - 1);
            zval args_mb_strlen2[] = {
                *otherStr,
                j_tmp,
                const_substr_len,
                *encoding,
            };
            ZVAL_STRING(&func, "mb_substr");
            call_user_function(NULL, NULL, &func, &otherChar, 4, args_mb_strlen2);
         
            zval *tmp = zend_hash_index_find(ht, i);
            if (zend_string_equals(Z_STR(otherChar), Z_STR(strChar))) {
                zval *tmp2 = zend_hash_index_find(ht, i - 1);
                tmp2 = zend_hash_index_find(Z_ARRVAL_P(tmp2), j - 1);
                zval value;
                ZVAL_LONG(&value, Z_LVAL_P(tmp2) + 1);

                zval_add_ref(&value);
                zend_hash_index_update(Z_ARR_P(tmp), j, &value);

                if (Z_LVAL(value) > Z_LVAL(len)) {
                    ZVAL_LONG(&len, Z_LVAL(value));
                    ZVAL_LONG(&end, i);
                }
            } else {
                zval_add_ref(&zero);
                zend_hash_index_update(Z_ARR_P(tmp), j, &zero);
            }

            zval_add_ref(tmp);
            zend_hash_index_update(ht, j, tmp);
        }
    }

    zval end_len;
    ZVAL_LONG(&end_len, Z_LVAL(end) - Z_LVAL(len));

    zval args_mb_strlen3[] = {
        *str,
        end_len,
        len,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_substr");
    call_user_function(NULL, NULL, &func, return_value, 4, args_mb_strlen3);

    zend_update_property(stringy_ce, &instance, ZEND_STRL("str"), return_value);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_longestCommonSubstring, 0)
    ZEND_ARG_INFO(0, otherStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, pad) 
{
    zval *length, *padStr, *padType;
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_ZVAL(length)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(padStr)
        Z_PARAM_ZVAL(padType)
    ZEND_PARSE_PARAMETERS_END();

    if (padStr == NULL) {
        padStr = malloc(sizeof(zval));
        ZVAL_STRING(padStr, " ");
    }

    if (padType == NULL) {
        padType = malloc(sizeof(zval));
        ZVAL_STRING(padType, "right");
    }

    zval left, right, both;
    ZVAL_STRING(&left, "left");
    ZVAL_STRING(&right, "right");
    ZVAL_STRING(&both, "both");

    zval func;
    
    if (zval_str_equal(&left, padType)) {
        ZVAL_STRING(&func, "padLeft");
    } else if (zval_str_equal(&right, padType) == 1) {
        ZVAL_STRING(&func, "padRight");
    } else if (zval_str_equal(&both, padType) == 1) {
        ZVAL_STRING(&func, "padBoth");
    } else {
        zend_throw_exception_ex(
            spl_ce_InvalidArgumentException, 
            0, 
            "Pad expects %s to be one of 'left', 'right' or 'both'",
            Z_STRVAL_P(padType)
        );
    }

    zval args[] = {
        *length, 
        *padStr
    };

    call_user_function(NULL, getThis(), &func, return_value, 2, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_pad, 0)
    ZEND_ARG_INFO(0, length)
    ZEND_ARG_INFO(0, padStr)
    ZEND_ARG_INFO(0, padType)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, applyPadding) 
{
    zval *left, *right, *padStr;
    ZEND_PARSE_PARAMETERS_START(0, 3)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(left);
        Z_PARAM_ZVAL(right);
        Z_PARAM_ZVAL(padStr);
    ZEND_PARSE_PARAMETERS_END();

    zval rv, zero;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 1, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 1, &rv);

    ZVAL_LONG(&zero, 0);

    if (left == NULL) {
        left = malloc(sizeof(zval));
        ZVAL_LONG(left, 0);
    }

    if (Z_TYPE_P(left) != IS_LONG) {
        convert_to_long(left);
    }

    if (right == NULL) {
        right = malloc(sizeof(zval));
        ZVAL_LONG(right, 0);
    }

    if (Z_TYPE_P(right) != IS_LONG) {
        convert_to_long(right);
    }

    if (padStr == NULL) {
        padStr = malloc(sizeof(zval));
        ZVAL_STRING(padStr, " ");
    }

    zval stringy;
    object_init_ex(&stringy, stringy_ce);
    zval func, args[] = {
        *str,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &stringy, &func, return_value, 2, args);

    zval length;
    zval args_mb_strlen[] = {
        *padStr,
        *encoding,
    };
    ZVAL_STRING(&func, "mb_strlen");
    call_user_function(NULL, NULL, &func, &length, 2, args_mb_strlen);

    zval strLength;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, &stringy, &func, return_value, 0, NULL);

    zval paddedLength;
    ZVAL_LONG(&paddedLength, Z_LVAL(strLength) + Z_LVAL_P(left) + Z_LVAL_P(right));
    
    if (Z_TYPE(length) == IS_FALSE || Z_LVAL(paddedLength) <= Z_LVAL(strLength)) {
        RETURN_ZVAL(&stringy, 0, 1);
    }

    zval str_repeat_0;
    zval str_repeat_0_val;
    ZVAL_LONG(&str_repeat_0_val, ceil(Z_LVAL_P(left)/(float)Z_LVAL(length)));
    zval leftPadding;    
    if (Z_LVAL(str_repeat_0_val) >=0)
    {
        zval args_str_repeat[] = {
            *padStr,
            str_repeat_0_val,
        };
        ZVAL_STRING(&func, "str_repeat");
        call_user_function(NULL, NULL, &func, &str_repeat_0, 2, args_str_repeat);

        zval args_leftPadding[] = {
            str_repeat_0,
            zero,
            *left,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &leftPadding, 4, args_leftPadding);
    } else {
        ZVAL_STRING(&leftPadding, "");
    }

    zval str_repeat_1;
    zval str_repeat_1_val;
    ZVAL_LONG(&str_repeat_1_val, ceil(Z_LVAL_P(right)/(float)Z_LVAL(length)));
    zval rightPadding;    
    if (Z_LVAL(str_repeat_1_val) >=0)
    {
        zval args_str_repeat_1[] = {
            *padStr,
            str_repeat_1_val,
        };
        ZVAL_STRING(&func, "str_repeat");
        call_user_function(NULL, NULL, &func, &str_repeat_1, 2, args_str_repeat_1);

        zval args_rightPadding[] = {
            str_repeat_1,
            zero,
            *right,
            *encoding,
        };
        ZVAL_STRING(&func, "mb_substr");
        call_user_function(NULL, NULL, &func, &rightPadding, 4, args_rightPadding);
    } else {
        ZVAL_STRING(&rightPadding, "");
    }

    zval x;
    concat_function(&x, &leftPadding, str);
    concat_function(&x, &x, &rightPadding);

    RETURN_ZVAL(&x, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_applyPadding, 0)
    ZEND_ARG_INFO(0, left)
    ZEND_ARG_INFO(0, right)
    ZEND_ARG_INFO(0, padStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, padLeft)
{
    zval *length, *padStr;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(length)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(padStr)
    ZEND_PARSE_PARAMETERS_END();

    if (padStr == NULL) {
        padStr = malloc(sizeof(zval));
        ZVAL_STRING(padStr, " ");
    }

    zval zero;
    ZVAL_LONG(&zero, 0);

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    zval first;
    ZVAL_LONG(&first, Z_LVAL_P(length) - Z_LVAL_P(return_value));
    zval args[] = {
        first,
        zero,
        *padStr,
    };
    ZVAL_STRING(&func, "applyPadding");
    call_user_function(NULL, getThis(), &func, return_value, 3, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_padLeft, 0)
    ZEND_ARG_INFO(0, length)
    ZEND_ARG_INFO(0, padStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, padRight)
{
    zval *length, *padStr;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(length)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(padStr)
    ZEND_PARSE_PARAMETERS_END();

    if (padStr == NULL) {
        padStr = malloc(sizeof(zval));
        ZVAL_STRING(padStr, " ");
    }

    zval zero;
    ZVAL_LONG(&zero, 0);

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    zval value;
    ZVAL_LONG(&value, Z_LVAL_P(length) - Z_LVAL_P(return_value));
    zval args[] = {
        zero,
        value,
        *padStr,
    };
    ZVAL_STRING(&func, "applyPadding");
    call_user_function(NULL, getThis(), &func, return_value, 3, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_padRight, 0)
    ZEND_ARG_INFO(0, length)
    ZEND_ARG_INFO(0, padStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, padBoth)
{
    zval *length, *padStr;
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(length)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(padStr)
    ZEND_PARSE_PARAMETERS_END();

    if (padStr == NULL) {
        padStr = malloc(sizeof(zval));
        ZVAL_STRING(padStr, " ");
    }

    zval func;
    ZVAL_STRING(&func, "length");
    call_user_function(NULL, getThis(), &func, return_value, 0, NULL);

    zval value;
    ZVAL_LONG(&value, Z_LVAL_P(length) - Z_LVAL_P(return_value));

    zval first, second;
    ZVAL_LONG(&first, floor(Z_LVAL(value) / 2.0));
    ZVAL_LONG(&second, ceil(Z_LVAL(value) / 2.0);)

    zval args[] = {
        first,
        second,
        *padStr,
    };
    ZVAL_STRING(&func, "applyPadding");
    call_user_function(NULL, getThis(), &func, return_value, 3, args);
}
ZEND_BEGIN_ARG_INFO(arginfo_padBoth, 0)
    ZEND_ARG_INFO(0, length)
    ZEND_ARG_INFO(0, padStr)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, repeat)
{
    zval *multilpier;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(multilpier)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    zval func, args[] = {
        *str,
        *multilpier,
    };
    ZVAL_STRING(&func, "str_repeat");
    call_user_function(NULL, NULL, &func, return_value, 2, args);

    zval instance;
    object_init_ex(&instance, stringy_ce);
    
    zval args_construct[] = {
        *return_value,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args_construct);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_repeat, 0)
    ZEND_ARG_INFO(0, multiplier)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, replace)
{
    zval *search, *replacement;
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(search)
        Z_PARAM_ZVAL(replacement)
    ZEND_PARSE_PARAMETERS_END();

    zval func, delimiter;
    ZVAL_STRING(&func, "preg_quote");
    zval args[] =  {
        *search,
    };
    call_user_function(NULL, NULL, &func, return_value, 1, args);
    
    ZVAL_STRING(&func, "regexReplace");
    zval args_regex[] =  {
        *return_value,
        *replacement,
    };
    call_user_function(NULL, getThis(), &func, return_value, 2, args_regex);
}
ZEND_BEGIN_ARG_INFO(arginfo_replace, 0)
    ZEND_ARG_INFO(0, search)
    ZEND_ARG_INFO(0, replacement)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, toAscii)
{
    zval *language, *removeUnsupported;
    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(language)
        Z_PARAM_ZVAL(removeUnsupported)
    ZEND_PARSE_PARAMETERS_END();

    if (language == NULL) {
        language = (zval *) malloc(sizeof(zval));
        ZVAL_STRING(language, "en");
    }

    if (removeUnsupported == NULL) {
        removeUnsupported = (zval *) malloc(sizeof(zval));
        ZVAL_BOOL(removeUnsupported, IS_TRUE);
    }

    zval rv;
    zval *str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval tmp = *str;

    zval func, args[] = {
        *language
    };
    ZVAL_STRING(&func, "langSpecificCharsArray");
    call_user_function(NULL, getThis(), &func, return_value, 1, args);

    zval chars;
    if (zend_array_count(Z_ARRVAL_P(return_value)) > 0) {
        zval args_preg_replace_1[] = {
            *zend_hash_index_find(Z_ARRVAL_P(return_value), 0),
            *zend_hash_index_find(Z_ARRVAL_P(return_value), 1),
            *str,
        };
        ZVAL_STRING(&func, "str_replace");
        call_user_function(NULL, NULL, &func, &tmp, 3, args_preg_replace_1);
    }

    ZVAL_STRING(&func, "charsArray");
    call_user_function(NULL, getThis(), &func, &chars, 0, NULL);

    zval *value;
    zend_string *key;
    zval x;
    ulong idx;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(chars), idx, key, value) {
        if (!key) {
            key = zend_long_to_str(idx);
         }
        ZVAL_NEW_STR(&x, key);

        zval args_preg_replace_2[] = {
            *value,
            x,
            tmp,
        };
        ZVAL_STRING(&func, "str_replace");

        call_user_function(NULL, NULL, &func, &tmp, 3, args_preg_replace_2);
    } ZEND_HASH_FOREACH_END();

    if (Z_TYPE_P(removeUnsupported) == IS_TRUE) {
        zval pattern, replacement;
        ZVAL_STRING(&pattern, "/[^\x20-\x7E]/u");
        ZVAL_STRING(&replacement, "");

        zval args_preg_replace_3[] = {
            pattern,
            replacement,
            tmp,
        };
        ZVAL_STRING(&func, "preg_replace");
        call_user_function(NULL, NULL, &func, &tmp, 3, args_preg_replace_3);
    }

    zval instance;
    object_init_ex(&instance, stringy_ce);

    zval *encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 0, &rv);
    
    zval args_construct[] = {
        tmp,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args_construct);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_toAscii, 0)
    ZEND_ARG_INFO(0, language)
    ZEND_ARG_INFO(0, removeUnsupported)
ZEND_END_ARG_INFO();

//TODO: 先实现再说
PHP_METHOD(Stringy, charsArray)
{
    zval ret;
    array_init(&ret);
        
    zval tb_0;
    array_init(&tb_0);
    char* th_0_arr[] = {"°", "₀", "۰", "０"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_0, th_0_arr[i]);
    }
    add_assoc_zval(&ret, "0", &tb_0);

    zval tb_1;
    array_init(&tb_1);
    char* th_1_arr[] = {"¹", "₁", "۱", "１"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_1, th_1_arr[i]);
    }
    add_assoc_zval(&ret, "1", &tb_1);

    zval tb_2;
    array_init(&tb_2);
    char* th_2_arr[] = {"²", "₂", "۲", "２"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_2, th_2_arr[i]);
    }
    add_assoc_zval(&ret, "2", &tb_2);

    zval tb_3;
    array_init(&tb_3);
    char* th_3_arr[] = {"³", "₃", "۳", "３"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_3, th_3_arr[i]);
    }
    add_assoc_zval(&ret, "3", &tb_3);

    zval tb_4;
    array_init(&tb_4);
    char* th_4_arr[] = {"⁴", "₄", "۴", "٤", "４"};
    for(int i = 0; i < 5; i++) {
        add_next_index_string(&tb_4, th_4_arr[i]);
    }
    add_assoc_zval(&ret, "4", &tb_4);

    zval tb_5;
    array_init(&tb_5);
    char* th_5_arr[] = {"⁵", "₅", "۵", "٥", "５"};
    for(int i = 0; i < 5; i++) {
        add_next_index_string(&tb_5, th_5_arr[i]);
    }
    add_assoc_zval(&ret, "5", &tb_5);

    zval tb_6;
    array_init(&tb_6);
    char* th_6_arr[] = {"⁶", "₆", "۶", "٦", "６"};
    for(int i = 0; i < 5; i++) {
        add_next_index_string(&tb_6, th_6_arr[i]);
    }
    add_assoc_zval(&ret, "5", &tb_6);

    zval tb_7;
    array_init(&tb_7);
    char* th_7_arr[] = {"⁷", "₇", "۷", "７"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_7, th_7_arr[i]);
    }
    add_assoc_zval(&ret, "7", &tb_7);

    zval tb_8;
    array_init(&tb_8);
    char* th_8_arr[] = {"⁸", "₈", "۸", "８"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_8, th_8_arr[i]);
    }
    add_assoc_zval(&ret, "8", &tb_8);

    zval tb_9;
    array_init(&tb_9);
    char* th_9_arr[] = {"⁹", "₉", "۹", "９"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_9, th_9_arr[i]);
    }
    add_assoc_zval(&ret, "9", &tb_9);

    zval tb_a;
    array_init(&tb_a);
    char* th_a_arr[] = {"à", "á", "ả", "ã", "ạ", "ă", "ắ", "ằ", "ẳ", "ẵ",
                        "ặ", "â", "ấ", "ầ", "ẩ", "ẫ", "ậ", "ā", "ą", "å",
                        "α", "ά", "ἀ", "ἁ", "ἂ", "ἃ", "ἄ", "ἅ", "ἆ", "ἇ",
                        "ᾀ", "ᾁ", "ᾂ", "ᾃ", "ᾄ", "ᾅ", "ᾆ", "ᾇ", "ὰ", "ά",
                        "ᾰ", "ᾱ", "ᾲ", "ᾳ", "ᾴ", "ᾶ", "ᾷ", "а", "أ", "အ",
                        "ာ", "ါ", "ǻ", "ǎ", "ª", "ა", "अ", "ا", "ａ", "ä"};
    for(int i = 0; i < 60; i++) {
        add_next_index_string(&tb_a, th_a_arr[i]);
    }
    add_assoc_zval(&ret, "a", &tb_a);

    zval tb_b;
    array_init(&tb_b);
    char* th_b_arr[] = {"б", "β", "ب", "ဗ", "ბ", "ｂ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_b, th_b_arr[i]);
    }
    add_assoc_zval(&ret, "b", &tb_b);

    zval tb_c;
    array_init(&tb_c);
    char* th_c_arr[] = {"ç", "ć", "č", "ĉ", "ċ", "ｃ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_c, th_c_arr[i]);
    }
    add_assoc_zval(&ret, "c", &tb_c);

    zval tb_d;
    array_init(&tb_d);
    char* th_d_arr[] = {"ď", "ð", "đ", "ƌ", "ȡ", "ɖ", "ɗ", "ᵭ", "ᶁ", "ᶑ",
                        "д", "δ", "د", "ض", "ဍ", "ဒ", "დ", "ｄ"};
    for(int i = 0; i < 18; i++) {
        add_next_index_string(&tb_d, th_d_arr[i]);
    }
    add_assoc_zval(&ret, "d", &tb_d);

    zval tb_e;
    array_init(&tb_e);
    char* th_e_arr[] = {"é", "è", "ẻ", "ẽ", "ẹ", "ê", "ế", "ề", "ể", "ễ",
                        "ệ", "ë", "ē", "ę", "ě", "ĕ", "ė", "ε", "έ", "ἐ",
                        "ἑ", "ἒ", "ἓ", "ἔ", "ἕ", "ὲ", "έ", "е", "ё", "э",
                        "є", "ə", "ဧ", "ေ", "ဲ", "ე", "ए", "إ", "ئ", "ｅ"};
    for(int i = 0; i < 40; i++) {
        add_next_index_string(&tb_e, th_e_arr[i]);
    }
    add_assoc_zval(&ret, "e", &tb_e);

    zval tb_f;
    array_init(&tb_f);
    char* th_f_arr[] = {"ф", "φ", "ف", "ƒ", "ფ", "ｆ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_f, th_f_arr[i]);
    }
    add_assoc_zval(&ret, "f", &tb_f);

    zval tb_g;
    array_init(&tb_g);
    char* th_g_arr[] = {"ĝ", "ğ", "ġ", "ģ", "г", "ґ", "γ", "ဂ", "გ", "گ",
                        "ｇ"};
    for(int i = 0; i < 11; i++) {
        add_next_index_string(&tb_g, th_g_arr[i]);
    }
    add_assoc_zval(&ret, "g", &tb_g);

    zval tb_h;
    array_init(&tb_h);
    char* th_h_arr[] = {"ĥ", "ħ", "η", "ή", "ح", "ه", "ဟ", "ှ", "ჰ", "ｈ"};
    for(int i = 0; i < 10; i++) {
        add_next_index_string(&tb_h, th_h_arr[i]);
    }
    add_assoc_zval(&ret, "h", &tb_h);

    zval tb_i;
    array_init(&tb_i);
    char* th_i_arr[] = {"í", "ì", "ỉ", "ĩ", "ị", "î", "ï", "ī", "ĭ", "į",
                        "ı", "ι", "ί", "ϊ", "ΐ", "ἰ", "ἱ", "ἲ", "ἳ", "ἴ",
                        "ἵ", "ἶ", "ἷ", "ὶ", "ί", "ῐ", "ῑ", "ῒ", "ΐ", "ῖ",
                        "ῗ", "і", "ї", "и", "ဣ", "ိ", "ီ", "ည်", "ǐ", "ი",
                        "इ", "ی", "ｉ"};
    for(int i = 0; i < 43; i++) {
        add_next_index_string(&tb_i, th_i_arr[i]);
    }
    add_assoc_zval(&ret, "i", &tb_i);

    zval tb_j;
    array_init(&tb_j);
    char* th_j_arr[] = {"ĵ", "ј", "Ј", "ჯ", "ج", "ｊ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_j, th_j_arr[i]);
    }
    add_assoc_zval(&ret, "j", &tb_j);

    zval tb_k;
    array_init(&tb_k);
    char* th_k_arr[] = {"ķ", "ĸ", "к", "κ", "Ķ", "ق", "ك", "က", "კ", "ქ",
                        "ک", "ｋ"};
    for(int i = 0; i < 12; i++) {
        add_next_index_string(&tb_k, th_k_arr[i]);
    }
    add_assoc_zval(&ret, "k", &tb_k);

    zval tb_l;
    array_init(&tb_l);
    char* th_l_arr[] = {"ł", "ľ", "ĺ", "ļ", "ŀ", "л", "λ", "ل", "လ", "ლ",
                        "ｌ"};
    for(int i = 0; i < 11; i++) {
        add_next_index_string(&tb_l, th_l_arr[i]);
    }
    add_assoc_zval(&ret, "l", &tb_l);

    zval tb_m;
    array_init(&tb_m);
    char* th_m_arr[] = {"м", "μ", "م", "မ", "მ", "ｍ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_m, th_m_arr[i]);
    }
    add_assoc_zval(&ret, "m", &tb_m);

    zval tb_n;
    array_init(&tb_n);
    char* th_n_arr[] = {"ñ", "ń", "ň", "ņ", "ŉ", "ŋ", "ν", "н", "ن", "န",
                        "ნ", "ｎ"};
    for(int i = 0; i < 12; i++) {
        add_next_index_string(&tb_n, th_n_arr[i]);
    }
    add_assoc_zval(&ret, "n", &tb_n);

    zval tb_o;
    array_init(&tb_o);
    char* th_o_arr[] = {"ó", "ò", "ỏ", "õ", "ọ", "ô", "ố", "ồ", "ổ", "ỗ",
                        "ộ", "ơ", "ớ", "ờ", "ở", "ỡ", "ợ", "ø", "ō", "ő",
                        "ŏ", "ο", "ὀ", "ὁ", "ὂ", "ὃ", "ὄ", "ὅ", "ὸ", "ό",
                        "о", "و", "θ", "ို", "ǒ", "ǿ", "º", "ო", "ओ", "ｏ",
                        "ö"};
    for(int i = 0; i < 41; i++) {
        add_next_index_string(&tb_o, th_o_arr[i]);
    }
    add_assoc_zval(&ret, "o", &tb_o);

    zval tb_p;
    array_init(&tb_p);
    char* th_p_arr[] = {"п", "π", "ပ", "პ", "پ", "ｐ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_p, th_p_arr[i]);
    }
    add_assoc_zval(&ret, "p", &tb_p);

    zval tb_q;
    array_init(&tb_q);
    char* th_q_arr[] = {"ყ", "ｑ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_q, th_q_arr[i]);
    }
    add_assoc_zval(&ret, "q", &tb_q);

    zval tb_r;
    array_init(&tb_r);
    char* th_r_arr[] = {"ŕ", "ř", "ŗ", "р", "ρ", "ر", "რ", "ｒ"};
    for(int i = 0; i < 8; i++) {
        add_next_index_string(&tb_r, th_r_arr[i]);
    }
    add_assoc_zval(&ret, "r", &tb_r);

    zval tb_s;
    array_init(&tb_s);
    char* th_s_arr[] = {"ś", "š", "ş", "с", "σ", "ș", "ς", "س", "ص", "စ",
                        "ſ", "ს", "ｓ"};
    for(int i = 0; i < 13; i++) {
        add_next_index_string(&tb_s, th_s_arr[i]);
    }
    add_assoc_zval(&ret, "s", &tb_s);

    zval tb_tt;
    array_init(&tb_tt);
    char* th_t_arr[] = {"ť", "ţ", "т", "τ", "ț", "ت", "ط", "ဋ", "တ", "ŧ",
                        "თ", "ტ", "ｔ"};
    for(int i = 0; i < 13; i++) {
        add_next_index_string(&tb_tt, th_t_arr[i]);
    }
    add_assoc_zval(&ret, "t", &tb_tt);

    zval tb_u;
    array_init(&tb_u);
    char* th_u_arr[] = {"ú", "ù", "ủ", "ũ", "ụ", "ư", "ứ", "ừ", "ử", "ữ",
                        "ự", "û", "ū", "ů", "ű", "ŭ", "ų", "µ", "у", "ဉ",
                        "ု", "ူ", "ǔ", "ǖ", "ǘ", "ǚ", "ǜ", "უ", "उ", "ｕ",
                        "ў", "ü"};
    for(int i = 0; i < 32; i++) {
        add_next_index_string(&tb_u, th_u_arr[i]);
    }
    add_assoc_zval(&ret, "u", &tb_u);

    zval tb_v;
    array_init(&tb_v);
    char* th_v_arr[] = {"в", "ვ", "ϐ", "ｖ"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_v, th_v_arr[i]);
    }
    add_assoc_zval(&ret, "v", &tb_v);
    
    zval tb_w;
    array_init(&tb_w);
    char* th_w_arr[] = {"ŵ", "ω", "ώ", "ဝ", "ွ", "ｗ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_w, th_w_arr[i]);
    }
    add_assoc_zval(&ret, "w", &tb_w);

    zval tb_x;
    array_init(&tb_x);
    char* th_x_arr[] = {"χ", "ξ", "ｘ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_x, th_x_arr[i]);
    }
    add_assoc_zval(&ret, "x", &tb_x);

    zval tb_y;
    array_init(&tb_y);
    char* th_y_arr[] = {"ý", "ỳ", "ỷ", "ỹ", "ỵ", "ÿ", "ŷ", "й", "ы", "υ",
                        "ϋ", "ύ", "ΰ", "ي", "ယ", "ｙ"};
    for(int i = 0; i < 16; i++) {
        add_next_index_string(&tb_y, th_y_arr[i]);
    }
    add_assoc_zval(&ret, "y", &tb_y);

    zval tb_z;
    array_init(&tb_z);
    char* th_z_arr[] = {"ź", "ž", "ż", "з", "ζ", "ز", "ဇ", "ზ", "ｚ"};
    for(int i = 0; i < 9; i++) {
        add_next_index_string(&tb_z, th_z_arr[i]);
    }
    add_assoc_zval(&ret, "z", &tb_z);

    zval tb_aa;
    array_init(&tb_aa);
    char* th_aa_arr[] = {"ع", "आ", "آ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_aa, th_aa_arr[i]);
    }
    add_assoc_zval(&ret, "aa", &tb_aa);

    zval tb_ae;
    array_init(&tb_ae);
    char* th_ae_arr[] = {"æ", "ǽ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_ae, th_ae_arr[i]);
    }
    add_assoc_zval(&ret, "aa", &tb_ae);

    zval tb_ai;
    array_init(&tb_ai);
    char* th_ai_arr[] = {"ऐ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ai, th_ai_arr[i]);
    }
    add_assoc_zval(&ret, "ai", &tb_ai);

    zval tb_ch;
    array_init(&tb_ch);
    char* th_ch_arr[] = {"ч", "ჩ", "ჭ", "چ"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_ch, th_ch_arr[i]);
    }
    add_assoc_zval(&ret, "ch", &tb_ch);

    zval tb_dj;
    array_init(&tb_dj);
    char* th_dj_arr[] = {"ђ", "đ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_dj, th_dj_arr[i]);
    }
    add_assoc_zval(&ret, "dj", &tb_dj);

    zval tb_dz;
    array_init(&tb_dz);
    char* th_dz_arr[] = {"џ", "ძ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_dz, th_dz_arr[i]);
    }
    add_assoc_zval(&ret, "dz", &tb_dz);

    zval tb_ei;
    array_init(&tb_ei);
    char* th_ei_arr[] = {"ऍ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ei, th_ei_arr[i]);
    }
    add_assoc_zval(&ret, "ei", &tb_ei);

    zval tb_gh;
    array_init(&tb_gh);
    char* th_gh_arr[] = {"غ", "ღ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_gh, th_gh_arr[i]);
    }
    add_assoc_zval(&ret, "gh", &tb_gh);

    zval tb_ii;
    array_init(&tb_ii);
    char* th_ii_arr[] = {"ई"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ii, th_ii_arr[i]);
    }
    add_assoc_zval(&ret, "ii", &tb_ii);

    zval tb_kh;
    array_init(&tb_kh);
    char* th_kh_arr[] = {"х", "خ", "ხ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_kh, th_kh_arr[i]);
    }
    add_assoc_zval(&ret, "kh", &tb_kh);

    zval tb_ij;
    array_init(&tb_ij);
    char* th_ij_arr[] = {"ĳ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ij, th_ij_arr[i]);
    }
    add_assoc_zval(&ret, "ij", &tb_ij);

    zval tb_lj;
    array_init(&tb_lj);
    char* th_lj_arr[] = {"љ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_lj, th_lj_arr[i]);
    }
    add_assoc_zval(&ret, "lj", &tb_lj);

    zval tb_nj;
    array_init(&tb_nj);
    char* th_nj_arr[] = {"њ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_nj, th_nj_arr[i]);
    }
    add_assoc_zval(&ret, "nj", &tb_nj);

    zval tb_oe;
    array_init(&tb_oe);
    char* th_oe_arr[] = {"œ", "ؤ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_oe, th_oe_arr[i]);
    }
    add_assoc_zval(&ret, "oe", &tb_oe);

    zval tb_oi;
    array_init(&tb_oi);
    char* th_oi_arr[] = {"ऑ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_oi, th_oi_arr[i]);
    }
    add_assoc_zval(&ret, "oi", &tb_oi);

    zval tb_oii;
    array_init(&tb_oii);
    char* th_oii_arr[] = {"ऒ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_oii, th_oii_arr[i]);
    }
    add_assoc_zval(&ret, "oii", &tb_oii);

    zval tb_ps;
    array_init(&tb_ps);
    char* th_ps_arr[] = {"ψ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ps, th_ps_arr[i]);
    }
    add_assoc_zval(&ret, "ps", &tb_ps);

    zval tb_sh;
    array_init(&tb_sh);
    char* th_sh_arr[] = {"ш", "შ", "ش"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_sh, th_sh_arr[i]);
    }
    add_assoc_zval(&ret, "sh", &tb_sh);

    zval tb_shch;
    array_init(&tb_shch);
    char* th_shch_arr[] = {"щ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_shch, th_shch_arr[i]);
    }
    add_assoc_zval(&ret, "shch", &tb_shch);

    zval tb_ss;
    array_init(&tb_ss);
    char* th_ss_arr[] = {"ß"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ss, th_ss_arr[i]);
    }
    add_assoc_zval(&ret, "ss", &tb_ss);
    
    zval tb_sx;
    array_init(&tb_sx);
    char* th_sx_arr[] = {"ŝ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_sx, th_sx_arr[i]);
    }
    add_assoc_zval(&ret, "sx", &tb_sx);

    zval tb_th;
    array_init(&tb_th);
    char* th_th_arr[] = {"þ", "ϑ", "ث", "ذ", "ظ"};
    for(int i = 0; i < 5; i++) {
        add_next_index_string(&tb_th, th_th_arr[i]);
    }
    add_assoc_zval(&ret, "th", &tb_th);

    zval tb_ts;
    array_init(&tb_ts);
    char* th_ts_arr[] = {"ц", "ც", "წ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_ts, th_ts_arr[i]);
    }
    add_assoc_zval(&ret, "ts", &tb_ts);

    zval tb_uu;
    array_init(&tb_uu);
    char* th_uu_arr[] = {"ऊ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_uu, th_uu_arr[i]);
    }
    add_assoc_zval(&ret, "uu", &tb_uu);

    zval tb_ya;
    array_init(&tb_ya);
    char* th_ya_arr[] = {"я"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ya, th_ya_arr[i]);
    }
    add_assoc_zval(&ret, "ya", &tb_ya);

    zval tb_yu;
    array_init(&tb_yu);
    char* th_yu_arr[] = {"ю"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_yu, th_yu_arr[i]);
    }
    add_assoc_zval(&ret, "yu", &tb_yu);

    zval tb_zh;
    array_init(&tb_zh);
    char* th_zh_arr[] = {"ж", "ჟ", "ژ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_zh, th_zh_arr[i]);
    }
    add_assoc_zval(&ret, "zh", &tb_zh);

    zval tb_ccc;
    array_init(&tb_ccc);
    char* th_ccc_arr[] = {"©"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_ccc, th_ccc_arr[i]);
    }
    add_assoc_zval(&ret, "(c)", &tb_ccc);

    zval tb_A;
    array_init(&tb_A);
    char* th_A_arr[] = {"Á", "À", "Ả", "Ã", "Ạ", "Ă", "Ắ", "Ằ", "Ẳ", "Ẵ",
                        "Ặ", "Â", "Ấ", "Ầ", "Ẩ", "Ẫ", "Ậ", "Å", "Ā", "Ą",
                        "Α", "Ά", "Ἀ", "Ἁ", "Ἂ", "Ἃ", "Ἄ", "Ἅ", "Ἆ", "Ἇ",
                        "ᾈ", "ᾉ", "ᾊ", "ᾋ", "ᾌ", "ᾍ", "ᾎ", "ᾏ", "Ᾰ", "Ᾱ",
                        "Ὰ", "Ά", "ᾼ", "А", "Ǻ", "Ǎ", "Ａ", "Ä"};
    for(int i = 0; i < 48; i++) {
        add_next_index_string(&tb_A, th_A_arr[i]);
    }
    add_assoc_zval(&ret, "A", &tb_A);

    zval tb_B;
    array_init(&tb_B);
    char* th_B_arr[] = {"Б", "Β", "ब", "Ｂ"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_B, th_B_arr[i]);
    }
    add_assoc_zval(&ret, "B", &tb_B);

    zval tb_C;
    array_init(&tb_C);
    char* th_C_arr[] = {"Ç", "Ć", "Č", "Ĉ", "Ċ", "Ｃ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_C, th_C_arr[i]);
    }
    add_assoc_zval(&ret, "C", &tb_C);
    
    zval tb_D;
    array_init(&tb_D);
    char* th_D_arr[] = {"Ď", "Ð", "Đ", "Ɖ", "Ɗ", "Ƌ", "ᴅ", "ᴆ", "Д", "Δ",
                        "Ｄ"};
    for(int i = 0; i < 11; i++) {
        add_next_index_string(&tb_D, th_D_arr[i]);
    }
    add_assoc_zval(&ret, "D", &tb_D);

    zval tb_E;
    array_init(&tb_E);
    char* th_E_arr[] = {"É", "È", "Ẻ", "Ẽ", "Ẹ", "Ê", "Ế", "Ề", "Ể", "Ễ",
                        "Ệ", "Ë", "Ē", "Ę", "Ě", "Ĕ", "Ė", "Ε", "Έ", "Ἐ",
                        "Ἑ", "Ἒ", "Ἓ", "Ἔ", "Ἕ", "Έ", "Ὲ", "Е", "Ё", "Э",
                        "Є", "Ə", "Ｅ"};
    for(int i = 0; i < 33; i++) {
        add_next_index_string(&tb_E, th_E_arr[i]);
    }
    add_assoc_zval(&ret, "E", &tb_E);

    zval tb_F;
    array_init(&tb_F);
    char* th_F_arr[] = {"Ф", "Φ", "Ｆ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_F, th_F_arr[i]);
    }
    add_assoc_zval(&ret, "F", &tb_F);

    zval tb_G;
    array_init(&tb_G);
    char* th_G_arr[] = {"Ğ", "Ġ", "Ģ", "Г", "Ґ", "Γ", "Ｇ"};
    for(int i = 0; i < 7; i++) {
        add_next_index_string(&tb_G, th_G_arr[i]);
    }
    add_assoc_zval(&ret, "G", &tb_G);

    zval tb_H;
    array_init(&tb_H);
    char* th_H_arr[] = {"Η", "Ή", "Ħ", "Ｈ"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_H, th_H_arr[i]);
    }
    add_assoc_zval(&ret, "H", &tb_H);

    zval tb_I;
    array_init(&tb_I);
    char* th_I_arr[] = {"Í", "Ì", "Ỉ", "Ĩ", "Ị", "Î", "Ï", "Ī", "Ĭ", "Į",
                        "İ", "Ι", "Ί", "Ϊ", "Ἰ", "Ἱ", "Ἳ", "Ἴ", "Ἵ", "Ἶ",
                        "Ἷ", "Ῐ", "Ῑ", "Ὶ", "Ί", "И", "І", "Ї", "Ǐ", "ϒ",
                        "Ｉ"};
    for(int i = 0; i < 31; i++) {
        add_next_index_string(&tb_I, th_I_arr[i]);
    }
    add_assoc_zval(&ret, "I", &tb_I);

    zval tb_J;
    array_init(&tb_J);
    char* th_J_arr[] = {"Ｊ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_J, th_J_arr[i]);
    }
    add_assoc_zval(&ret, "J", &tb_J);

    zval tb_K;
    array_init(&tb_K);
    char* th_K_arr[] = {"К", "Κ", "Ｋ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_K, th_K_arr[i]);
    }
    add_assoc_zval(&ret, "K", &tb_K);

    zval tb_L;
    array_init(&tb_L);
    char* th_L_arr[] = {"Ĺ", "Ł", "Л", "Λ", "Ļ", "Ľ", "Ŀ", "ल", "Ｌ"};
    for(int i = 0; i < 9; i++) {
        add_next_index_string(&tb_L, th_L_arr[i]);
    }
    add_assoc_zval(&ret, "L", &tb_L);

    zval tb_M;
    array_init(&tb_M);
    char* th_M_arr[] = {"М", "Μ", "Ｍ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_M, th_M_arr[i]);
    }
    add_assoc_zval(&ret, "M", &tb_M);

    zval tb_N;
    array_init(&tb_N);
    char* th_N_arr[] = {"Ń", "Ñ", "Ň", "Ņ", "Ŋ", "Н", "Ν", "Ｎ"};
    for(int i = 0; i < 8; i++) {
        add_next_index_string(&tb_N, th_N_arr[i]);
    }
    add_assoc_zval(&ret, "N", &tb_N);

    zval tb_O;
    array_init(&tb_O);
    char* th_O_arr[] = {"Ó", "Ò", "Ỏ", "Õ", "Ọ", "Ô", "Ố", "Ồ", "Ổ", "Ỗ",
                        "Ộ", "Ơ", "Ớ", "Ờ", "Ở", "Ỡ", "Ợ", "Ø", "Ō", "Ő",
                        "Ŏ", "Ο", "Ό", "Ὀ", "Ὁ", "Ὂ", "Ὃ", "Ὄ", "Ὅ", "Ὸ",
                        "Ό", "О", "Θ", "Ө", "Ǒ", "Ǿ", "Ｏ", "Ö"};
    for(int i = 0; i < 38; i++) {
        add_next_index_string(&tb_O, th_O_arr[i]);
    }
    add_assoc_zval(&ret, "O", &tb_O);

    zval tb_P;
    array_init(&tb_P);
    char* th_P_arr[] = {"П", "Π", "Ｐ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_P, th_P_arr[i]);
    }
    add_assoc_zval(&ret, "P", &tb_P);

    zval tb_Q;
    array_init(&tb_Q);
    char* th_Q_arr[] = {"Ｑ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Q, th_Q_arr[i]);
    }
    add_assoc_zval(&ret, "Q", &tb_Q);

    zval tb_R;
    array_init(&tb_R);
    char* th_R_arr[] = {"Ř", "Ŕ", "Р", "Ρ", "Ŗ", "Ｒ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_R, th_R_arr[i]);
    }
    add_assoc_zval(&ret, "R", &tb_R);

    zval tb_S;
    array_init(&tb_S);
    char* th_S_arr[] = {"Ş", "Ŝ", "Ș", "Š", "Ś", "С", "Σ", "Ｓ"};
    for(int i = 0; i < 8; i++) {
        add_next_index_string(&tb_S, th_S_arr[i]);
    }
    add_assoc_zval(&ret, "S", &tb_S);

    zval tb_T;
    array_init(&tb_T);
    char* th_T_arr[] = {"Ť", "Ţ", "Ŧ", "Ț", "Т", "Τ", "Ｔ"};
    for(int i = 0; i < 7; i++) {
        add_next_index_string(&tb_T, th_T_arr[i]);
    }
    add_assoc_zval(&ret, "T", &tb_T);

    zval tb_U;
    array_init(&tb_U);
    char* th_U_arr[] = {"Ú", "Ù", "Ủ", "Ũ", "Ụ", "Ư", "Ứ", "Ừ", "Ử", "Ữ",
                        "Ự", "Û", "Ū", "Ů", "Ű", "Ŭ", "Ų", "У", "Ǔ", "Ǖ",
                        "Ǘ", "Ǚ", "Ǜ", "Ｕ", "Ў", "Ü"};
    for(int i = 0; i < 26; i++) {
        add_next_index_string(&tb_U, th_U_arr[i]);
    }
    add_assoc_zval(&ret, "U", &tb_U);

    zval tb_V;
    array_init(&tb_V);
    char* th_V_arr[] = {"В", "Ｖ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_V, th_V_arr[i]);
    }
    add_assoc_zval(&ret, "V", &tb_V);

    zval tb_W;
    array_init(&tb_W);
    char* th_W_arr[] = {"Ω", "Ώ", "Ŵ", "Ｗ"};
    for(int i = 0; i < 4; i++) {
        add_next_index_string(&tb_W, th_W_arr[i]);
    }
    add_assoc_zval(&ret, "W", &tb_W);

    zval tb_X;
    array_init(&tb_X);
    char* th_X_arr[] = {"Χ", "Ξ", "Ｘ"};
    for(int i = 0; i < 3; i++) {
        add_next_index_string(&tb_X, th_X_arr[i]);
    }
    add_assoc_zval(&ret, "X", &tb_X);

    zval tb_Y;
    array_init(&tb_Y);
    char* th_Y_arr[] = {"Ý", "Ỳ", "Ỷ", "Ỹ", "Ỵ", "Ÿ", "Ῠ", "Ῡ", "Ὺ", "Ύ",
                        "Ы", "Й", "Υ", "Ϋ", "Ŷ", "Ｙ"};
    for(int i = 0; i < 16; i++) {
        add_next_index_string(&tb_Y, th_Y_arr[i]);
    }
    add_assoc_zval(&ret, "Y", &tb_Y);

    zval tb_Z;
    array_init(&tb_Z);
    char* th_Z_arr[] = {"Ź", "Ž", "Ż", "З", "Ζ", "Ｚ"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_Z, th_Z_arr[i]);
    }
    add_assoc_zval(&ret, "Z", &tb_Z);

    zval tb_AE;
    array_init(&tb_AE);
    char* th_AE_arr[] = {"Æ", "Ǽ"};
    for(int i = 0; i < 2; i++) {
        add_next_index_string(&tb_AE, th_AE_arr[i]);
    }
    add_assoc_zval(&ret, "AE", &tb_AE);

    zval tb_Ch;
    array_init(&tb_Ch);
    char* th_Ch_arr[] = {"Ч"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Ch, th_Ch_arr[i]);
    }
    add_assoc_zval(&ret, "Ch", &tb_Ch);

    zval tb_Dj;
    array_init(&tb_Dj);
    char* th_Dj_arr[] = {"Ђ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Dj, th_Dj_arr[i]);
    }
    add_assoc_zval(&ret, "Dj", &tb_Dj);

    zval tb_Dz;
    array_init(&tb_Dz);
    char* th_Dz_arr[] = {"Џ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Dz, th_Dz_arr[i]);
    }
    add_assoc_zval(&ret, "Dz", &tb_Dz);

    zval tb_Gx;
    array_init(&tb_Gx);
    char* th_Gx_arr[] = {"Ĝ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Gx, th_Gx_arr[i]);
    }
    add_assoc_zval(&ret, "Gx", &tb_Gx);

    zval tb_Hx;
    array_init(&tb_Hx);
    char* th_Hx_arr[] = {"Ĥ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Hx, th_Hx_arr[i]);
    }
    add_assoc_zval(&ret, "Hx", &tb_Hx);

    zval tb_Ij;
    array_init(&tb_Ij);
    char* th_Ij_arr[] = {"Ĳ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Ij, th_Ij_arr[i]);
    }
    add_assoc_zval(&ret, "Ij", &tb_Ij);

    zval tb_Jx;
    array_init(&tb_Jx);
    char* th_Jx_arr[] = {"Ĵ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Jx, th_Jx_arr[i]);
    }
    add_assoc_zval(&ret, "Jx", &tb_Jx);

    zval tb_Kh;
    array_init(&tb_Kh);
    char* th_Kh_arr[] = {"Х"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Kh, th_Kh_arr[i]);
    }
    add_assoc_zval(&ret, "Kh", &tb_Kh);

    zval tb_Lj;
    array_init(&tb_Lj);
    char* th_Lj_arr[] = {"Љ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Lj, th_Lj_arr[i]);
    }
    add_assoc_zval(&ret, "Lj", &tb_Lj);

    zval tb_Nj;
    array_init(&tb_Nj);
    char* th_Nj_arr[] = {"Њ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Nj, th_Nj_arr[i]);
    }
    add_assoc_zval(&ret, "Nj", &tb_Nj);

    zval tb_Oe;
    array_init(&tb_Oe);
    char* th_Oe_arr[] = {"Œ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Oe, th_Oe_arr[i]);
    }
    add_assoc_zval(&ret, "Oe", &tb_Oe);

    zval tb_Ps;
    array_init(&tb_Ps);
    char* th_Ps_arr[] = {"Ψ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Ps, th_Ps_arr[i]);
    }
    add_assoc_zval(&ret, "Ps", &tb_Ps);

    zval tb_Sh;
    array_init(&tb_Sh);
    char* th_Sh_arr[] = {"Ш"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Sh, th_Sh_arr[i]);
    }
    add_assoc_zval(&ret, "Sh", &tb_Sh);

    zval tb_Shch;
    array_init(&tb_Shch);
    char* th_Shch_arr[] = {"Щ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Shch, th_Shch_arr[i]);
    }
    add_assoc_zval(&ret, "Shch", &tb_Shch);

    zval tb_Ss;
    array_init(&tb_Ss);
    char* th_Ss_arr[] = {"ẞ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Ss, th_Ss_arr[i]);
    }
    add_assoc_zval(&ret, "Ss", &tb_Ss);

    zval tb_Th;
    array_init(&tb_Th);
    char* th_Th_arr[] = {"Þ"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Th, th_Th_arr[i]);
    }
    add_assoc_zval(&ret, "Th", &tb_Th);

    zval tb_Ts;
    array_init(&tb_Ts);
    char* th_Ts_arr[] = {"Ц"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Ts, th_Ts_arr[i]);
    }
    add_assoc_zval(&ret, "Ts", &tb_Ts);

    zval tb_Ya;
    array_init(&tb_Ya);
    char* th_Ya_arr[] = {"Я"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Ya, th_Ya_arr[i]);
    }
    add_assoc_zval(&ret, "Ya", &tb_Ya);

    zval tb_Yu;
    array_init(&tb_Yu);
    char* th_Yu_arr[] = {"Yu"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Yu, th_Yu_arr[i]);
    }
    add_assoc_zval(&ret, "Yu", &tb_Yu);

    zval tb_Zh;
    array_init(&tb_Zh);
    char* th_Zh_arr[] = {"Ж"};
    for(int i = 0; i < 1; i++) {
        add_next_index_string(&tb_Zh, th_Zh_arr[i]);
    }
    add_assoc_zval(&ret, "Zh", &tb_Zh);

    zval tb_str;
    array_init(&tb_str);
    char* th_str_arr[] = {"\xC2\xA0", "\xE2\x80\x80", "\xE2\x80\x81",
                        "\xE2\x80\x82", "\xE2\x80\x83", "\xE2\x80\x84",
                        "\xE2\x80\x85", "\xE2\x80\x86", "\xE2\x80\x87",
                        "\xE2\x80\x88", "\xE2\x80\x89", "\xE2\x80\x8A",
                        "\xE2\x80\xAF", "\xE2\x81\x9F", "\xE3\x80\x80",
                        "\xEF\xBE\xA0"};
    for(int i = 0; i < 16; i++) {
        add_next_index_string(&tb_str, th_str_arr[i]);
    }
    add_assoc_zval(&ret, " ", &tb_str);

    RETURN_ZVAL(&ret, 0, 0);
}

PHP_METHOD(Stringy, langSpecificCharsArray)
{
    zval *language;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(language)
    ZEND_PARSE_PARAMETERS_END();

    if (language == NULL) {
        language = malloc(sizeof(zval));
        ZVAL_STRING(language, "en");
    }

    zval reg;
    ZVAL_STRING(&reg, "/[-_]/");
    zval func, args[] = {
        reg,
        *language,
    };
    ZVAL_STRING(&func, "preg_split");
    call_user_function(NULL, NULL, &func, return_value, 2, args);

    zval args_strtolower[] = {
        *zend_hash_index_find(Z_ARRVAL_P(return_value), (zend_long) 0),
    };
    ZVAL_STRING(&func, "strtolower");
    call_user_function(NULL, NULL, &func, return_value, 1, args_strtolower);

    zval languageSpecific;
    array_init(&languageSpecific);

    zval tb_0;
    array_init(&tb_0);
    char* th_0_arr[6] = {"ä", "ö", "ü", "Ä", "Ö", "Ü"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_0, th_0_arr[i]);
    }

    zval tb_1;
    array_init(&tb_1);
    char* th_1_arr[6] = {"ae", "oe", "ue", "AE", "OE", "UE"};
    for(int i = 0; i < 6; i++) {
        add_next_index_string(&tb_1, th_1_arr[i]);
    }

    zval val_0;
    array_init(&val_0);
    add_next_index_zval(&val_0, &tb_0);
    add_next_index_zval(&val_0, &tb_1);

    add_assoc_zval(&languageSpecific, "de", &val_0);

    zval tb_1_0;
    array_init(&tb_1_0);
    char* th_1_0_arr[8] = {"х", "Х", "щ", "Щ", "ъ", "Ъ", "ь", "Ь"};
    for(int i = 0; i < 8; i++) {
        add_next_index_string(&tb_1_0, th_1_0_arr[i]);
    }

    zval tb_1_1;
    array_init(&tb_1_1);
    char* th_1_1_arr[8] = {"h", "H", "sht", "SHT", "a", "А", "y", "Y"};
    for(int i = 0; i < 8; i++) {
        add_next_index_string(&tb_1_1, th_1_1_arr[i]);
    }

    zval val_1;
    array_init(&val_1);
    add_next_index_zval(&val_1, &tb_0);
    add_next_index_zval(&val_1, &tb_1);

    add_assoc_zval(&languageSpecific, "bg", &val_1);

    zval ret;
    zend_string *key = Z_STR_P(return_value);
    if (zend_hash_exists(Z_ARRVAL(languageSpecific), key)) {
        ret = *zend_hash_find(Z_ARR(languageSpecific), key);
    } else {
        array_init(&ret);
    }
    
    RETURN_ZVAL(&ret, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_langSpecificCharsArray, 0)
    ZEND_ARG_INFO(0, language)
ZEND_END_ARG_INFO();

PHP_METHOD(Stringy, toSpaces)
{
    zval * tabLength;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_ZVAL(tabLength)
    ZEND_PARSE_PARAMETERS_END();

    zval rv;
    zval * str = zend_read_property(stringy_ce, getThis(), ZEND_STRL("str"), 0, &rv);
    zval * encoding = zend_read_property(stringy_ce, getThis(), ZEND_STRL("encoding"), 1, &rv);

    zval tmp_str;
    ZVAL_STRING(&tmp_str, " ");

    convert_to_long(tabLength);
    zval func, args[] = {
        tmp_str,
        *tabLength,
    };
    ZVAL_STRING(&func, "str_repeat");
    call_user_function(NULL, NULL, &func, return_value, 2, args);

    ZVAL_STRING(&tmp_str, "\t");
    zval args_str_replace[] = {
        tmp_str,
        *return_value,
        *str,
    };
    ZVAL_STRING(&func, "str_replace");
    call_user_function(NULL, NULL, &func, return_value, 3, args_str_replace);

    zval instance;
    object_init_ex(&instance, stringy_ce);

    zval args_construct[] = {
        *return_value,
        *encoding,
    };
    ZVAL_STRING(&func, "__construct");
    call_user_function(NULL, &instance, &func, return_value, 2, args_construct);

    RETURN_ZVAL(&instance, 0, 1);
}
ZEND_BEGIN_ARG_INFO(arginfo_toSpaces, 0)
    ZEND_ARG_INFO(0, tabLength)
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
    PHP_ME(Stringy, startsWith, arginfo_startsWith, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, endsWithAny, arginfo_endsWithAny, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, ensureLeft, arginfo_ensureLeft, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, ensureRight, arginfo_ensureRight, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, first, arginfo_first, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, last, arginfo_last, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isAlpha, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isAlphanumeric, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isHexadecimal, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isLowerCase, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isUpperCase, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, matchesPattern, arginfo_matchesPattern, ZEND_ACC_PROTECTED)
    PHP_ME(Stringy, isBlank, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, hasLowerCase, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, hasUpperCase, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, htmlEncode, arginfo_htmlEncode, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, htmlDecode, arginfo_htmlDecode, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, removeLeft, arginfo_removeLeft, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, removeRight, arginfo_removeRight, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, humanize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, insert, arginfo_insert, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isJson, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isSerialized, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, isBase64, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, longestCommonPrefix, arginfo_longestCommonPrefix, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, longestCommonSuffix, arginfo_longestCommonSuffix, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, longestCommonSubstring, arginfo_longestCommonSubstring, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, pad, arginfo_pad, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, padLeft, arginfo_padLeft, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, padRight, arginfo_padRight, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, padBoth, arginfo_padBoth, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, applyPadding, arginfo_applyPadding, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, repeat, arginfo_repeat, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, replace, arginfo_replace, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, toAscii, arginfo_toAscii, ZEND_ACC_PUBLIC)
    PHP_ME(Stringy, charsArray, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Stringy, langSpecificCharsArray, arginfo_langSpecificCharsArray, ZEND_ACC_PROTECTED)
    PHP_ME(Stringy, toSpaces, arginfo_toSpaces, ZEND_ACC_PUBLIC)
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
        spl_ce_ArrayAccess
    );
}
