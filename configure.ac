dnl configure.ac for spdrs60
dnl Guido Scholz
dnl last update: $Date: 2008-08-06 20:44:04 $
dnl version: $Revision: 1.23 $
dnl Process this file with autoconf to produce a configure script.

AC_INIT([spdrs60],[0.5.3],[Guido Scholz <guido.scholz@bayernline.de>])
AC_CONFIG_SRCDIR([spdrs60/main.cpp])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([dist-bzip2])


AC_PREFIX_DEFAULT(/usr/local)
if test "x$prefix" = "xNONE"; then
  prefix=$ac_default_prefix
  ac_configure_args="$ac_configure_args --prefix $prefix"
fi

dnl generate the config header
AC_CONFIG_HEADERS([spdrs60/config.h])

dnl if test "x$CFLAGS" = "x"; then CFLAGS="-Wall"; fi

dnl Checks for programs
AC_PROG_CXX


dnl option to skip html doc generation 
AC_ARG_ENABLE(html-doc, AC_HELP_STRING([--disable-html-doc],
            [disable building and installation of html documentation]),, enable_html_doc=yes)

AM_CONDITIONAL(HTML_DOC, test x${enable_html_doc} = xyes)
if test x$enable_html_doc = xyes; then

    dnl check for jade or openjade to generate html documentation
  AC_PATH_PROGS(JADE, jade openjade, no)
  if test "x$JADE" = "xno" ; then
    AC_MSG_ERROR(
      [cannot find 'jade' or 'openjade', please install the adequate package.])
  fi

fi

dnl AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])
all_libraries="$all_libraries $USER_LDFLAGS"
all_includes="$all_includes $USER_INCLUDES"
AC_SUBST(all_includes)
AC_SUBST(all_libraries)

AC_PATH_X
KDE_USE_QT(3.0)
AC_PATH_QT

dnl TODO: check for QTDIR

dnl check for lupdate
AC_PATH_PROG(LUPDATE, lupdate, no, [$PATH:$QTDIR/bin])
if test "x$LUPDATE" = "xno" ; then
  AC_MSG_ERROR(
    [cannot find 'lupdate', please install the Qt development tools package.])
fi

dnl check for lrelease
AC_PATH_PROG(LRELEASE, lrelease, no, [$PATH:$QTDIR/bin])
if test "x$LRELEASE" = "xno" ; then
  AC_MSG_ERROR(
    [cannot find 'lrelease', please install the Qt development tools package.])
else
  AC_MSG_CHECKING([for lrelease version])
  LRELEASE_VERSION=`$LRELEASE -version 2>&1 | awk '{print $3}'`
  AC_SUBST(LRELEASE_VERSION)
  AC_MSG_RESULT([$LRELEASE_VERSION])
    
  AC_MSG_CHECKING([whether lrelease supports -qm])
  lrelease_mqline=`$LRELEASE -help 2>&1| sed -n '/-qm/p'`
  if test -z "$lrelease_mqline"; then
    lrelease_qm=no 
  else
    lrelease_qm=yes
  fi
  AC_MSG_RESULT([$lrelease_qm])
fi
AM_CONDITIONAL(LRELEASE_QM, test x$lrelease_qm = xyes)


AC_MSG_CHECKING([for Qt translations])
    qt_dirs="$QTDIR /usr/lib/qt3 /usr/lib/qt /usr/share/qt3"
    for dir in $qt_dirs
    do
       if test -d "$dir/translations"; then
          qt_translations_dir="$dir/translations"
          translate_qt=yes
          break
       else
          translate_qt=no
       fi
    done
    if test x$translate_qt = xyes; then
       AC_MSG_RESULT([$qt_translations_dir])
       QT_TRANSLATIONS_DIR=$qt_translations_dir
       AC_SUBST(QT_TRANSLATIONS_DIR)
    else
       AC_MSG_WARN([no translations available])
    fi
AM_CONDITIONAL(TRANSLATE_QT, test x${translate_qt} = xyes)


AC_CHECK_LIB(m, roundf,,AC_MSG_ERROR([required library missing]))

AC_CHECK_HEADERS(signal.h math.h,,AC_MSG_ERROR([required header file missing]))


if test x$enable_html_doc = xyes; then
  dnl this dsssl check is (modified) from the bonsai project
  AC_MSG_CHECKING(for docbook.dsl)
  AC_ARG_WITH(dsssl, AC_HELP_STRING([--with-dsssl=[DIR]],
              [Look for DSSSL stylesheets in the specified directory]),
   [
     if test -d "$withval" ; then
   	DOCBOOK_HTML=$withval/html/docbook.dsl
          DOCBOOK_PRINT=$withval/print/docbook.dsl
          AC_MSG_RESULT(in $withval)
     fi
   ],[
     for dir in \
          /usr/share/sgml/docbook/dsssl-stylesheets \
          /usr/share/sgml/docbook/stylesheet/dsssl/modular \
          /usr/share/sgml/stylesheets/docbook \
          /usr/share/sgml/docbk30/dsl \
          /usr/share/sgml/docbkdsl \
          /usr/lib/dsssl/stylesheets/docbook \
          /usr/lib/dsssl/stylesheets/nwalsh-modular \
          /usr/lib/dsssl/stylesheets/docbook \
          /usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh \
          /usr/lib/sgml/stylesheets/nwalsh-modular \
          /usr/lib/sgml/docbook\
          /usr/lib/sgml/stylesheets/docbook \
          /opt/sgml/lib/stylesheets/docbook \
          /usr/local/lib/dsssl/stylesheets/docbook \
          /usr/local/lib/dsssl/stylesheets/nwalsh-modular \
          /usr/local/lib/sgml/stylesheets/docbook \
          /usr/local/lib/sgml/stylesheets/nwalsh-modular \
          /usr/local/lib/sgml/docbook \
          /usr/local/share/sgml/docbook/stylesheet/dsssl/modular \
          /usr/local/share/sgml/docbook/dsssl-stylesheets \
          /usr/local/share/sgml/stylesheets/docbook \
          /usr/local/share/sgml/docbook/dsssl/modular
     do
    	if test -f "$dir/html/docbook.dsl"; then
              DOCBOOK_HTML="$dir/html/docbook.dsl"
              DOCBOOK_PRINT="$dir/print/docbook.dsl"
              AC_MSG_RESULT(detected: $dir)
              break
  	fi
     done
  ])
  if test -z "$DOCBOOK_HTML"; then
      AC_MSG_ERROR([DSSSL not found, please install your dsssl-stylesheet package])
  fi
  AC_SUBST(DOCBOOK_HTML)
  AC_SUBST(DOCBOOK_PRINT)
fi
    
AC_ARG_WITH(doc-dir,
  [AC_HELP_STRING([--with-doc-dir=DIR],
    [directory for documentation (e.g. html files)])],
  [ if test "x$withval" != "x" ; then
     DOC_DIR="$withval"
    fi ],
  [DOC_DIR="${prefix}/share/doc"])

dnl define this variable for Makefiles
AC_SUBST(DOC_DIR)
dnl define this variable for source files (config.h)
AC_DEFINE_UNQUOTED([DOC_DIR], ["$DOC_DIR"], \
  [directory for package documentation])


HTML_DOC_DIR="$DOC_DIR/$PACKAGE/html"
AC_SUBST(HTML_DOC_DIR)
AC_DEFINE_UNQUOTED([HTML_DOC_DIR], ["$HTML_DOC_DIR"], \
  [directory html package documentation in html])


AC_DEFINE_UNQUOTED([RES_DIR], ["${prefix}/share/$PACKAGE/"], \
  [directory for shared data])

AC_ARG_WITH(icon-dir,
  [AC_HELP_STRING([--with-icon-dir=DIR],
    [directory for icons])],
  [ if test "x$withval" != "x" ; then
     ICONDIR="$withval"
    fi ],
  [ICONDIR="${prefix}/share/pixmaps"])
AC_SUBST(ICONDIR)
AC_DEFINE_UNQUOTED([ICONDIR], ["$ICONDIR"], [directory for icons])


RES_DIR_LAYT="${prefix}/share/$PACKAGE/layouts/"
AC_SUBST(RES_DIR_LAYT)
AC_DEFINE_UNQUOTED([RES_DIR_LAYT], ["$RES_DIR_LAYT"], \
  [directory for layouts])

dnl add here all your Makefiles. This are created by configure
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([clock/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_CONFIG_FILES([icons/Makefile])
AC_CONFIG_FILES([layouts/Makefile])
AC_CONFIG_FILES([man/Makefile])
AC_CONFIG_FILES([man/de/Makefile])
AC_CONFIG_FILES([spdrs60/Makefile])
AC_CONFIG_FILES([spdrs60/pixmaps/Makefile])
AC_OUTPUT

