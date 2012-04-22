/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    prefs.c
    Copyright (C) 2000 Naba Kumar

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <libanjuta/interfaces/ianjuta-editor.h>
#include <libanjuta/anjuta-preferences.h>
#include <libanjuta/anjuta-debug.h>

#include "properties.h"
#include "text_editor_prefs.h"
#include "text_editor_cbs.h"

#define GTK
#undef PLAT_GTK
#define PLAT_GTK 1
#include "Scintilla.h"
#include "SciLexer.h"
#include "ScintillaWidget.h"

#include "lexer.h"
#include "aneditor.h"

/* Editor preferences notifications */

#if 0

typedef {
	TextEditor *te;
	gchar *key;
	GCallback proxy;
	gpointer proxy_data;
	gboolean is_string;
} PrefPassedData;

static PrefPassedData*
pref_passed_data_new (TextEditor *te, const gchar *key, gboolean is_string,
					  GCallback *proxy, gponter proxy_data)
{
	PrefPassedData *pd = g_new0(PrefPassedData, 1);
	pd->te = te;
	pd->key = g_strdup (key);
	pd->proxy = proxy;
	pd->is_string = is_string;
	pd->data1 = data1;
	pd->data2 = data2;
	pd->data3 = data3;
	pd->data4 = data4;
}

static void
pref_notify (GConfClient *gclient, guint cnxn_id,
				   GConfEntry *entry, gpointer user_data)
{
	PrefPassedData *pd = (PrefPassedData*)user_data;
	if (!is_string)
		set_n_get_prop_int (pd->te, pd->key);
	else
		set_n_get_prop_string (pd->te, pd->key);
	if (pd->proxy)
		(pd->proxy)(data1, data2, data3, data4);
}

#endif

static gint
set_n_get_prop_int (GSettings *settings, const gchar *key)
{
	gint val;
	val = g_settings_get_int (settings, key);
	sci_prop_set_int_with_key (text_editor_get_props (), key, val);
	return val;
}

static gint
set_n_get_prop_bool (GSettings *settings, const gchar *key)
{
	gboolean val;
	val = g_settings_get_boolean (settings, key);
	sci_prop_set_int_with_key (text_editor_get_props (), key, val);
	return val;
}

static gchar *
set_n_get_prop_string (GSettings *settings, const gchar *key)
{
	gchar *val;
	val = g_settings_get_string (settings, key);
	sci_prop_set_with_key (text_editor_get_props (), key, val);
	return val;
}

static void
on_notify_disable_hilite (GSettings *settings,
                          const gchar* key,
                          gpointer user_data)
{
	TextEditor *te;
	
	te = TEXT_EDITOR (user_data);
	set_n_get_prop_bool (settings, key);
	text_editor_hilite (te, TRUE);
}

static void
on_notify_zoom_factor(GSettings* settings,
                      const gchar* key,
                      gpointer user_data)
{
	TextEditor *te;
	gint zoom_factor;

	te = TEXT_EDITOR (user_data);
	zoom_factor = set_n_get_prop_int (settings, key);
	text_editor_set_zoom_factor (te, zoom_factor);
	g_signal_emit_by_name(G_OBJECT (te), "update_ui");
}

static void
on_notify_tab_size (GSettings* settings,
                    const gchar* key,
                    gpointer user_data)
{
	TextEditor *te;
	gint tab_size;

	te = TEXT_EDITOR (user_data);
	tab_size = set_n_get_prop_int (settings, key);
	text_editor_command (te, ANE_SETTABSIZE, tab_size, 0);
}

static void
on_notify_use_tab_for_indentation(GSettings* settings,
                                  const gchar* key,
                                  gpointer user_data)
{
	TextEditor *te;
	gboolean use_tabs;

	te = TEXT_EDITOR (user_data);
	use_tabs = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETUSETABFORINDENT, use_tabs, 0);
}

static void
on_notify_indent_size (GSettings* settings,
                       const gchar* key,
                       gpointer user_data)
{
	TextEditor *te;
	gint indent_size;

	te = TEXT_EDITOR (user_data);
	indent_size = set_n_get_prop_int (settings, key);
	text_editor_command (te, ANE_SETINDENTSIZE, indent_size, 0);
}

static void
on_notify_wrap_bookmarks(GSettings* settings,
                         const gchar* key,
                         gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETWRAPBOOKMARKS, state, 0);
}

static void
on_notify_braces_check (GSettings* settings,
                        const gchar* key,
                        gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETINDENTBRACESCHECK, state, 0);
}

static void
on_notify_indent_maintain (GSettings* settings,
                           const gchar* key,
                           gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETINDENTMAINTAIN, state, 0);
}

static void
on_notify_tab_indents (GSettings* settings,
                       const gchar* key,
                       gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETTABINDENTS, state, 0);
}

static void
on_notify_backspace_unindents (GSettings* settings,
                               const gchar* key,
                               gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETBACKSPACEUNINDENTS, state, 0);
}

static void
on_notify_view_eols (GSettings* settings,
                     const gchar* key,
                     gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_VIEWEOL, state, 0);
}

static void
on_notify_view_whitespaces (GSettings* settings,
                            const gchar* key,
                            gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_VIEWSPACE, state, 0);
}

static void
on_notify_view_linewrap (GSettings* settings,
                         const gchar* key,
                         gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_LINEWRAP, state, 0);
}

static void
on_notify_view_indentation_guides (GSettings* settings,
                                   const gchar* key,
                                   gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_VIEWGUIDES, state, 0);
}

static void
on_notify_view_folds (GSettings* settings,
                      const gchar* key,
                      gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_FOLDMARGIN, state, 0);
}

static void
on_notify_view_markers (GSettings* settings,
                        const gchar* key,
                        gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SELMARGIN, state, 0);
}

static void
on_notify_view_linenums (GSettings* settings,
                         const gchar* key,
                         gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_LINENUMBERMARGIN, state, 0);
	/* text_editor_set_line_number_width (te); */
}

static void
on_notify_fold_symbols (GSettings* settings,
                        const gchar* key,
                        gpointer user_data)
{
	TextEditor *te;
	gchar *symbols;

	te = TEXT_EDITOR (user_data);
	symbols = set_n_get_prop_string (settings, key);
	text_editor_command (te, ANE_SETFOLDSYMBOLS, (long)symbols, 0);
	g_free (symbols);
}

static void
on_notify_fold_underline (GSettings* settings,
                          const gchar* key,
                          gpointer user_data)
{
	TextEditor *te;
	gboolean state;

	te = TEXT_EDITOR (user_data);
	state = set_n_get_prop_bool (settings, key);
	text_editor_command (te, ANE_SETFOLDUNDERLINE, state, 0);
}

static void
on_notify_edge_column (GSettings* settings,
                       const gchar* key,
                       gpointer user_data)
{
	TextEditor *te;
	gint size;

	te = TEXT_EDITOR (user_data);
	size = set_n_get_prop_int (settings, key);
	text_editor_command (te, ANE_SETEDGECOLUMN, size, 0);
}

#define REGISTER_NOTIFY(settings, key, func) \
	g_signal_connect (settings, "changed::" key, G_CALLBACK(func), te);

void
text_editor_prefs_init (TextEditor *te)
{
	gint val;
	GSettings *settings = te->settings;
	GSettings *docman_settings = te->docman_settings;
	GSettings *editor_settings = te->editor_settings;
	
	/* Sync prefs from gconf to props */
	set_n_get_prop_int (editor_settings, IANJUTA_EDITOR_TAB_WIDTH_KEY);
	set_n_get_prop_int (docman_settings, TEXT_ZOOM_FACTOR);
	set_n_get_prop_int (editor_settings, IANJUTA_EDITOR_INDENT_WIDTH_KEY);
	set_n_get_prop_bool (editor_settings, IANJUTA_EDITOR_USE_TABS_KEY);
	set_n_get_prop_bool (settings, DISABLE_SYNTAX_HILIGHTING);
	set_n_get_prop_bool (settings, WRAP_BOOKMARKS);
	set_n_get_prop_bool (settings, BRACES_CHECK);

	
	/* This one is special */
	val = set_n_get_prop_bool (settings, INDENT_MAINTAIN);
	sci_prop_set_int_with_key (te->props_base, INDENT_MAINTAIN".*", val);
	
	set_n_get_prop_bool (settings, TAB_INDENTS);
	set_n_get_prop_bool (settings, BACKSPACE_UNINDENTS);
	
	set_n_get_prop_bool (settings, VIEW_EOL);
	set_n_get_prop_bool (settings, VIEW_LINE_WRAP);
	set_n_get_prop_bool (settings, VIEW_WHITE_SPACES);
	set_n_get_prop_bool (settings, VIEW_INDENTATION_GUIDES);
	set_n_get_prop_bool (settings, VIEW_FOLD_MARGIN);
	set_n_get_prop_bool (settings, VIEW_MARKER_MARGIN);
	set_n_get_prop_bool (settings, VIEW_LINENUMBERS_MARGIN);
	g_free (set_n_get_prop_string (settings, FOLD_SYMBOLS));
	set_n_get_prop_bool (settings, FOLD_UNDERLINE);
	set_n_get_prop_int (settings, EDGE_COLUMN);
	
	/* Register gconf notifications */
	REGISTER_NOTIFY (editor_settings, IANJUTA_EDITOR_TAB_WIDTH_KEY, on_notify_tab_size);
	REGISTER_NOTIFY (docman_settings, TEXT_ZOOM_FACTOR, on_notify_zoom_factor);
	REGISTER_NOTIFY (editor_settings, IANJUTA_EDITOR_INDENT_WIDTH_KEY, on_notify_indent_size);
	REGISTER_NOTIFY (editor_settings, IANJUTA_EDITOR_USE_TABS_KEY, on_notify_use_tab_for_indentation);
	REGISTER_NOTIFY (settings, DISABLE_SYNTAX_HILIGHTING, on_notify_disable_hilite);
	/* REGISTER_NOTIFY (settings, INDENT_AUTOMATIC, on_notify_automatic_indentation); */
	REGISTER_NOTIFY (settings, WRAP_BOOKMARKS, on_notify_wrap_bookmarks);
	REGISTER_NOTIFY (settings, BRACES_CHECK, on_notify_braces_check);
	REGISTER_NOTIFY (settings, INDENT_MAINTAIN, on_notify_indent_maintain);
	REGISTER_NOTIFY (settings, TAB_INDENTS, on_notify_tab_indents);
	REGISTER_NOTIFY (settings, BACKSPACE_UNINDENTS, on_notify_backspace_unindents);
	REGISTER_NOTIFY (settings, VIEW_EOL, on_notify_view_eols);
	REGISTER_NOTIFY (settings, VIEW_LINE_WRAP, on_notify_view_linewrap);
	REGISTER_NOTIFY (settings, VIEW_WHITE_SPACES, on_notify_view_whitespaces);
	REGISTER_NOTIFY (settings, VIEW_INDENTATION_GUIDES, on_notify_view_indentation_guides);
	REGISTER_NOTIFY (settings, VIEW_FOLD_MARGIN, on_notify_view_folds);
	REGISTER_NOTIFY (settings, VIEW_MARKER_MARGIN, on_notify_view_markers);
	REGISTER_NOTIFY (settings, VIEW_LINENUMBERS_MARGIN, on_notify_view_linenums);
	REGISTER_NOTIFY (settings, FOLD_SYMBOLS, on_notify_fold_symbols);
	REGISTER_NOTIFY (settings, FOLD_UNDERLINE, on_notify_fold_underline);
	REGISTER_NOTIFY (settings, EDGE_COLUMN, on_notify_edge_column);
}

void
text_editor_prefs_finalize (TextEditor *te)
{
}
