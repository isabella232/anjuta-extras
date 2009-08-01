/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    plugin.c
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

#include <config.h>
#include <libanjuta/anjuta-shell.h>
#include <libanjuta/anjuta-debug.h>
#include <libanjuta/anjuta-encodings.h>
#include <libanjuta/interfaces/ianjuta-document-manager.h>
#include <libanjuta/interfaces/ianjuta-symbol-manager.h>
#include <libanjuta/interfaces/ianjuta-file.h>
#include <libanjuta/interfaces/ianjuta-file-savable.h>
#include <libanjuta/interfaces/ianjuta-editor-factory.h>
#include <libanjuta/interfaces/ianjuta-preferences.h>


#include "aneditor.h"
#include "lexer.h"
#include "plugin.h"
#include "style-editor.h"
#include "text_editor.h"

#define PREFS_GLADE PACKAGE_DATA_DIR"/glade/anjuta-editor-scintilla.ui"
#define ICON_FILE "anjuta-editor-scintilla-plugin-48.png"

gpointer parent_class;

/* Keep an up to date list of type name
 *---------------------------------------------------------------------------*/

static void
update_type_list (AnjutaShell *shell, IAnjutaIterable *iter, const gchar *name)
{
	gchar *list = NULL;
	GValue value = {0,};
	
	if (iter)
	{
		ianjuta_iterable_first (iter, NULL);
		if (ianjuta_iterable_get_length (iter, NULL) > 0)
		{
			GString *s = g_string_sized_new(ianjuta_iterable_get_length (iter, NULL) * 10);
			do {
				IAnjutaSymbol *symbol = IANJUTA_SYMBOL (iter);
				const gchar *sname = ianjuta_symbol_get_name (symbol, NULL);
				g_string_append(s, sname);
				g_string_append_c(s, ' ');
			} while (ianjuta_iterable_next (iter, NULL));
			list =  g_string_free(s, FALSE);
		}
		g_object_unref (iter);
	}
	
	anjuta_shell_get_value (shell, name, &value, NULL);
	if (G_VALUE_HOLDS_STRING(&value))
	{
		const gchar *value_list = g_value_get_string (&value);
		
		if (list == NULL)
		{
			anjuta_shell_remove_value (shell, name, NULL);
		}
		else if (strcmp (list, value_list) == 0)
		{
			g_free (list);
		}
		else
		{
			g_value_take_string (&value, list);
			anjuta_shell_add_value (shell, name, &value, NULL);
		}
	}
	else
	{
		if (list != NULL)
		{
			g_value_init (&value, G_TYPE_STRING);
			g_value_take_string (&value, list);
			anjuta_shell_add_value (shell, name, &value, NULL);
		}
	}
	g_value_unset (&value);
}

static void
project_symbol_found (guint search_id, IAnjutaIterable *iter, gpointer user_data)
{
	update_type_list (ANJUTA_SHELL (user_data), iter, TEXT_EDITOR_PROJECT_TYPE_LIST);
}

static void
system_symbol_found (guint search_id, IAnjutaIterable *iter, gpointer user_data)
{
	update_type_list (ANJUTA_SHELL (user_data), iter, TEXT_EDITOR_SYSTEM_TYPE_LIST);
}

static void
on_project_symbol_scanned (IAnjutaSymbolManager *manager, guint process, AnjutaShell *shell)
{
	/* Re-scan project symbols */
	ianjuta_symbol_manager_search_project_async (manager,
	    IANJUTA_SYMBOL_TYPE_TYPEDEF,
	    TRUE,
	    IANJUTA_SYMBOL_FIELD_SIMPLE,
	    "%",
	    IANJUTA_SYMBOL_MANAGER_SEARCH_FS_IGNORE,
	    -1,
	    -1,
	    NULL,
	    NULL,
	    project_symbol_found,
	    shell,
	    NULL);
}

static void
on_system_symbol_scanned (IAnjutaSymbolManager *manager, guint process, AnjutaShell *shell)
{
	/* Re-scan system symbols */
	ianjuta_symbol_manager_search_system_async (manager,
	    IANJUTA_SYMBOL_TYPE_TYPEDEF,
	    TRUE,
	    IANJUTA_SYMBOL_FIELD_SIMPLE,
	    "%",
	    IANJUTA_SYMBOL_MANAGER_SEARCH_FS_IGNORE,
	    -1,
	    -1,
	    NULL,
	    NULL,
	    system_symbol_found,
	    shell,
	    NULL);
}

static void 
on_style_button_clicked(GtkWidget* button, AnjutaPreferences* prefs)
{
	StyleEditor* se = style_editor_new(prefs);
	style_editor_show(se);
}

static gboolean
activate_plugin (AnjutaPlugin *plugin)
{
	IAnjutaSymbolManager *manager = anjuta_shell_get_interface (plugin->shell, IAnjutaSymbolManager, NULL);

	/* Get notified when scan end, to update type list */
	g_signal_connect (G_OBJECT (manager), "prj_scan_end", G_CALLBACK (on_project_symbol_scanned), plugin->shell);
	g_signal_connect (G_OBJECT (manager), "sys_scan_end", G_CALLBACK (on_system_symbol_scanned), plugin->shell);

	/* Initialize type list */
	on_project_symbol_scanned (manager, 0, plugin->shell);
	on_system_symbol_scanned (manager, 0, plugin->shell);
	
	return TRUE;
}

static gboolean
deactivate_plugin (AnjutaPlugin *plugin)
{
	IAnjutaSymbolManager *manager = anjuta_shell_get_interface (plugin->shell, IAnjutaSymbolManager, NULL);

	/* Disconnect signals */
	g_signal_handlers_disconnect_by_func (G_OBJECT (manager), G_CALLBACK (on_project_symbol_scanned), plugin->shell);
	g_signal_handlers_disconnect_by_func (G_OBJECT (manager), G_CALLBACK (on_system_symbol_scanned), plugin->shell);
	
	return TRUE;
}

static void
dispose (GObject *obj)
{
	/* EditorPlugin *eplugin = ANJUTA_PLUGIN_EDITOR (obj); */

	G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
finalize (GObject *obj)
{
	/* Finalization codes here */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
editor_plugin_instance_init (GObject *obj)
{
	/* EditorPlugin *plugin = ANJUTA_PLUGIN_EDITOR (obj); */
}

static void
editor_plugin_class_init (GObjectClass *klass) 
{
	AnjutaPluginClass *plugin_class = ANJUTA_PLUGIN_CLASS (klass);

	parent_class = g_type_class_peek_parent (klass);

	plugin_class->activate = activate_plugin;
	plugin_class->deactivate = deactivate_plugin;
	klass->dispose = dispose;
	klass->finalize = finalize;
}

static IAnjutaEditor*
itext_editor_factory_new_editor(IAnjutaEditorFactory* factory, 
								GFile* file,
								const gchar* filename, 
								GError** error)
{
	AnjutaShell *shell = ANJUTA_PLUGIN (factory)->shell;
	AnjutaPreferences *prefs = anjuta_shell_get_preferences (shell, NULL);
	AnjutaStatus *status = anjuta_shell_get_status (shell, NULL);
	/* file can be NULL, if we open a buffer, not a file */
	gchar* uri = file ? g_file_get_uri (file) : NULL;
	IAnjutaEditor* editor = IANJUTA_EDITOR(text_editor_new(status, prefs, shell,
														   uri, filename));
	g_free(uri);
	return editor;
}

static void
itext_editor_factory_iface_init (IAnjutaEditorFactoryIface *iface)
{
	iface->new_editor = itext_editor_factory_new_editor;
}

static void
ipreferences_merge(IAnjutaPreferences* ipref, AnjutaPreferences* prefs, GError** e)
{
	GtkBuilder* bxml = gtk_builder_new ();
	EditorPlugin* plugin = ANJUTA_PLUGIN_EDITOR (ipref);
	GError* error = NULL;
	if (!gtk_builder_add_from_file (bxml, PREFS_GLADE, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
	plugin->style_button = GTK_WIDGET (gtk_builder_get_object (bxml, "style_button"));
	g_signal_connect(G_OBJECT(plugin->style_button), "clicked", 
		G_CALLBACK(on_style_button_clicked), prefs);
	anjuta_preferences_add_from_builder (prefs,
								 bxml, "prefs_editor", _("Scintilla Editor"),  ICON_FILE);
	g_object_unref(bxml);
}

static void
ipreferences_unmerge(IAnjutaPreferences* ipref, AnjutaPreferences* prefs, GError** e)
{
	EditorPlugin* plugin = ANJUTA_PLUGIN_EDITOR (ipref);
	g_signal_handlers_disconnect_by_func(G_OBJECT(plugin->style_button), 
		G_CALLBACK(on_style_button_clicked), 
		anjuta_shell_get_preferences(ANJUTA_PLUGIN(plugin)->shell, NULL));
	
	anjuta_preferences_remove_page(prefs, _("Scintilla Editor"));
}

static void
ipreferences_iface_init(IAnjutaPreferencesIface* iface)
{
	iface->merge = ipreferences_merge;
	iface->unmerge = ipreferences_unmerge;	
}

ANJUTA_PLUGIN_BEGIN (EditorPlugin, editor_plugin);
ANJUTA_TYPE_ADD_INTERFACE(itext_editor_factory, IANJUTA_TYPE_EDITOR_FACTORY);
ANJUTA_TYPE_ADD_INTERFACE(ipreferences, IANJUTA_TYPE_PREFERENCES);
ANJUTA_PLUGIN_END;

ANJUTA_SIMPLE_PLUGIN (EditorPlugin, editor_plugin);
