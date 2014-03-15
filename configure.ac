dnl configure.ac for spdrs60
dnl (C) 2014 Guido Scholz
dnl Process this file with autoconf to produce a configure script.

AC_INIT([spdrs60],[0.5.7],[Guido Scholz <guido-scholz@gmx.net>])
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

dnl Preset CXX compiler flags if no user setting is available
if test "x$CXXFLAGS" = "x"; then 
    CXXFLAGS="-g -O2 -Wall -Wno-long-long -pedantic"
fi

dnl Checks for programs
AC_PROG_CXX
AC_PROG_CC
AC_LANG_CPLUSPLUS

dnl check for math library
AC_CHECK_LIB(m, roundf,,AC_MSG_ERROR([required library missing]))
AC_CHECK_LIB(m, nearbyint, AC_DEFINE(CONFIG_M_NEARBYINT, 1,
              [Define if nearbyint is available in libm.]))


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

dnl option to enable pdf doc generation 
AC_ARG_ENABLE(pdf-doc, AC_HELP_STRING([--enable-pdf-doc],
            [enable building and installation of pdf documentation]),, enable_pdf_doc=no)

AM_CONDITIONAL(PDF_DOC, test x${enable_pdf_doc} = xyes)
if test x$enable_pdf_doc = xyes; then
  dnl check for pdfjadetex to generate pdf documentation
  AC_PATH_PROG(PDFJADETEX, pdfjadetex, no)
  if test "x$PDFJADETEX" = "xno" ; then
    AC_MSG_ERROR(
      [cannot find 'pdfjadetex', please install the adequate package.])
  fi
fi

dnl option to enable Qt4 instead of default Qt3 library 
AC_ARG_ENABLE(qt4, AC_HELP_STRING([--enable-qt4],
            [enable Qt4 instead of Qt3 library]),, enable_qt3=yes)
AS_IF([test x$enable_qt3 = xyes],
[dnl Check for Qt3 library files
dnl AC_PATH_X
AC_PATH_QT3
ac_qtversion=qt3],
dnl
dnl ... or use Qt4
[
PKG_CHECK_MODULES(QT, QtCore QtGui >= 4.2.0, HAVE_QT4="yes", HAVE_QT4="no")
if test "x$HAVE_QT4" = "xyes"; then
  dnl Check needed because in some cases the QtGui includedir
  dnl doesn't contain the subsystem dir.
  QT_INCLUDE_DIR=`$PKG_CONFIG --variable=includedir QtGui`
  QT_CXXFLAGS=`$PKG_CONFIG --cflags QtCore QtGui QtNetwork`
  EXTRA_QT_INCLUDE_DIR="$QT_INCLUDE_DIR/Qt"
  AC_CHECK_FILE([$QT_INCLUDE_DIR/QWidget],
  AC_MSG_NOTICE([No extra QT_INCLUDE_DIR needed]),
  AC_CHECK_FILE([$EXTRA_QT_INCLUDE_DIR/QWidget],
             QT_CPPFLAGS="$QT_CPPFLAGS -I$EXTRA_QT_INCLUDE_DIR",
             AC_MSG_WARN([QWidget not found])))

  AC_MSG_NOTICE([Set QT_CXXFLAGS... $QT_CXXFLAGS])
  AC_SUBST(QT_CXXFLAGS)
  QT_INCLUDES=`$PKG_CONFIG --cflags-only-I QtCore QtGui QtNetwork Qt3Support`
  AC_SUBST(QT_INCLUDES)
else
  AC_MSG_ERROR([cannot find Qt4 library >= 4.2])
fi
ac_qtversion=qt4
])
dnl endo of qt version test


dnl check for moc
AC_PATH_PROGS(MOC, moc-$ac_qtversion moc, no, [$PATH:$QTDIR/bin])
if test "x$MOC" = "xno" ; then
    AC_MSG_ERROR(
                 [cannot find 'moc', please install the Qt
                  development tools package.])
fi

dnl check for lupdate
AC_PATH_PROGS(LUPDATE, lupdate-$ac_qtversion lupdate, no, [$PATH:$QTDIR/bin])
if test "x$LUPDATE" = "xno" ; then
  AC_MSG_ERROR(
    [cannot find 'lupdate', please install the Qt development tools package.])
else
  AC_MSG_CHECKING([for lupdate version])
  lupdate_version=`$LUPDATE -version 2>&1 | awk '{print $3}'`
  AC_MSG_RESULT([$lupdate_version])
    
  AC_MSG_CHECKING([whether lupdate supports -ts])
  lupdate_tsline=`$LUPDATE -help 2>&1| sed -n '/-ts/p'`
  if test -z "$lupdate_tsline"; then
    lupdate_ts=no 
  else
    lupdate_ts=yes
  fi
  AC_MSG_RESULT([$lupdate_ts])
fi
AM_CONDITIONAL(LUPDATE_TS, test x$lupdate_ts = xyes)

dnl check for lrelease
AC_PATH_PROGS(LRELEASE, lrelease-$ac_qtversion lrelease, no, [$PATH:$QTDIR/bin])
if test "x$LRELEASE" = "xno" ; then
  AC_MSG_ERROR(
    [cannot find 'lrelease', please install the Qt development tools package.])
else
  AC_MSG_CHECKING([for lrelease version])
  lrelease_version=`$LRELEASE -version 2>&1 | awk '{print $3}'`
  AC_MSG_RESULT([$lrelease_version])
    
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

dnl check for Qt library translations
AC_MSG_CHECKING([for Qt translations])
    qt_dirs="$QTDIR /usr/lib/$ac_qtversion /usr/lib/qt /usr/share/$ac_qtversion"
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


dnl check for some header files
AC_CHECK_HEADERS([cstdlib],,AC_MSG_ERROR([required header file missing]))

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

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
    

dnl check setting for documentation directory
AC_ARG_WITH(doc-dir,
  [AC_HELP_STRING([--with-doc-dir=DIR],
    [directory for documentation (e.g. html files)])],
  [ if test "x$withval" != "x" ; then
     DOC_DIR="$withval"
    fi ],
  [DOC_DIR="${prefix}/share/doc"])
AC_SUBST(DOC_DIR)

dnl set variable for application documentation directory
SPDRS60_DOC_DIR="$DOC_DIR/$PACKAGE"
AC_SUBST(SPDRS60_DOC_DIR)

dnl set variable for HTML documentation directory
HTML_DOC_DIR="$DOC_DIR/$PACKAGE/html"
AC_SUBST(HTML_DOC_DIR)
AC_DEFINE_UNQUOTED([HTML_DOC_DIR], ["$HTML_DOC_DIR"], \
  [directory html package documentation in html])


dnl directory for application icons
AC_ARG_WITH(icon-dir,
  [AC_HELP_STRING([--with-icon-dir=DIR],
    [directory for icons])],
  [ if test "x$withval" != "x" ; then
     ICONDIR="$withval"
    fi ],
  [ICONDIR="${prefix}/share/pixmaps"])
AC_SUBST(ICONDIR)


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

