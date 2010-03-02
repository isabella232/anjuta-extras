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
on_expanded_class_title_event (GnomeCanvasItem *item, GdkEvent *event,
                               ClsNode *cls_node)
{
	switch (event->type)
	{
	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1 &&
		    cls_node->expansion_status != CLS_NODE_COLLAPSED &&
		    cls_node_collapse (cls_node))
		{
			cls_inherit_draw(cls_node->plugin);
			return TRUE;
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		/* Make the outline wide */
		gnome_canvas_item_set (item,
							   "fill_color_gdk",
							   &cls_node->canvas->style->bg[GTK_STATE_PRELIGHT],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		/* Make the outline thin */
		gnome_canvas_item_set (item,
							   "fill_color_gdk",
							   &cls_node->canvas->style->bg[GTK_STATE_ACTIVE],
							   NULL);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

gint
on_expanded_class_item_event (GnomeCanvasItem *item, GdkEvent *event,
                              gpointer data)
{
	AnjutaClassInheritance *plugin;
	ClsNodeItem *node_item;
	
	node_item = (ClsNodeItem*)data;
	plugin = node_item->cls_node->plugin;

	switch (event->type)
	{
	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1)
		{
			/* Goto uri line */
			if (node_item->file) 
			{
				IAnjutaDocumentManager *dm;
				dm = anjuta_shell_get_interface (ANJUTA_PLUGIN (plugin)->shell,
				                                 IAnjutaDocumentManager, NULL);
				if (dm) 
					ianjuta_document_manager_goto_file_line (dm,
					                                         node_item->file,
					                                         node_item->line,
					                                         NULL);
			}
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		gnome_canvas_item_set (node_item->canvas_node_item,
							   "fill_color_gdk",
							   &node_item->cls_node->canvas->style->base[GTK_STATE_SELECTED],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		gnome_canvas_item_set (node_item->canvas_node_item,
							   "fill_color_gdk",
							   &node_item->cls_node->canvas->style->base[GTK_STATE_NORMAL],
							   NULL);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

gint
on_expanded_class_more_event (GnomeCanvasItem *item, GdkEvent *event,
                              ClsNode *cls_node)
{
	switch (event->type)
	{
	case GDK_2BUTTON_PRESS:		/* double click */
		break;

	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1 &&
		    cls_node->expansion_status == CLS_NODE_SEMI_EXPANDED &&
		    cls_node_expand (cls_node, CLS_NODE_FULL_EXPANDED))
		{
			cls_inherit_draw(cls_node->plugin);
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		gnome_canvas_item_set (item,
							   "fill_color_gdk",
							   &cls_node->canvas->style->bg[GTK_STATE_PRELIGHT],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		gnome_canvas_item_set (item,
							   "fill_color_gdk",
		                       &cls_node->canvas->style->bg[GTK_STATE_ACTIVE],
							   NULL);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
}

gint
on_collapsed_class_event (GnomeCanvasItem *item, GdkEvent *event, gpointer data)
{
	AnjutaClassInheritance *plugin;
	ClsNode *cls_node;
	
	cls_node = (ClsNode*)data;
	plugin = cls_node->plugin;

	switch (event->type)
	{
	case GDK_BUTTON_PRESS:		/* single click */
		if (event->button.button == 1)
		{
			if (cls_node_expand (cls_node, CLS_NODE_SEMI_EXPANDED))
			{
			    cls_inherit_draw(plugin);
		    }
			return TRUE;
		}
		break;
		
	case GDK_ENTER_NOTIFY:		/* mouse entered in item's area */
		/* Make the outline wide */
		gnome_canvas_item_set (item,
							   "fill_color_gdk",
							   &plugin->canvas->style->base[GTK_STATE_SELECTED],
							   NULL);
		return TRUE;

	case GDK_LEAVE_NOTIFY:		/* mouse exited item's area */
		/* Make the outline thin */
		gnome_canvas_item_set (item,
							   "fill_color_gdk",
							   &plugin->canvas->style->base[GTK_STATE_NORMAL],
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
	cls_inherit_update (plugin);
}

/*----------------------------------------------------------------------------
 * callback for theme/colors changes
 */
void
on_style_set (GtkWidget *widget, GtkStyle  *previous_style,
			  AnjutaClassInheritance *plugin)
{
	cls_inherit_update (plugin);
}
