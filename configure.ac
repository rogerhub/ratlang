AC_INIT([ratlang], [0.1], [roger@rogerhub-dot-com])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE
AC_PROG_CC
AM_PROG_LEX
if test "x$LEX" != xflex; then
  AC_MSG_ERROR([Please install flex before installing.])
fi
AC_PROG_YACC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
