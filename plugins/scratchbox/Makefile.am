# Plugin glade file
anjuta_gladedir = $(anjuta_glade_dir)
anjuta_glade_DATA = anjuta-scratchbox.ui \
		    anjuta-scratchbox-panel.png

# Plugin Icon file
anjuta_pixmapsdir = $(anjuta_image_dir)
anjuta_pixmaps_DATA = \
	anjuta-scratchbox-48.png

# Plugin description file
plugin_in_files = anjuta-scratchbox.plugin.in
%.plugin: %.plugin.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

anjuta_plugindir = $(anjuta_plugin_dir)
anjuta_plugin_DATA = $(plugin_in_files:.plugin.in=.plugin)

# Where to install the plugin
plugindir = $(anjuta_plugin_dir)
plugin_LTLIBRARIES = libanjuta-scratchbox.la

# Include paths
AM_CPPFLAGS = \
	$(LIBANJUTA_CFLAGS) \
	$(EXTRA_CFLAGS)

libanjuta_scratchbox_la_LDFLAGS = $(ANJUTA_PLUGIN_LDFLAGS)

# Plugin dependencies
libanjuta_scratchbox_la_LIBADD = \
	$(GNOME_LIBS) \
	$(LIBANJUTA_LIBS)

# Plugin sources
libanjuta_scratchbox_la_SOURCES = \
	plugin.c \
	plugin.h

prefs_ui_files = anjuta-scratchbox.ui
prefs_name = org.gnome.anjuta.scratchbox
include $(top_srcdir)/scripts/build-schemas.mk

EXTRA_DIST = \
	$(plugin_in_files) \
	$(anjuta_pixmaps_DATA) \
	$(anjuta_plugin_DATA) \
	$(anjuta_glade_DATA)
