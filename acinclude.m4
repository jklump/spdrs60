##   -*- autoconf -*-

dnl    This file is part of the KDE libraries/packages
dnl    Copyright (C) 1997 Janos Farkas (chexum@shadow.banki.hu)
dnl              (C) 1997,98,99 Stephan Kulow (coolo@kde.org)

dnl    This file is free software; you can redistribute it and/or
dnl    modify it under the terms of the GNU Library General Public
dnl    License as published by the Free Software Foundation; either
dnl    version 2 of the License, or (at your option) any later version.

dnl    This library is distributed in the hope that it will be useful,
dnl    but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl    Library General Public License for more details.

dnl    You should have received a copy of the GNU Library General Public License
dnl    along with this library; see the file COPYING.LIB.  If not, write to
dnl    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl    Boston, MA 02111-1307, USA.


dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_FIND_FILE],
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    echo "configure: __oline__: $i/$j" >&AC_FD_CC
    if test -r "$i/$j"; then
      echo "taking that" >&AC_FD_CC
      $3=$i
      break 2
    fi
  done
done
])

dnl KDE_FIND_PATH(program-name, variable-name, list-of-dirs,
dnl	if-not-found, test-parameter, prepend-path)
dnl
dnl Look for program-name in list-of-dirs+$PATH.
dnl If prepend-path is set, look in $PATH+list-of-dirs instead.
dnl If found, $variable-name is set. If not, if-not-found is evaluated.
dnl test-parameter: if set, the program is executed with this arg,
dnl                 and only a successful exit code is required.
AC_DEFUN([KDE_FIND_PATH],
[
   AC_MSG_CHECKING([for $1])
   if test -n "$$2"; then
        kde_cv_path="$$2";
   else
        kde_cache=`echo $1 | sed 'y%./+-%__p_%'`

        AC_CACHE_VAL(kde_cv_path_$kde_cache,
        [
        kde_cv_path="NONE"
	kde_save_IFS=$IFS
	IFS=':'
	dirs=""
	for dir in $PATH; do
	  dirs="$dirs $dir"
	done
	if test -z "$6"; then  dnl Append dirs in PATH (default)
	  dirs="$3 $dirs"
        else  dnl Prepend dirs in PATH (if 6th arg is set)
	  dirs="$dirs $3"
	fi
	IFS=$kde_save_IFS

        for dir in $dirs; do
	  if test -x "$dir/$1"; then
	    if test -n "$5"
	    then
              evalstr="$dir/$1 $5 2>&1 "
	      if eval $evalstr; then
                kde_cv_path="$dir/$1"
                break
	      fi
            else
		kde_cv_path="$dir/$1"
                break
	    fi
          fi
        done

        eval "kde_cv_path_$kde_cache=$kde_cv_path"

        ])

      eval "kde_cv_path=\"`echo '$kde_cv_path_'$kde_cache`\""

   fi

   if test -z "$kde_cv_path" || test "$kde_cv_path" = NONE; then
      AC_MSG_RESULT(not found)
      $4
   else
      AC_MSG_RESULT($kde_cv_path)
      $2=$kde_cv_path

   fi
])


dnl
AC_DEFUN([KDE_MOC_ERROR_MESSAGE],
[
    AC_MSG_ERROR([No Qt meta object compiler (moc) found!
Please check whether you installed Qt correctly.
You need to have a running moc binary.
configure tried to run $ac_cv_path_moc and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable MOC to the right one before running
configure.
])
])


dnl ------------------------------------------------------------------------
dnl Find the meta object compiler in the PATH,
dnl in $QTDIR/bin, and some more usual places
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_MOC],
[
   qt_bindirs=""
   for dir in $kde_qt_dirs; do
      qt_bindirs="$qt_bindirs $dir/bin $dir/src/moc"
   done
   qt_bindirs="$qt_bindirs /usr/bin /usr/X11R6/bin /usr/local/qt/bin"
   if test ! "$ac_qt_bindir" = "NO"; then
      qt_bindirs="$ac_qt_bindir $qt_bindirs"
   fi

   KDE_FIND_PATH(moc, MOC, [$qt_bindirs], [KDE_MOC_ERROR_MESSAGE])
   AC_SUBST(MOC)
])


dnl
AC_DEFUN([KDE_PRINT_QT_PROGRAM],
[
AC_REQUIRE([KDE_USE_QT3])
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include <qglobal.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qstylefactory.h>
#include <private/qucomextra_p.h>
EOF

echo "#if ! ($kde_qt_verstring)" >> conftest.$ac_ext
cat >> conftest.$ac_ext <<EOF
#error 1
#endif

int main() {
EOF
cat >> conftest.$ac_ext <<EOF
    (void)QStyleFactory::create(QString::null);
    QCursor c(Qt::WhatsThisCursor);
EOF
cat >> conftest.$ac_ext <<EOF
    return 0;
}
EOF
])


dnl
AC_DEFUN([KDE_USE_QT3],
[
# Current default Qt version: 3.0
kde_qt_minversion=">= Qt 3.0 and < 4.0"
kde_qt_verstring="QT_VERSION >= 300 && QT_VERSION < 0x040000"
kde_qt_dirs="$QTDIR /usr/lib/qt3 /usr/lib/qt /usr/share/qt3"
])


dnl ------------------------------------------------------------------------
dnl Try to find the Qt headers and libraries.
dnl $(QT_LDFLAGS) will be -Lqtliblocation (if needed)
dnl and $(QT_INCLUDES) will be -Iqthdrlocation (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT3],
[
AC_REQUIRE([KDE_USE_QT3])
AC_REQUIRE([KDE_CHECK_LIB64])

USING_QT_MT=""

dnl ------------------------------------------------------------------------
dnl If we not get --disable-qt-mt then adjust some vars for the host.
dnl ------------------------------------------------------------------------

KDE_MT_LDFLAGS=
KDE_MT_LIBS=
KDE_CHECK_THREADING
if test "x$kde_use_threading" = "xyes"; then
  CPPFLAGS="$USE_THREADS -DQT_THREAD_SUPPORT $CPPFLAGS"
  KDE_MT_LDFLAGS="$USE_THREADS"
dnl programs using qt3-mt do not have to link against pthread lib
dnl  KDE_MT_LIBS="$LIBPTHREAD"
fi
AC_SUBST(KDE_MT_LDFLAGS)
dnl AC_SUBST(KDE_MT_LIBS)

kde_qt_was_given=yes

dnl ------------------------------------------------------------------------
dnl If we haven't been told how to link to Qt, we work it out for ourselves.
dnl ------------------------------------------------------------------------
if test -z "$LIBQT_GLOB"; then
    LIBQT_GLOB="libqt.*"
fi

if test -z "$LIBQT"; then
  qtlib="qt"
  kde_int_qt="-l$qtlib"
else
  kde_int_qt="$LIBQT"
  kde_lib_qt_set=yes
fi

dnl ------------------------------------------------------------------------
dnl If we got --enable-qt-mt then adjust the Qt library name for the host.
dnl ------------------------------------------------------------------------

if test -z "$LIBQT"; then
  LIBQT="-l$qtlib-mt"
  kde_int_qt="-l$qtlib-mt"
else
  LIBQT="$qtlib-mt"
  kde_int_qt="$qtlib-mt"
fi
LIBQT_GLOB="lib$qtlib-mt.*"
USING_QT_MT="using -mt"

AC_MSG_CHECKING([for Qt3])

ac_qt_includes=NO ac_qt_libraries=NO ac_qt_bindir=NO
qt_libraries=""
qt_includes=""
AC_ARG_WITH(qt-dir,
    AC_HELP_STRING([--with-qt-dir=DIR],[where the root of Qt is installed ]),
    [  ac_qt_includes="$withval"/include
       ac_qt_libraries="$withval"/lib${kdelibsuff}
       ac_qt_bindir="$withval"/bin
    ])

AC_ARG_WITH(qt-includes,
    AC_HELP_STRING([--with-qt-includes=DIR],[where the Qt includes are. ]),
    [
       ac_qt_includes="$withval"
    ])

kde_qt_libs_given=no

AC_ARG_WITH(qt-libraries,
    AC_HELP_STRING([--with-qt-libraries=DIR],[where the Qt library is installed.]),
    [  ac_qt_libraries="$withval"
       kde_qt_libs_given=yes
    ])

AC_CACHE_VAL(ac_cv_have_qt,
[#try to guess Qt locations

qt_incdirs=""
for dir in $kde_qt_dirs; do
   qt_incdirs="$qt_incdirs $dir/include $dir"
done
qt_incdirs="$QTINC $qt_incdirs /usr/local/qt/include /usr/include/qt /usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/qt /usr/include/qt3 $x_includes"
if test ! "$ac_qt_includes" = "NO"; then
   qt_incdirs="$ac_qt_includes $qt_incdirs"
fi

kde_qt_header=qstyle.h

AC_FIND_FILE($kde_qt_header, $qt_incdirs, qt_incdir)
ac_qt_includes="$qt_incdir"

qt_libdirs=""
for dir in $kde_qt_dirs; do
   qt_libdirs="$qt_libdirs $dir/lib${kdelibsuff} $dir"
done
qt_libdirs="$QTLIB $qt_libdirs /usr/X11R6/lib /usr/lib /usr/local/qt/lib $x_libraries"
if test ! "$ac_qt_libraries" = "NO"; then
  qt_libdir=$ac_qt_libraries
else
  qt_libdirs="$ac_qt_libraries $qt_libdirs"
  # if the Qt was given, the chance is too big that libqt.* doesn't exist
  qt_libdir=NONE
  for dir in $qt_libdirs; do
    try="ls -1 $dir/${LIBQT_GLOB}"
    if test -n "`$try 2> /dev/null`"; then qt_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
  done
fi
for a in $qt_libdir/lib`echo ${kde_int_qt} | sed 's,^-l,,'`_incremental.*; do
  if test -e "$a"; then
    LIBQT="$LIBQT ${kde_int_qt}_incremental"
    break
  fi
done

ac_qt_libraries="$qt_libdir"

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

ac_cxxflags_safe="$CXXFLAGS"
ac_ldflags_safe="$LDFLAGS"
ac_libs_safe="$LIBS"

CXXFLAGS="$CXXFLAGS -I$qt_incdir"
LDFLAGS="$LDFLAGS -L$qt_libdir $USER_LDFLAGS $KDE_MT_LDFLAGS"
LIBS="$LIBS $LIBQT $KDE_MT_LIBS"

KDE_PRINT_QT_PROGRAM

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
  ac_qt_libraries="NO"
fi
rm -f conftest*
CXXFLAGS="$ac_cxxflags_safe"
LDFLAGS="$ac_ldflags_safe"
LIBS="$ac_libs_safe"

AC_LANG_RESTORE
if test "$ac_qt_includes" = NO || test "$ac_qt_libraries" = NO; then
  ac_cv_have_qt="have_qt=no"
  ac_qt_notfound=""
  missing_qt_mt=""
  if test "$ac_qt_includes" = NO; then
    if test "$ac_qt_libraries" = NO; then
      ac_qt_notfound="(headers and libraries)";
    else
      ac_qt_notfound="(headers)";
    fi
  else
       missing_qt_mt="
Make sure that you have compiled Qt with thread support!"
       ac_qt_notfound="(library $qtlib-mt)";
  fi

  AC_MSG_ERROR([Qt ($kde_qt_minversion) $ac_qt_notfound not found. Please check your installation!
For more details about this problem, look at the end of config.log.$missing_qt_mt])
else
  have_qt="yes"
fi
])

eval "$ac_cv_have_qt"

if test "$have_qt" != yes; then
  AC_MSG_RESULT([$have_qt]);
else
  ac_cv_have_qt="have_qt=yes \
    ac_qt_includes=$ac_qt_includes ac_qt_libraries=$ac_qt_libraries"
  AC_MSG_RESULT([libraries $ac_qt_libraries, headers $ac_qt_includes $USING_QT_MT])

  qt_libraries="$ac_qt_libraries"
  qt_includes="$ac_qt_includes"
fi

AC_SUBST(qt_libraries)
AC_SUBST(qt_includes)

if test "$qt_includes" = "$x_includes" || test -z "$qt_includes"; then
 QT_INCLUDES=""
else
 QT_INCLUDES="-I$qt_includes"
fi

if test "$qt_libraries" = "$x_libraries" || test -z "$qt_libraries"; then
 QT_LDFLAGS=""
else
 QT_LDFLAGS="-L$qt_libraries"
fi

AC_SUBST(QT_INCLUDES)
AC_SUBST(QT_LDFLAGS)
AC_PATH_QT_MOC

LIB_QT="$kde_int_qt "

test -z "$KDE_MT_LIBS" || LIB_QT="$LIB_QT $KDE_MT_LIBS"
for a in $qt_libdir/lib`echo ${kde_int_qt} | sed 's,^-l,,'`_incremental.*; do
  if test -e "$a"; then
     LIB_QT="$LIB_QT ${kde_int_qt}_incremental"
     break
  fi
done

AC_SUBST(LIB_QT)
])


dnl
AC_DEFUN([KDE_CHECK_COMPILER_FLAG],
[
AC_MSG_CHECKING([whether $CXX supports -$1])
kde_cache=`echo $1 | sed 'y% .=/+-,%____p__%'`
AC_CACHE_VAL(kde_cv_prog_cxx_$kde_cache,
[
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -$1"
  AC_TRY_LINK([],[ return 0; ], [eval "kde_cv_prog_cxx_$kde_cache=yes"], [])
  CXXFLAGS="$save_CXXFLAGS"
  AC_LANG_RESTORE
])
if eval "test \"`echo '$kde_cv_prog_cxx_'$kde_cache`\" = yes"; then
 AC_MSG_RESULT(yes)
 :
 $2
else
 AC_MSG_RESULT(no)
 :
 $3
fi
])


dnl
AC_DEFUN([KDE_CHECK_LIB64],
[
    AC_ARG_ENABLE(libsuffix,
        AC_HELP_STRING([--enable-libsuffix],
            [/lib directory suffix (64,32,none,auto[=default])]),
            kdelibsuff=$enableval, kdelibsuff="auto")
    
    if test "$kdelibsuff" = "auto"; then
    
cat > conftest.c << EOF
#include <stdio.h>
int main() {
 return 0;
}
EOF
        kdelibsuff=`$CC conftest.c -o conftest.out; ldd conftest.out |sed -ne '/libc.so/{
    s,.*/lib\([[^\/]]*\)/.*,\1, 
    p 
}'`
        rm -rf conftest.*
    fi  
        
    if test "$kdelibsuff" = "no" || test "$kdelibsuff" = "none"; then
       kdelibsuff=
    fi
    if test -z "$kdelibsuff"; then
        AC_MSG_RESULT([not using lib directory suffix])
        AC_DEFINE(KDELIBSUFF, [""], Suffix for lib directories)
    else
        if test "$libdir" = '${exec_prefix}/lib'; then
            libdir="$libdir${kdelibsuff}"
            AC_SUBST([libdir], ["$libdir"])  dnl ugly hack for lib64 platforms
        fi
        AC_DEFINE_UNQUOTED(KDELIBSUFF, ["${kdelibsuff}"], Suffix for lib directories)
        AC_MSG_RESULT([using lib directory suffix $kdelibsuff])
    fi
])


dnl
AC_DEFUN([KDE_CHECK_LIBPTHREAD],
[
  AC_MSG_CHECKING([for pthread_create in -lpthread])
  kde_safe_libs=$LIBS
  LIBS="$LIBS -lpthread"
  AC_TRY_LINK([#include <pthread.h>],[(void)pthread_create(0,0,0,0);],[
      AC_MSG_RESULT(yes)
      LIBPTHREAD="-lpthread"],[
      AC_MSG_RESULT(no)])
  LIBS=$kde_safe_libs

  AC_SUBST(LIBPTHREAD)
])


dnl
AC_DEFUN([KDE_CHECK_PTHREAD_OPTION],
[
    USE_THREADS=""
    if test -z "$LIBPTHREAD"; then
      KDE_CHECK_COMPILER_FLAG(pthread, [USE_THREADS="-D_THREAD_SAFE -pthread"])
    fi

    AH_VERBATIM(__svr_define, [
#if defined(__SVR4) && !defined(__svr4__)
#define __svr4__ 1
#endif
])
    case $host_os in
 	solaris*)
		KDE_CHECK_COMPILER_FLAG(mt, [USE_THREADS="-mt"])
                CPPFLAGS="$CPPFLAGS -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DUSE_SOLARIS -DSVR4"
    		;;
        freebsd*)
                CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE $PTHREAD_CFLAGS"
                ;;
        aix*)
                CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE"
                LIBPTHREAD="$LIBPTHREAD -lc_r"
                ;;
        linux*) CPPFLAGS="$CPPFLAGS -D_REENTRANT"
                if test "$CXX" = "KCC"; then
                  CXXFLAGS="$CXXFLAGS --thread_safe"
		  NOOPT_CXXFLAGS="$NOOPT_CXXFLAGS --thread_safe"
                fi
                ;;
	*)
		;;
    esac
    AC_SUBST(USE_THREADS)
    AC_SUBST(LIBPTHREAD)
])


dnl
AC_DEFUN([KDE_CHECK_THREADING],
[
  AC_REQUIRE([KDE_CHECK_LIBPTHREAD])
  AC_REQUIRE([KDE_CHECK_PTHREAD_OPTION])
  dnl default is yes if libpthread is found and no if no libpthread is available
  if test -z "$LIBPTHREAD"; then
    if test -z "$USE_THREADS"; then
      kde_check_threading_default=no
    else
      kde_check_threading_default=yes
    fi
  else
    kde_check_threading_default=yes
  fi
  AC_ARG_ENABLE(threading,AC_HELP_STRING([--disable-threading],[disables threading even if libpthread found]),
   kde_use_threading=$enableval, kde_use_threading=$kde_check_threading_default)
  if test "x$kde_use_threading" = "xyes"; then
    AC_DEFINE(HAVE_LIBPTHREAD, 1, [Define if you have a working libpthread (will enable threaded code)])
  fi
])


dnl this is a redefinition of autoconf 2.5x's AC_FOREACH.
dnl When the argument list becomes big, as in KDE for AC_OUTPUT in
dnl big packages, m4_foreach is dog-slow.  So use our own version of
dnl it.  (matz@kde.org)
m4_define([mm_foreach], [m4_pushdef([$1])_mm_foreach($@)m4_popdef([$1])])
m4_define([mm_car], [[$1]])
m4_define([mm_car2], [[$@]])
m4_define([_mm_foreach], [m4_if(m4_quote($2), [], [],
    [m4_define([$1], mm_car($2))$3[]_mm_foreach([$1],
        mm_car2(m4_shift($2)), [$3])])])
m4_define([AC_FOREACH],
    [mm_foreach([$1], m4_split(m4_normalize([$2])), [$3])])

