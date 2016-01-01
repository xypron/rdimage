dnl Crevice

dnl Initialization
AC_INIT([rdimage], [0.1], [xypron.glpk@gmx.de])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

dnl Compiler check
AC_PROG_CC
AC_CHECK_PROG([have_cc],[$CC],[yes],[no])
if test [$have_cc] != [yes]; then
  AC_MSG_ERROR([$CC is missing])
fi

dnl Configure macros
AC_CONFIG_MACRO_DIR([m4])

dnl Create config.h
AC_CONFIG_HEADERS([config.h])

dnl Check for programs needed

dnl Set compiler flags
    
CPPFLAGS="-O2 $CPPFLAGS"

CPPFLAGS="-I`cd $srcdir && pwd`/src $CPPFLAGS"

AC_MSG_NOTICE([CPPFLAGS = $CPPFLAGS])
AC_MSG_NOTICE([LDFLAGS  = $LDFLAGS])

dnl Makefiles
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  ])

dnl Generate files
AC_OUTPUT
