dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_PREREQ(2.59)

m4_define(anjuta_major_version,  3)
m4_define(anjuta_minor_version,  1)
m4_define(anjuta_micro_version,  0)
m4_define(anjuta_nano_version,   0)
m4_define(anjuta_version, anjuta_major_version.anjuta_minor_version.anjuta_micro_version.anjuta_nano_version)

AC_INIT(anjuta-extras, anjuta_version)

ANJUTA_EXTRAS_VERSION=anjuta_version
AC_SUBST(ANJUTA_EXTRAS_VERSION)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_CPP
AC_LANG_C

ANJUTA_REQUIRED=anjuta_major_version.anjuta_minor_version.0
GTK_REQUIRED=3.0.0
GLIB_REQUIRED=2.16.0

AC_SUBST(GTK_REQUIRED)
AC_SUBST(GLIB_REQUIRED)

dnl GSettings
GLIB_GSETTINGS

dnl ***************************************************************************
dnl Check for pkgconfig
dnl ***************************************************************************
AC_PATH_PROG(PKG_CONFIG, pkg-config,no)
if test x$PKGCONFIG_CONFIG = xno; then
  AC_MSG_ERROR(Please install the pkg-config package from http://www.freedesktop.org/software/pkgconfig/)
fi

dnl ***************************************************************************
dnl Check for marshal and enum generators
dnl ***************************************************************************
GLIB_GENMARSHAL="`$PKG_CONFIG --variable=glib_genmarshal glib-2.0`"
AC_SUBST(GLIB_GENMARSHAL)
GLIB_MKENUMS="`$PKG_CONFIG --variable=glib_mkenums glib-2.0`"
AC_SUBST(GLIB_MKENUMS)

dnl Compiler settings
dnl ---------------------------------------------------------------------------

GNOME_COMPILE_WARNINGS(maximum)
GNOME_CXX_WARNINGS(yes)
AM_CFLAGS="$AM_CFLAGS $WARN_CFLAGS"
AM_CXXFLAGS="$AM_CXXFLAGS $WARN_CXXFLAGS"

dnl Enable debugging mode
AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--enable-debug],[Enable debug messages]),
  AM_CFLAGS="$AM_CFLAGS -DDEBUG"
  AM_CXXFLAGS="$AM_CXXFLAGS -DDEBUG")
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_CXXFLAGS)

dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=anjuta-extras
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])
AM_PROG_LIBTOOL

dnl maintainer mode
AM_MAINTAINER_MODE

dnl Check for libanjuta
PKG_CHECK_MODULES(LIBANJUTA, [libanjuta-3.0 >= $ANJUTA_REQUIRED])

dnl check for gthread
PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= $GLIB_REQUIRED)

dnl Scintilla Editor
dnl -------------------------------------------------------------

AC_ARG_ENABLE(plugin-scintilla,
	AC_HELP_STRING([--disable-plugin-scintilla],[ Scintilla based editor in Anjuta]), 
	[ if test "$enableval" = "no"; then
		user_disabled_scintilla=1
	  fi ],
	[ user_disabled_scintilla=0 ])

AC_MSG_CHECKING(if scintilla plugin is enabled)
if test "$user_disabled_scintilla" = 1; then
	AC_MSG_RESULT(no)
	scintilla="no"
else
	AC_MSG_RESULT(yes)
	scintilla="yes"
fi
AM_CONDITIONAL(HAVE_PLUGIN_SCINTILLA, [test x$scintilla = xyes])

dnl Setup Plugin directories
dnl ------------------------

dnl In order to pass distcheck, the script has to install the plugin
dnl in the correct prefix if defined. This is done using the
dnl pkg-config option --define-variable.
if test "x$prefix" = "xNONE" ; then
	pkg_prefix=""
else
	pkg_prefix="--define-variable=prefix=$prefix"
fi

anjutalibdir=`pkg-config $pkg_prefix --variable=libdir libanjuta-3.0`
anjutadatadir=`pkg-config $pkg_prefix --variable=datadir libanjuta-3.0`
AC_SUBST(anjutalibdir)
AC_SUBST(anjutadatadir)
anjuta_plugin_dir='$(anjutalibdir)/anjuta'
anjuta_data_dir='$(anjutadatadir)/anjuta'
anjuta_ui_dir='$(anjutadatadir)/anjuta/ui'
anjuta_glade_dir='$(anjutadatadir)/anjuta/glade'
anjuta_image_dir='$(anjutadatadir)/pixmaps/anjuta'
AC_SUBST(anjuta_plugin_dir)
AC_SUBST(anjuta_data_dir)
AC_SUBST(anjuta_ui_dir)
AC_SUBST(anjuta_glade_dir)
AC_SUBST(anjuta_image_dir)

EXTRA_CFLAGS='-DPACKAGE_PIXMAPS_DIR=\""$(anjuta_image_dir)"\" -DPACKAGE_LIB_DIR=\""$(anjutalibdir)"\" -DPACKAGE_DATA_DIR=\""$(anjuta_data_dir)"\"'
AC_SUBST(EXTRA_CFLAGS)

ANJUTA_PLUGIN_LDFLAGS='-module -avoid-version -no-undefined'
AC_SUBST(ANJUTA_PLUGIN_LDFLAGS)

PKG_CHECK_MODULES(LIBXML, libxml-2.0)
AC_OUTPUT([
Makefile
plugins/Makefile
plugins/scratchbox/Makefile
plugins/sample1/Makefile
plugins/scintilla/Makefile
plugins/scintilla/scintilla/Makefile
plugins/scintilla/scintilla/include/Makefile
plugins/scintilla/properties/Makefile
po/Makefile.in
scripts/Makefile
])

echo " "
echo "-------------------------------------------------------------------"
echo "Conditionally built plugins:"
echo "-------------------------------------------------------------------"
if [ test x$scintilla = xyes ]; then
        echo "Building Scintilla based editor: .......................YES"
else
        echo "Building Scintilla based editor: .......................NO"
fi
