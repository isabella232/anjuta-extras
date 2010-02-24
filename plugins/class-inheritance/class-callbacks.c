/*
 *  Copyright (C) Massimo Cora' 2005 <maxcvs@email.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <glib.h>
#include <gio/gio.h>
#include <libanjuta/anjuta-debug.h>
#include <libanjuta/interfaces/ianjuta-document-manager.h>
#include <libanjuta/interfaces/ianjuta-symbol-manager.h>

#include "plugin.h"
#include "class-callbacks.h"
#include "class-inherit.h"

gint
on_canvas_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data) 
{
	AnjutaClassInheritance *plugin;
	plugin = ANJUTA_PLUGIN_CLASS_INHERITANCE (data);
	
	switch (event->type)
	{
	case GDK_BUTTON_PRESS:
		if (event->button.button == 3)
		{
			g_return_val_if_fail (plugin->menu != NULL, FALSE);
			
			gtk_menu_popup (GTK_MENU (plugin->menu), NULL, NULL, NULL, NULL, 
							event->button.button, event->button.time);
		}
		break;

	default:
		break;
	}
	
	return FALSE;
}

gint
on_nodedata_expanded_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data)
{
	AnjutaClassInheritance *plugin;
	NodeData *nodedata;	
	nodedata = (NodeData*)data;
	plugin = nodedata->plugin;

	switch (event->type)
	{
	case GDK_2BUTTON_PRESS:		/* double click */
		break;

	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1) {
			NodeExpansionStatus *node_status;
			if ((node_status = 
			     (NodeExpansionStatus*) g_tree_lookup (plugin->expansion_node_list, 
			                                           GINT_TO_POINTER (nodedata->klass_id))) 
				== NULL) 
			{
				break;
			}
			else if (strcmp (nodedata->sub_item, NODE_SHOW_ALL_MEMBERS_STR) == 0) 
			{
					node_status->expansion_status = NODE_FULL_EXPANDED;
					class_inheritance_update_graph (plugin);
			}			
			else 		/* it's a class member. Take line && uri of definition */
			{			/* and reach them */
				const gchar *file;
				gint line;
				
				file = g_object_get_data (G_OBJECT (item), "__filepath");
				line = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (item), "__line"));				
								
				if (file) 
				{
					GFile* gfile;
					
					gfile = g_file_new_for_path (file);
					
					/* Goto uri line */
					IAnjutaDocumentManager *dm;
					dm = anjuta_shell_get_interface (ANJUTA_PLUGIN (plugin)->shell,
					                                 IAnjutaDocumentManager, NULL);
					if (dm) 
					{
						ianjuta_document_manager_goto_file_line (dm, gfile, line, NULL);
					}
					
					if (gfile)
						g_object_unref (gfile);
				}
			}
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		gnome_canvas_item_set (nodedata->canvas_item,
							   "fill_color_gdk",
							   &plugin->canvas->style->base[GTK_STATE_SELECTED],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		gnome_canvas_item_set (nodedata->canvas_item,
							   "fill_color_gdk",
							   &plugin->canvas->style->base[GTK_STATE_NORMAL],
							   NULL);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
	
}


gint
on_collapsed_class_nodedata_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data)
{
	AnjutaClassInheritance *plugin;
	NodeData *nodedata;	
	
	nodedata = (NodeData*)data;
	plugin = nodedata->plugin;

	switch (event->type)
	{
	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1 && !nodedata->anchored)
		{
			NodeExpansionStatus *node_status;
			nodedata->anchored = TRUE;

			node_status = g_new0 (NodeExpansionStatus, 1);
			node_status->klass_id = nodedata->klass_id;
			/* set to half. This will display at least NODE_HALF_DISPLAY_ELEM_NUM.
			 * User will decide whether to show all elements or not. */
			node_status->expansion_status = NODE_HALF_EXPANDED;

			/* insert the class name to the hash_table */
			g_tree_insert (nodedata->plugin->expansion_node_list, 
			               GINT_TO_POINTER (nodedata->klass_id), 
			               node_status);
			class_inheritance_update_graph (nodedata->plugin);
			return TRUE;
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		/* Make the outline wide */
		gnome_canvas_item_set (nodedata->canvas_item,
							   "fill_color_gdk",
							   &plugin->canvas->style->base[GTK_STATE_SELECTED],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		/* Make the outline thin */
		gnome_canvas_item_set (nodedata->canvas_item,
							   "fill_color_gdk",
							   &plugin->canvas->style->base[GTK_STATE_NORMAL],
							   NULL);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
}

gint
on_expanded_class_nodedata_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data)
{
	AnjutaClassInheritance *plugin;
	NodeData *nodedata;	
	
	nodedata = (NodeData*)data;
	plugin = nodedata->plugin;

	switch (event->type)
	{
	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1 && nodedata->anchored)
		{
			nodedata->anchored = FALSE;
		
			/* remove the key from the hash table, if present */
			if (g_tree_lookup (nodedata->plugin->expansion_node_list, 
							   GINT_TO_POINTER (nodedata->klass_id))) 
			{
				g_tree_remove (nodedata->plugin->expansion_node_list, 
							   GINT_TO_POINTER (nodedata->klass_id));
			}
			class_inheritance_update_graph (nodedata->plugin);
			return TRUE;
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		/* Make the outline wide */
		gnome_canvas_item_set (nodedata->canvas_item,
							   "fill_color_gdk",
							   &plugin->canvas->style->bg[GTK_STATE_PRELIGHT],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		/* Make the outline thin */
		gnome_canvas_item_set (nodedata->canvas_item,
							   "fill_color_gdk",
							   &plugin->canvas->style->bg[GTK_STATE_ACTIVE],
							   NULL);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
}

/*----------------------------------------------------------------------------
 * callback for the canvas' right-click menu - update button.
 */
void
on_update_menu_item_selected (GtkMenuItem *item,
							  AnjutaClassInheritance *plugin)
{
	class_inheritance_update_graph (plugin);
}

/*----------------------------------------------------------------------------
 * callback for theme/colors changes
 */
void
on_style_set (GtkWidget *widget, GtkStyle  *previous_style,
			  AnjutaClassInheritance *plugin)
{
	class_inheritance_update_graph (plugin);
}
