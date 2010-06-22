/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gprof-view.c
 * Copyright (C) James Liggett 2006 <jrliggett@cox.net>
 * 
 * gprof-view.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2, or (at your option) any later version.
 * 
 * plugin.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with plugin.c.  See the file "COPYING".  If not,
 * write to:  The Free Software Foundation, Inc.,
 *            51 Franklin Street, Fifth Floor,
 *            Boston,  MA  02110-1301, USA.
 */

#include "gprof-view.h"

struct _GProfViewPriv
{
	GProfProfileData *profile_data;
	IAnjutaSymbolManager *symbol_manager;
	IAnjutaDocumentManager *document_manager;
};

static void
gprof_view_init (GProfView *self)
{
	self->priv = g_new0 (GProfViewPriv, 1);
}

static void
gprof_view_finalize (GObject *obj)
{
	GProfView *self;
	
	self = (GProfView *) obj;
	
	gprof_profile_data_free (self->priv->profile_data);
	g_free(self->priv);
}

static void 
gprof_view_class_init (GProfViewClass *klass)
{
	GObjectClass *object_class;
	
	object_class = (GObjectClass *) klass;
	object_class->finalize = gprof_view_finalize;
	
	klass->refresh = NULL;
	klass->get_widget = NULL;
}

GType 
gprof_view_get_type (void)
{
	static GType obj_type = 0;
	
	if (!obj_type)
	{
		static const GTypeInfo obj_info = 
		{
			sizeof (GProfViewClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) gprof_view_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,           /* class_data */
			sizeof (GProfView),
			0,              /* n_preallocs */
			(GInstanceInitFunc) gprof_view_init,
			NULL            /* value_table */
		};
		obj_type = g_type_register_static (G_TYPE_OBJECT,
		                                   "GProfView", &obj_info, 0);
	}
	return obj_type;
}

void
gprof_view_set_data (GProfView *self, GProfProfileData *profile_data)
{
	self->priv->profile_data = g_object_ref (profile_data);
}

GProfProfileData *
gprof_view_get_data (GProfView *self)
{
	return self->priv->profile_data;
}

void
gprof_view_set_symbol_manager (GProfView *self, 
							   IAnjutaSymbolManager *symbol_manager)
{
	self->priv->symbol_manager = symbol_manager;
}

void
gprof_view_set_document_manager (GProfView *self, 
							   	 IAnjutaDocumentManager *document_manager)
{
	self->priv->document_manager = document_manager;
}

void 
gprof_view_show_symbol_in_editor (GProfView *self,
								  const gchar *symbol_name)
{
	IAnjutaIterable *symbol_iter;
	IAnjutaSymbol *symbol;
	guint line;
	IAnjutaSymbolQuery *query;
	static IAnjutaSymbolField query_fields[] = {
		IANJUTA_SYMBOL_FIELD_ID,
		IANJUTA_SYMBOL_FIELD_NAME,
		IANJUTA_SYMBOL_FIELD_TYPE
	};
	
	
	if (self->priv->symbol_manager &&
		self->priv->document_manager)
	{									   	
		/* create one everytime. This method doesn't need particular performances */
		query = ianjuta_symbol_manager_create_query (self->priv->symbol_manager,
	    										IANJUTA_SYMBOL_QUERY_SEARCH,
	    										IANJUTA_SYMBOL_QUERY_DB_PROJECT,
	    										NULL);
		
		ianjuta_symbol_query_set_fields (query,
	                                 G_N_ELEMENTS (query_fields),
	                                 query_fields, NULL);
		
		ianjuta_symbol_query_set_file_scope (query,
	                                     IANJUTA_SYMBOL_QUERY_SEARCH_FS_PUBLIC, NULL);

		ianjuta_symbol_query_set_mode (query,
	                               IANJUTA_SYMBOL_QUERY_MODE_SYNC, NULL);

		ianjuta_symbol_query_set_filters (query, IANJUTA_SYMBOL_TYPE_FUNCTION,  
	    							  TRUE, NULL);
		
		
		/* do the search */
		symbol_iter = ianjuta_symbol_query_search (query, symbol_name, NULL);
				
		if (symbol_iter &&
			ianjuta_iterable_get_length (symbol_iter, NULL) > 0)
		{
			GFile* file;
			symbol = IANJUTA_SYMBOL (symbol_iter);
			file = ianjuta_symbol_get_file (symbol, NULL);
			line = ianjuta_symbol_get_int (symbol, IANJUTA_SYMBOL_FIELD_FILE_POS, NULL);
			
			ianjuta_document_manager_goto_file_line (self->priv->document_manager, 
													file, line, NULL);
			
			g_object_unref (symbol_iter);
			g_object_unref (file);
		}

		g_object_unref (query);
	}
}

void 
gprof_view_refresh (GProfView *self)
{
	/* Don't refresh views if we don't have any data to work with */
	if (gprof_profile_data_has_data (self->priv->profile_data))
		GPROF_VIEW_GET_CLASS (self)->refresh (self);
}

GtkWidget *
gprof_view_get_widget (GProfView *self)
{
	return GPROF_VIEW_GET_CLASS (self)->get_widget (self);
}

void 
gprof_view_format_float (GtkTreeViewColumn *col,  GtkCellRenderer *renderer,
						 GtkTreeModel *model, GtkTreeIter *iter,
					     gpointer column_number)
{
	gfloat number;
	gchar *formatted_number;
	
	gtk_tree_model_get (model, iter, GPOINTER_TO_INT (column_number), &number,
						-1);
	
	formatted_number = g_strdup_printf ("%0.2f", number);
	g_object_set (renderer, "text", formatted_number, NULL);
	
	g_free (formatted_number);
}

