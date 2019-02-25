dnl $Id$
dnl config.m4 for extension startup

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(startup, for startup support,
dnl Make sure that the comment is aligned:
dnl [  --with-startup             Include startup support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(startup, whether to enable startup support,
dnl Make sure that the comment is aligned:
[  --enable-startup           Enable startup support])

if test -z "$PHP_DEBUG"; then
  AC_ARG_ENABLE(debug,
    [--enable-debug  compile with debugging system],
    [PHP_DEBUG=$enableval], [PHP_DEBUG=no]
  )
fi

if test "$PHP_STARTUP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-startup -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/startup.h"  # you most likely want to change this
  dnl if test -r $PHP_STARTUP/$SEARCH_FOR; then # path given as parameter
  dnl   STARTUP_DIR=$PHP_STARTUP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for startup files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       STARTUP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$STARTUP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the startup distribution])
  dnl fi

  dnl # --with-startup -> add include path
  dnl PHP_ADD_INCLUDE($STARTUP_DIR/include)

  dnl # --with-startup -> check for lib and symbol presence
  dnl LIBNAME=startup # you may want to change this
  dnl LIBSYMBOL=startup # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $STARTUP_DIR/$PHP_LIBDIR, STARTUP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_STARTUPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong startup lib version or lib not found])
  dnl ],[
  dnl   -L$STARTUP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(STARTUP_SHARED_LIBADD)

  source="startup.c \
              src/ext/functions.c \
              src/ext/test.class.c \ 
              src/ext/stringy/stringy.c";

  PHP_NEW_EXTENSION(startup, $source, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

  dnl configure can't use ".." as a source filename, so we make a link here
  # ln -sf $ext_srcdir/../common $ext_srcdir
  # ln -sf $ext_srcdir/../deps $ext_srcdir
  # ln -sf ./thirdParty/rxi-log include/rxi-log

  dnl add common include path
  PHP_ADD_INCLUDE(thirdParty/rxi-log/src)

  # PHP_ADD_SOURCES(thirdParty/rxi-log/src)

  PHP_ADD_MAKEFILE_FRAGMENT
fi

if test "$PHP_DEBUG" != "no"; then
    dnl 是，则设置 C 语言宏指令
    AC_DEFINE(PHP_DEBUG, 1, [Include debugging support in ext1])
    AC_DEFINE(PHP_EXT_DEBUG, 1, [Include debugging support in ext])
  fi


