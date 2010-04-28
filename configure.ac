dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_PREREQ(2.59)

m4_define(anjuta_major_version,  2)
m4_define(anjuta_minor_version, 30)
m4_define(anjuta_micro_version,  1)
m4_define(anjuta_nano_version, 0)
m4_define(anjuta_version, anjuta_major_version.anjuta_minor_version.anjuta_micro_version.anjuta_nano_version)

AC_INIT(anjuta-extras, 2.30.0)

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

BINUTILS_REQUIRED=2.15.92
LIBGRAPHVIZ_REQUIRED=1.0
GTK_REQUIRED=2.17.10
GLIB_REQUIRED=2.16.0
GCONF_REQUIRED=2.12.0
GNOME_REQUIRED=2.12.0

AC_SUBST(LIBGRAPHVIZ_REQUIRED)
AC_SUBST(BINUTILS_REQUIRED)
AC_SUBST(GTK_REQUIRED)
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GCONF_REQUIRED)
AC_SUBST(GNOME_REQUIRED)


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


dnl ***************************************************************************
dnl we need gconftool-2 in order to install the schema
dnl ***************************************************************************
AC_PATH_PROG(GCONFTOOL, gconftool-2, no)

if test "x$GCONFTOOL" = "xno"; then
  AC_MSG_ERROR([gconftool-2 executable not found in your path - should be installed with GConf])
fi
AM_GCONF_SOURCE_2


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
PKG_CHECK_MODULES(LIBANJUTA, [libanjuta-1.0 >= $ANJUTA_EXTRAS_VERSION])

dnl Check for libgnome
PKG_CHECK_MODULES(GNOME, libgnomecanvas-2.0 >= $GNOME_REQUIRED)

dnl check for gthread
PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= $GLIB_REQUIRED)

dnl Valgrind plugin
dnl check for libbfd
dnl -------------------------------------------------------------

AC_ARG_ENABLE(plugin-valgrind,
	AC_HELP_STRING([--disable-plugin-valgrind],[Disable Valgrind plugin]), 
	[ if test "$enableval" = "no"; then
		user_disabled_valgrind=1
	  fi ],
	[ user_disabled_valgrind=0 ])

AC_MSG_CHECKING(if valgrind plugin is disabled)
if test "$user_disabled_valgrind" = 1; then
	AC_MSG_RESULT(yes)
	valgrind="no"
else
	AC_MSG_RESULT(no)
	valgrind="yes"
fi

AC_CHECK_LIB(bfd, bfd_get_error, [PLUGIN_VALGRIND_LIBS="-Wl,-Bstatic -lbfd -liberty -Wl,-Bdynamic -lz"],
   valgrind="no",
  [-liberty -lz])

AC_SUBST(PLUGIN_VALGRIND_LIBS)
AM_CONDITIONAL(HAVE_PLUGIN_VALGRIND, [test x$valgrind = xyes])

dnl check for bfd_get_section_size_before_reloc() in libbfd
AC_MSG_CHECKING(for bfd_get_section_size_before_reloc)
AC_TRY_LINK([
        #include <bfd.h>
        ], [
        asection *section;

        bfd_get_section_size_before_reloc (section);
        ],
        AC_MSG_RESULT(yes)
        AC_DEFINE(HAVE_BFD_GET_SECTION_SIZE_BEFORE_RELOC, [], [Define if libbfd
contains the function bfd_get_section_size_before_reloc])
,
        AC_MSG_RESULT(no)
)

dnl Check for graphviz (class inheritance and profiler plugins)
dnl --------------------------------------------------------------------------
dnl FIXME: For some strange reason graphviz-devel rpm install the *.pc files
dnl in /usr/lib/graphviz/pkgconfig, instead of usual /usr/lib/pkgconfig. This
dnl makes pkgconfig check fail if we don't add the above path in
dnl PKG_CONFIG_PATH. We have added both /usr/lib/graphviz/pkgconfig and
dnl /usr/local/lib/pkgconfig paths. If graphviz is installed in other prefix
dnl (other than /usr or /usr/local or the current installation prefix),
dnl following pkgconfig check will fail.
dnl --------------------------------------------------------------------------
AC_ARG_ENABLE(graphviz,
	AC_HELP_STRING([--disable-graphviz],[Disable GraphViz support in Anjuta]), 
	[ if test "$enableval" = "no"; then
		user_disabled_graphviz=1
	  fi ],
	[ user_disabled_graphviz=0 ])

AC_MSG_CHECKING(if GraphViz support is disabled)
if test "$user_disabled_graphviz" = 1; then
	AC_MSG_RESULT(yes)
	libgraphviz_found="no"
else
	AC_MSG_RESULT(no)
	
	dnl Set pkgconfig path
	ANJUTA_PKG_CONFIG_PATH_SAVE="${PKG_CONFIG_PATH}"
	PKG_CONFIG_PATH="/usr/lib/graphviz/pkgconfig:$PKG_CONFIG_PATH"
	PKG_CONFIG_PATH="/usr/local/lib/graphviz/pkgconfig:$PKG_CONFIG_PATH"
	PKG_CONFIG_PATH="/usr/lib64/graphviz/pkgconfig:$PKG_CONFIG_PATH"
	PKG_CONFIG_PATH="/usr/local/lib64/graphviz/pkgconfig:$PKG_CONFIG_PATH"
	
	anjuta_save_prefix="$prefix"
	test "x$prefix" = xNONE && prefix=$ac_default_prefix
	graphviz_pkgconfig=`eval echo ${libdir}/graphviz/pkgconfig`
	PKG_CONFIG_PATH="$graphviz_pkgconfig:$PKG_CONFIG_PATH"
	prefix="$anjuta_save_prefix"

	export PKG_CONFIG_PATH
	PKG_CHECK_MODULES(GRAPHVIZ,
		[libgvc >= $LIBGRAPHVIZ_REQUIRED libgraph >= $LIBGRAPHVIZ_REQUIRED], 
		[libgraphviz_found="yes"],
		[libgraphviz_found="no"])
	
	dnl Restore pkgconfig path
	PKG_CONFIG_PATH="${ANJUTA_PKG_CONFIG_PATH_SAVE}"
	export PKG_CONFIG_PATH
fi

AM_CONDITIONAL(HAVE_GRAPHVIZ, [test x$libgraphviz_found = xyes])

dnl Define HAVE_GRAPHVIZ to disable Function Call Chart in the Profiler if 
dnl Graphviz isn't on the system
if test "$libgraphviz_found" = yes; then
	AC_DEFINE(HAVE_GRAPHVIZ, [], [Define if we have GraphViz >= 2.6.0])
fi

dnl we need gconftool-2 in order to install the schema
AC_PATH_PROG(GCONFTOOL, gconftool-2, no)

if test "x$GCONFTOOL" = "xno"; then
  AC_MSG_ERROR([gconftool-2 executable not found in your path - should be installed with GConf])
fi

AM_GCONF_SOURCE_2

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

anjutalibdir=`pkg-config $pkg_prefix --variable=libdir libanjuta-1.0`
anjutadatadir=`pkg-config $pkg_prefix --variable=datadir libanjuta-1.0`
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
plugins/class-inheritance/Makefile
plugins/sample1/Makefile
plugins/valgrind/Makefile
plugins/profiler/Makefile
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
if [ test x$libgraphviz_found = xyes ]; then
        echo "Building class inheritance plugin: .....................YES"
        echo "Building performance profiler with function call chart: YES"
else
        echo "Building class inheritance plugin: .....................NO"
        echo "        Requires graphviz (>= 2.6.0); http://graphviz.org"
        echo "Building performance profiler with function call chart: NO"
	 echo "        Requires graphviz (>= 2.6.0); http://graphviz.org"
fi
if [ test x$valgrind = xyes ]; then
        echo "Building Valgrind debugger plugin: .....................YES"
else
        echo "Building Valgrind debugger plugin: .....................NO"
	echo "		Requires binutils-dev"
fi
if [ test x$scintilla = xyes ]; then
        echo "Building Scintilla based editor: .......................YES"
else
        echo "Building Scintilla based editor: .......................NO"
fi
