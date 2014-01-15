AC_INIT([ratlang], [0.1], [roger@rogerhub-dot-com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CC
AM_PROG_LEX
if test "x$LEX" != xflex; then
  AC_MSG_ERROR([Please install flex before installing.])
fi
AC_PROG_YACC
AC_CHECK_HEADER([gmp.h], , [AC_MSG_ERROR([Cannot find gmp.h])])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
