/*
    style-editor.c
    Copyright (C) 2000  Naba Kumar <gnome.org>

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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <libanjuta/anjuta-utils.h>
#include <libanjuta/anjuta-debug.h>

#include "text_editor.h"
#include "style-editor.h"

#define string_assign(dest, src) g_free ((*dest)); (*dest) = g_strdup ((src));
#define GLADE_FILE PACKAGE_DATA_DIR "/glade/anjuta-editor-scintilla.ui"

gchar *hilite_style[] = {
	"Normal <Default>", "style.anjuta.normal",
	"White space", "style.anjuta.whitespace",
	"Comments", "style.anjuta.comment",
	"Numbers", "style.anjuta.number",
	"Operators","style.anjuta.operator",
	"Keywords", "style.anjuta.keyword",
	"System Keywords", "style.anjuta.syskeyword",
	"Local Keywords", "style.anjuta.localkeyword",
	"Additional Keywords", "style.anjuta.extrakeyword",
	"Characters", "style.anjuta.char",
	"Strings", "style.anjuta.string",
	"Raw Strings", "style.anjuta.raws",
	"Regular Expression", "style.anjuta.regex",
	"Unclosed Strings", "style.anjuta.unclosed",
	"Preprocessor Directives", "style.anjuta.preprocessor",
	"Errors", "style.anjuta.error",
	"Identifiers (Not C Style)", "style.anjuta.identifier",
	"Definitions (Not C Style)", "style.anjuta.definition",
	"Functions (Not C Style)", "style.anjuta.function",
	"Attribute (Not C Style)", "style.anjuta.attribute",
	"Matched Braces", "style.anjuta.matchbrace",
	"Incomplete Brace", "style.anjuta.singlebrace",
	"Control Characters", "style.anjuta.controlchar",
	"Line Numbers", "style.anjuta.linenumber",
	"Indentation Guides", "style.anjuta.indentguide",
	NULL, NULL
};

typedef struct _StyleData StyleData;

struct _StyleData
{
	gchar *item;
	gchar *font;
	gint size;
	gboolean bold, italics, underlined, eolfilled;
	gchar *fore, *back;

	gboolean font_use_default;
	gboolean attrib_use_default;
	gboolean fore_use_default;
	gboolean back_use_default;
};

struct _StyleEditorPriv
{
	/* Widgets */
	GtkWidget *dialog;
	GtkWidget *hilite_item_combobox;
	GtkWidget *font_picker;
	GtkWidget *font_bold_check;
	GtkWidget *font_italics_check;
	GtkWidget *font_underlined_check;
	GtkWidget *fore_colorpicker;
	GtkWidget *back_colorpicker;
	GtkWidget *font_use_default_check;
	GtkWidget *font_attrib_use_default_check;
	GtkWidget *fore_color_use_default_check;
	GtkWidget *back_color_use_default_check;
	GtkWidget *caret_fore_color;
	GtkWidget *selection_fore_color;
	GtkWidget *selection_back_color;
	GtkWidget *calltip_back_color;

	/* Data */
	StyleData gtk_style;
	StyleData *default_style;
	StyleData *current_style;

	/* Save all properties */
	GList *saved_props;
};

static gchar *
style_data_get_string (StyleData * sdata)
{
	gchar *tmp, *str;

	g_return_val_if_fail (sdata != NULL, NULL);

	str = NULL;
	if ((sdata->font) && strlen (sdata->font) && sdata->font_use_default == FALSE)
	{
		str = g_strconcat ("font:", sdata->font, NULL);
	}
	if (sdata->size > 0 && sdata->font_use_default == FALSE)
	{
		tmp = str;
		if (tmp)
		{
			str =
				g_strdup_printf ("%s,size:%d", tmp,
						 sdata->size);
			g_free (tmp);
		}
		else
			str = g_strdup_printf ("size:%d", sdata->size);

	}
	if (sdata->attrib_use_default == FALSE)
	{
		if (sdata->bold)
		{
			tmp = str;
			if (tmp)
			{
				str = g_strconcat (tmp, ",bold", NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("bold");
		}
		else
		{
			tmp = str;
			if (tmp)
			{
				str = g_strconcat (tmp, ",notbold", NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("notbold");
		}
		if (sdata->italics)
		{
			tmp = str;
			if (tmp)
			{
				str = g_strconcat (tmp, ",italics", NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("italics");
		}
		else
		{
			tmp = str;
			if (tmp)
			{
				str = g_strconcat (tmp, ",notitalics", NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("notitalics");
		}
		if (sdata->underlined)
		{
			tmp = str;
			if (tmp)
			{
				str = g_strconcat (tmp, ",underlined", NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("underlined");
		}
		else
		{
			tmp = str;
			if (tmp)
			{
				str =
					g_strconcat (tmp, ",notunderlined",
						     NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("notunderlined");
		}
		if (sdata->eolfilled)
		{
			tmp = str;
			if (tmp)
			{
				str = g_strconcat (tmp, ",eolfilled", NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("eolfilled");
		}
		else
		{
			tmp = str;
			if (tmp)
			{
				str =
					g_strconcat (tmp, ",noteolfilled",
						     NULL);
				g_free (tmp);
			}
			else
				str = g_strdup ("noteolfilled");
		}
	}
	if (sdata->fore_use_default == FALSE)
	{
		tmp = str;
		if (tmp)
		{
			str = g_strconcat (tmp, ",fore:", sdata->fore, NULL);
			g_free (tmp);
		}
		else
			str = g_strconcat ("fore:", sdata->fore, NULL);
	}
	if (sdata->back_use_default == FALSE)
	{
		tmp = str;
		if (tmp)
		{
			str = g_strconcat (tmp, ",back:", sdata->back, NULL);
			g_free (tmp);
		}
		else
			str = g_strconcat ("back:", sdata->back, NULL);
	}
	if (str == NULL)
		str = g_strdup ("");
	return str;
}

static void
style_data_set_font (StyleData * sdata, const gchar *font)
{
	PangoFontDescription *desc;
	const gchar *font_family;

	g_return_if_fail (sdata);

	desc = pango_font_description_from_string (font);
	font_family = pango_font_description_get_family(desc);
	string_assign (&sdata->font, font_family);

	/* Change to lower case */
	if (sdata->font)
	{
		gchar *s = sdata->font;
		while(*s)
		{
			*s = tolower(*s);
			s++;
		}
	}
	pango_font_description_free (desc);
}

static void
style_data_set_font_size_from_pango (StyleData * sdata, const gchar *font)
{
	PangoFontDescription *desc;

	g_return_if_fail (sdata);

	desc = pango_font_description_from_string (font);
	sdata->size = pango_font_description_get_size (desc) / PANGO_SCALE;
	pango_font_description_free (desc);
}

static void
style_data_set_fore (StyleData * sdata, const gchar *fore)
{
	g_return_if_fail (sdata);
	string_assign (&sdata->fore, fore);
}

static void
style_data_set_back (StyleData * sdata, const gchar *back)
{
	g_return_if_fail (sdata);
	string_assign (&sdata->back, back);
}

static void
style_data_set_item (StyleData * sdata, const gchar *item)
{
	g_return_if_fail (sdata);
	string_assign (&sdata->item, item);
}

static StyleData *
style_data_new (void)
{
	StyleData *sdata;
	sdata = g_new0 (StyleData, 1);

	sdata->font = g_strdup ("");
	sdata->size = 12;
	sdata->font_use_default = TRUE;
	sdata->attrib_use_default = TRUE;
	sdata->fore_use_default = TRUE;
	sdata->back_use_default = TRUE;
	style_data_set_fore (sdata, "#000000");	/* Black */
	style_data_set_back (sdata, "#FFFFFF");	/* White */
	return sdata;
}

static void
style_data_copy (StyleData *sdata, const StyleData *src)
{
	string_assign (&sdata->item, src->item);
	string_assign (&sdata->font, src->font);
	sdata->size = src->size;
	sdata->bold = src->bold;
	sdata->italics = src->italics;
	sdata->underlined = src->underlined;
	sdata->eolfilled = src->eolfilled;
	string_assign (&sdata->fore, src->fore);
	string_assign (&sdata->back, src->back);

	sdata->font_use_default = src->font_use_default;
	sdata->attrib_use_default = src->attrib_use_default;
	sdata->fore_use_default = src->fore_use_default;
	sdata->back_use_default = src->back_use_default;
}

static void
style_data_destroy (StyleData * sdata)
{
	if (!sdata)
		return;
	if (sdata->item)
		g_free (sdata->item);
	if (sdata->font)
		g_free (sdata->font);
	if (sdata->fore)
		g_free (sdata->fore);
	if (sdata->back)
		g_free (sdata->back);
	g_free (sdata);
}

void
style_data_parse (StyleData *style_data, gchar * style_string)
{
	gchar *val, *opt;

	val = (style_string != NULL)? g_strdup (style_string) : NULL;
	opt = val;
	while (opt)
	{
		gchar *cpComma, *colon;

		cpComma = strchr (opt, ',');
		if (cpComma)
			*cpComma = '\0';
		colon = strchr (opt, ':');
		if (colon)
			*colon++ = '\0';
		if (0 == strcmp (opt, "italics"))
		{
			style_data->italics = TRUE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "notitalics"))
		{
			style_data->italics = FALSE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "bold"))
		{
			style_data->bold = TRUE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "notbold"))
		{
			style_data->bold = FALSE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "font"))
		{
			style_data_set_font (style_data, colon);
			style_data->font_use_default = FALSE;
		}
		if (0 == strcmp (opt, "fore"))
		{
			style_data_set_fore (style_data, colon);
			style_data->fore_use_default = FALSE;
		}
		if (0 == strcmp (opt, "back"))
		{
			style_data_set_back (style_data, colon);
			style_data->back_use_default = FALSE;
		}
		if (0 == strcmp (opt, "size"))
		{
			style_data->size = atoi (colon);
			style_data->font_use_default = FALSE;
		}
		if (0 == strcmp (opt, "eolfilled"))
		{
			style_data->eolfilled = TRUE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "noteolfilled"))
		{
			style_data->eolfilled = FALSE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "underlined"))
		{
			style_data->underlined = TRUE;
			style_data->attrib_use_default = FALSE;
		}
		if (0 == strcmp (opt, "notunderlined"))
		{
			style_data->underlined = FALSE;
			style_data->attrib_use_default = FALSE;
		}
		if (cpComma)
			opt = cpComma + 1;
		else
			opt = NULL;
	}
	if (val)
		g_free (val);
	return style_data;
}

static void
sync_from_gtk (StyleEditor *se)
{
	GtkWidgetPath *path;
	GtkStyleContext *context;
	GdkRGBA fore;
	GdkRGBA back;
	const PangoFontDescription *desc;

	/* Get theme style information for view widget */
	path = gtk_widget_path_new ();
	gtk_widget_path_append_type (path, GTK_TYPE_WIDGET);
	context = gtk_style_context_new ();
	gtk_style_context_set_path (context, path);
	gtk_widget_path_free (path);
	gtk_style_context_add_class (context, GTK_STYLE_CLASS_VIEW);
	gtk_style_context_get_color (context, GTK_STATE_FLAG_NORMAL, &fore);
	gtk_style_context_get_background_color (context, GTK_STATE_FLAG_NORMAL, &back);
	desc = gtk_style_context_get_font (context, GTK_STATE_FLAG_NORMAL);

	se->priv->gtk_style.item = NULL;
	if (se->priv->gtk_style.font != NULL) g_free (se->priv->gtk_style.font);
	se->priv->gtk_style.font = g_strdup (pango_font_description_get_family(desc));
	se->priv->gtk_style.size = pango_font_description_get_size (desc) / PANGO_SCALE;
	se->priv->gtk_style.bold = pango_font_description_get_weight (desc) >= PANGO_WEIGHT_BOLD;
	se->priv->gtk_style.italics = pango_font_description_get_style (desc) != PANGO_STYLE_NORMAL;
	se->priv->gtk_style.underlined = FALSE;
	se->priv->gtk_style.eolfilled = FALSE;
	if (se->priv->gtk_style.fore != NULL) g_free (se->priv->gtk_style.fore);
	se->priv->gtk_style.fore = anjuta_util_string_from_color ((gint)(fore.red * 65535), (gint)(fore.green * 65535), (gint)(fore.blue *65535));
	if (se->priv->gtk_style.back != NULL) g_free (se->priv->gtk_style.back);
	se->priv->gtk_style.back = anjuta_util_string_from_color ((gint)(back.red * 65535), (gint)(back.green * 65535), (gint)(back.blue *65535));
	g_object_unref (context);

	se->priv->gtk_style.font_use_default = TRUE;
	se->priv->gtk_style.attrib_use_default = TRUE;
	se->priv->gtk_style.fore_use_default = TRUE;
	se->priv->gtk_style.back_use_default = TRUE;
}

static void
sync_from_props (StyleEditor *se)
{
	gint i;
	gchar *str;

	g_return_if_fail (se);
	/* Never hurts to use g_object_*_data as temp hash buffer */
	for (i = 0;; i += 2)
	{
		StyleData *sdata;

		if (hilite_style[i] == NULL)
			break;
		str = sci_prop_get_expanded (se->props, hilite_style[i + 1]);
		sdata = style_data_new ();
		/* Copy GTK style in the first style */
		if (i == 0) style_data_copy (sdata, &se->priv->gtk_style);
		style_data_parse (sdata, str);
		/* DEBUG_PRINT ("Parsing %s => %s", hilite_style[i + 1], str); */
		if (str)
			g_free (str);

		style_data_set_item (sdata, hilite_style[i]);
		g_object_set_data_full (G_OBJECT (se->priv->dialog),
					  hilite_style[i], sdata,
					  (GDestroyNotify)style_data_destroy);
	}
	se->priv->default_style =
		g_object_get_data (G_OBJECT (se->priv->dialog),
				     hilite_style[0]);
	se->priv->current_style = NULL;

	str = sci_prop_get (se->props, CARET_FORE_COLOR);
	if(str)
	{
		GdkColor color;

		gdk_color_parse (str, &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->caret_fore_color),
									&color);
		g_free (str);
	}
	else
	{
		GdkColor color;

		gdk_color_parse ("#000000", &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->caret_fore_color),
									&color);
	}

	str = sci_prop_get (se->props, CALLTIP_BACK_COLOR);
	if(str)
	{
		GdkColor color;

		gdk_color_parse (str, &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->calltip_back_color),
									&color);
		g_free (str);
	}
	else
	{
		GdkColor color;

		gdk_color_parse ("#E6D6B6", &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->calltip_back_color),
									&color);
	}

	str = sci_prop_get (se->props, SELECTION_FORE_COLOR);
	if(str)
	{
		GdkColor color;

		gdk_color_parse (str, &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->selection_fore_color),
									&color);
		g_free (str);
	}
	else
	{
		GdkColor color;

		gdk_color_parse ("#FFFFFF", &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->selection_fore_color),
									&color);
	}

	str = sci_prop_get (se->props, SELECTION_BACK_COLOR);
	if(str)
	{
		GdkColor color;

		gdk_color_parse (str, &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->selection_back_color),
									&color);
		g_free (str);
	}
	else
	{
		GdkColor color;

		gdk_color_parse ("#000000", &color);
		gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->selection_back_color),
									&color);
	}
}

static void
set_one_color (PropsID props, gchar *key, GtkWidget *picker)
{
	GdkColor color;
	gchar *str;
	gtk_color_button_get_color (GTK_COLOR_BUTTON (picker), &color);
	str = anjuta_util_string_from_color (color.red,
										 color.green,
										 color.blue);
	if(str)
	{
		sci_prop_set_with_key (props, key, str);
		g_free (str);
	}
}

static void
sync_to_props (StyleEditor *se)
{
	gint i;
	gchar *str;

	g_return_if_fail (se);

	/* Transfer to props */
	for (i = 0;; i += 2)
	{
		StyleData *sdata;

		if (hilite_style[i] == NULL)
			break;
		sdata =
			g_object_get_data (G_OBJECT (se->priv->dialog),
								 hilite_style[i]);
		str = style_data_get_string (sdata);
		if (str)
		{
			sci_prop_set_with_key (se->props, hilite_style[i + 1], str);
			g_free (str);
		}
	}
	set_one_color (se->props, CARET_FORE_COLOR,
				   se->priv->caret_fore_color);

	set_one_color (se->props, CALLTIP_BACK_COLOR,
				   se->priv->calltip_back_color);

	set_one_color (se->props, SELECTION_FORE_COLOR,
				   se->priv->selection_fore_color);

	set_one_color (se->props, SELECTION_BACK_COLOR,
				   se->priv->selection_back_color);
}

/* Select a new style, update all entries */
static void
on_hilite_style_item_changed (StyleEditor *se)
{
	gchar *style_item;
	StyleData *used_style;
	StyleData *current_style;
	StyleData *default_style;
	PangoFontDescription *desc;
	gchar *font_name;
	GdkColor color;



	/* Find current style */
	style_item = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (se->priv->hilite_item_combobox));
	if (style_item != NULL)
	{
		current_style = g_object_get_data (G_OBJECT (se->priv->dialog), style_item);
		g_free (style_item);
	}

	g_return_if_fail (current_style);

	/* Disable current style to avoid changed notification */
	se->priv->current_style = NULL;

	/* Default style is Gtk for first style or first style for other one */
	default_style = current_style == se->priv->default_style ? &se->priv->gtk_style : se->priv->default_style;

	/* Get font */
	used_style = ((current_style->font_use_default) ||
	              (current_style->font == NULL) ||
	              (*(current_style->font) == '\0')) ? default_style : current_style;
	desc = pango_font_description_from_string (used_style->font);
	pango_font_description_set_size (desc, used_style->size * PANGO_SCALE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->font_use_default_check), used_style != current_style);
	gtk_widget_set_sensitive (se->priv->font_use_default_check, TRUE);
	gtk_widget_set_sensitive (se->priv->font_picker, used_style == current_style);

	/* Get attribute */
	used_style = current_style->attrib_use_default ?
		default_style : current_style;
	if (used_style->bold) pango_font_description_set_weight (desc, PANGO_WEIGHT_BOLD);
	if (used_style->italics) pango_font_description_set_style (desc, PANGO_STYLE_ITALIC);

	font_name = pango_font_description_to_string (desc);
	pango_font_description_free (desc);
	gtk_font_button_set_font_name (GTK_FONT_BUTTON (se->priv->font_picker), font_name);
	g_free (font_name);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->font_attrib_use_default_check), used_style != current_style);
	gtk_widget_set_sensitive (se->priv->font_attrib_use_default_check, TRUE);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->font_bold_check), used_style->bold);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->font_italics_check), used_style->italics);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->font_underlined_check), used_style->underlined);
	gtk_widget_set_sensitive (se->priv->font_bold_check, used_style == current_style);
	gtk_widget_set_sensitive (se->priv->font_italics_check, used_style == current_style);
	gtk_widget_set_sensitive (se->priv->font_underlined_check, used_style == current_style);


	/* Get foreground color */
	used_style = current_style->fore_use_default ?
		default_style : current_style;
	gdk_color_parse (used_style->fore, &color);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->fore_color_use_default_check), used_style != current_style);
	gtk_widget_set_sensitive (se->priv->fore_color_use_default_check, TRUE);

	gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->fore_colorpicker), &color);
	gtk_widget_set_sensitive (se->priv->fore_colorpicker, used_style == current_style);

	/* Get background color */
	used_style = current_style->back_use_default ?
		default_style : current_style;
	gdk_color_parse (used_style->back, &color);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(se->priv->back_color_use_default_check), used_style != current_style);
	gtk_widget_set_sensitive (se->priv->back_color_use_default_check, TRUE);

	gtk_color_button_set_color (GTK_COLOR_BUTTON (se->priv->back_colorpicker), &color);
	gtk_widget_set_sensitive (se->priv->back_colorpicker, used_style == current_style);

	/* Set current style after setting all entries */
	se->priv->current_style = current_style;
}

/* Font button allow to select bold and italic attribute,
 * so update those check buttons too */
static void
on_font_changed (StyleEditor *se)
{
	const gchar *font;

	if (se->priv->current_style == NULL) return;

	font = gtk_font_button_get_font_name (GTK_FONT_BUTTON (se->priv->font_picker));
	if (font)
	{
		PangoFontDescription *desc = pango_font_description_from_string (font);
		gboolean bold;
		gboolean italic;
		gboolean underline;

		bold = pango_font_description_get_weight (desc) >= PANGO_WEIGHT_BOLD;
		italic = pango_font_description_get_style (desc) != PANGO_STYLE_NORMAL;
		pango_font_description_free (desc);

		style_data_set_font (se->priv->current_style, font);
		style_data_set_font_size_from_pango (se->priv->current_style, font);
		se->priv->current_style->bold = bold;
		se->priv->current_style->italics = italic;

		underline = se->priv->current_style->attrib_use_default ? se->priv->default_style->underlined :se->priv->current_style->underlined;
		se->priv->current_style->attrib_use_default = ((bold == se->priv->default_style->bold) &&
		                                               (italic == se->priv->default_style->italics) &&
		                                               (underline == se->priv->default_style->underlined) &&
		                                               (se->priv->current_style != se->priv->default_style));

		/* Changing bold and italic attribute could need a change in font */
		on_hilite_style_item_changed (se);

		sync_to_props (se);
		g_signal_emit_by_name (se->plugin, "style-changed");
	}
}

/* Change a style entry, store values */
static void
on_hilite_style_entry_changed (StyleEditor *se)
{
	GdkColor color;
	gchar *str;
	const gchar *font;

	g_return_if_fail (se);

	if (se->priv->current_style == NULL) return;

	font = gtk_font_button_get_font_name (GTK_FONT_BUTTON
											(se->priv->font_picker));
	if (font)
	{
		style_data_set_font (se->priv->current_style, font);
		style_data_set_font_size_from_pango (se->priv->current_style, font);
	}
	else
	{
		style_data_set_font (se->priv->current_style,
							 se->priv->default_style->font);
		se->priv->current_style->size = se->priv->default_style->size;
	}

	se->priv->current_style->bold =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->font_bold_check));
	se->priv->current_style->italics =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->font_italics_check));
	se->priv->current_style->underlined =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->font_underlined_check));

	gtk_color_button_get_color (GTK_COLOR_BUTTON (se->priv->fore_colorpicker),
								&color);
	str = anjuta_util_string_from_color (color.red,
										 color.green,
										 color.blue);
	style_data_set_fore (se->priv->current_style, str);
	g_free (str);

	gtk_color_button_get_color (GTK_COLOR_BUTTON (se->priv->back_colorpicker),
								&color);
	str = anjuta_util_string_from_color (color.red,
										 color.green,
										 color.blue);
	style_data_set_back (se->priv->current_style, str);
	g_free (str);

	se->priv->current_style->font_use_default =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->font_use_default_check));
	se->priv->current_style->attrib_use_default =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->font_attrib_use_default_check));
	se->priv->current_style->fore_use_default =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->fore_color_use_default_check));
	se->priv->current_style->back_use_default =
		gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					      (se->priv->back_color_use_default_check));

	/* Changing bold and italic attribute could need a change in font */
	on_hilite_style_item_changed (se);

	sync_to_props (se);
	g_signal_emit_by_name (se->plugin, "style-changed");
}

static void
save_props (StyleEditor *se)
{
	gint i;
	gchar *str;

	g_return_if_fail (se);

	for (i = 0;; i += 2)
	{
		StyleData *sdata;

		if (hilite_style[i] == NULL)
			break;
		str = sci_prop_get_expanded (se->props, hilite_style[i + 1]);
		se->priv->saved_props = g_list_prepend (se->priv->saved_props, str);
	}

	str = sci_prop_get (se->props, CARET_FORE_COLOR);
	se->priv->saved_props = g_list_prepend (se->priv->saved_props, str);
	str = sci_prop_get (se->props, CALLTIP_BACK_COLOR);
	se->priv->saved_props = g_list_prepend (se->priv->saved_props, str);
	str = sci_prop_get (se->props, SELECTION_FORE_COLOR);
	se->priv->saved_props = g_list_prepend (se->priv->saved_props, str);
	str = sci_prop_get (se->props, SELECTION_BACK_COLOR);
	se->priv->saved_props = g_list_prepend (se->priv->saved_props, str);

	se->priv->saved_props = g_list_reverse (se->priv->saved_props);
}

static void
restore_props (StyleEditor *se)
{
	gint i;
	GList *str;

	g_return_if_fail (se);

	/* Transfer to props */
	str = g_list_first (se->priv->saved_props);
	for (i = 0;; i += 2)
	{
		StyleData *sdata;

		if (hilite_style[i] == NULL)
			break;

		if (str->data) sci_prop_set_with_key (se->props, hilite_style[i + 1], (gchar *)str->data);
		str = g_list_next (str);
	}
	sci_prop_set_with_key (se->props, CARET_FORE_COLOR, (gchar *)str->data);
	str = g_list_next (str);
	sci_prop_set_with_key (se->props, CALLTIP_BACK_COLOR, (gchar *)str->data);
	str = g_list_next (str);
	sci_prop_set_with_key (se->props, SELECTION_FORE_COLOR, (gchar *)str->data);
	str = g_list_next (str);
	sci_prop_set_with_key (se->props, SELECTION_BACK_COLOR, (gchar *)str->data);
}

static void
free_saved_props (StyleEditor *se)
{
	g_return_if_fail (se);

	g_list_foreach (se->priv->saved_props, (GFunc)g_free, NULL);
	g_list_free (se->priv->saved_props);
	se->priv->saved_props = NULL;
}

static void
apply_styles (StyleEditor *se)
{
	FILE *ofile;
	gchar *filename;

	filename = anjuta_util_get_user_config_file_path ("scintilla","editor-style.properties",NULL);
	ofile = fopen (filename, "w");
	if (!ofile) {
		DEBUG_PRINT ("Could not open %s for writing", filename);
	} else {
		style_editor_save (se, ofile);
		fclose (ofile);
		g_free (filename);
	}
}


static void
cancel_styles (StyleEditor *se)
{
	FILE *ofile;
	gchar *filename;

	restore_props (se);
	g_signal_emit_by_name (se->plugin, "style-changed");
}

static void
on_response (GtkWidget *dialog, gint res, StyleEditor *se)
{
	g_return_if_fail (se);

	switch (res)
	{
	case GTK_RESPONSE_OK:
		apply_styles (se);
		style_editor_hide (se);
		break;
	case GTK_RESPONSE_CANCEL:
		cancel_styles (se);
		style_editor_hide (se);
		break;
	}

	return;
}

static void
on_delete_event (GtkWidget *dialog, GdkEvent *event, StyleEditor *se)
{
	g_return_if_fail (se);
	style_editor_hide (se);
}

static void
create_style_editor_gui (StyleEditor * se)
{
	GtkBuilder *bxml = gtk_builder_new ();
	GtkWidget *pref_dialog;
	GtkListStore *store;
	gint i;
	GError* error = NULL;

	g_return_if_fail (se);
	g_return_if_fail (se->priv->dialog == NULL);

	if (!gtk_builder_add_from_file (bxml, GLADE_FILE, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	se->priv->dialog = GTK_WIDGET (gtk_builder_get_object (bxml, "style_editor_dialog"));
	gtk_widget_show (se->priv->dialog);
	se->priv->font_picker = GTK_WIDGET (gtk_builder_get_object (bxml, "font"));
	se->priv->hilite_item_combobox = GTK_WIDGET (gtk_builder_get_object (bxml, "comboBox"));
	se->priv->font_bold_check = GTK_WIDGET (gtk_builder_get_object (bxml, "bold"));
	se->priv->font_italics_check = GTK_WIDGET (gtk_builder_get_object (bxml, "italic"));
	se->priv->font_underlined_check = GTK_WIDGET (gtk_builder_get_object (bxml, "underlined"));
	se->priv->fore_colorpicker = GTK_WIDGET (gtk_builder_get_object (bxml, "fore_color"));
	se->priv->back_colorpicker = GTK_WIDGET (gtk_builder_get_object (bxml, "back_color"));
	se->priv->font_use_default_check = GTK_WIDGET (gtk_builder_get_object (bxml, "font_default"));
	se->priv->font_attrib_use_default_check = GTK_WIDGET (gtk_builder_get_object (bxml, "attributes_default"));
	se->priv->fore_color_use_default_check = GTK_WIDGET (gtk_builder_get_object (bxml, "fore_default"));
	se->priv->back_color_use_default_check = GTK_WIDGET (gtk_builder_get_object (bxml, "back_default"));
	se->priv->caret_fore_color = GTK_WIDGET (gtk_builder_get_object (bxml, "caret"));
	se->priv->calltip_back_color = GTK_WIDGET (gtk_builder_get_object (bxml, "calltip"));
	se->priv->selection_fore_color = GTK_WIDGET (gtk_builder_get_object (bxml, "selection_fore"));
	se->priv->selection_back_color = GTK_WIDGET (gtk_builder_get_object (bxml, "selection_back"));

	/* Fill combo box with modules */
    store = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_combo_box_set_entry_text_column (GTK_COMBO_BOX (se->priv->hilite_item_combobox), 0);

	for (i = 0;; i += 2)
	{
		GtkTreeIter iter;

		if (hilite_style[i] == NULL)
			break;
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, hilite_style[i], -1);
	}
    gtk_combo_box_set_model (GTK_COMBO_BOX(se->priv->hilite_item_combobox), GTK_TREE_MODEL(store));
    g_object_unref (store);
	gtk_combo_box_set_active (GTK_COMBO_BOX (se->priv->hilite_item_combobox), 0);

	pref_dialog = anjuta_preferences_get_dialog (se->prefs);
	gtk_window_set_transient_for (GTK_WINDOW (se->priv->dialog),
								  GTK_WINDOW (pref_dialog));

	g_signal_connect_swapped (se->priv->hilite_item_combobox, "changed", G_CALLBACK (on_hilite_style_item_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->font_use_default_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->font_picker),
					  "font-set", G_CALLBACK (on_font_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->font_attrib_use_default_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->font_bold_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->font_italics_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->font_underlined_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->fore_color_use_default_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->fore_colorpicker),
					  "color-set", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->back_color_use_default_check),
					  "toggled", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect_swapped (G_OBJECT (se->priv->back_colorpicker),
					  "color-set", G_CALLBACK (on_hilite_style_entry_changed),
					  se);
	g_signal_connect (G_OBJECT (se->priv->dialog),
					  "delete_event", G_CALLBACK (on_delete_event),
					  se);
	g_signal_connect (G_OBJECT (se->priv->dialog),
					  "response", G_CALLBACK (on_response),
					  se);
	g_object_unref (bxml);
}

StyleEditor *
style_editor_new (AnjutaPlugin *plugin, AnjutaPreferences *prefs, GSettings *settings)
{
	StyleEditor *se;
	se = g_new0 (StyleEditor, 1);
	se->priv = g_new0 (StyleEditorPriv, 1);
	se->props = text_editor_get_props ();
	se->priv->dialog = NULL;
	se->priv->saved_props = NULL;
	se->prefs = prefs;
	se->settings = g_object_ref (settings);
	se->plugin = g_object_ref (plugin);
	return se;
}

void style_editor_destroy (StyleEditor *se)
{
	g_return_if_fail (se);

	g_free (se->priv->gtk_style.item);
	g_free (se->priv->gtk_style.font);
	g_free (se->priv->gtk_style.fore);
	g_free (se->priv->gtk_style.back);

	if (se->priv->dialog)
		gtk_widget_destroy (se->priv->dialog);
	free_saved_props (se);
	g_free (se->priv);
	g_object_unref (se->settings);
	g_object_unref (se->plugin);
	g_free (se);
}

void style_editor_show (StyleEditor *se)
{
	g_return_if_fail (se);
	if (se->priv->dialog)
		return;
	create_style_editor_gui (se);
	sync_from_gtk (se);
	sync_from_props (se);
	save_props (se);
	on_hilite_style_item_changed (se);
}

void style_editor_hide (StyleEditor *se)
{
	g_return_if_fail (se);
	g_return_if_fail (se->priv->dialog);
	gtk_widget_destroy (se->priv->dialog);
	se->priv->dialog = NULL;
	free_saved_props (se);
}

void
style_editor_save (StyleEditor *se, FILE *fp)
{
	gint i;
	gchar *str;

	for (i = 0;; i += 2)
	{
		if (hilite_style[i] == NULL)
			break;
		str = sci_prop_get (se->props, hilite_style[i + 1]);
		if (str)
		{
			fprintf (fp, "%s=%s\n", hilite_style[i + 1], str);
			g_free (str);
		}
		/* else
			fprintf (fp, "%s=\n", hilite_style[i + 1]); */
	}
	str = sci_prop_get (se->props, CARET_FORE_COLOR);
	if(str)
	{
		fprintf (fp, "%s=%s\n", CARET_FORE_COLOR, str);
		g_free (str);
	}

	str = sci_prop_get (se->props, CALLTIP_BACK_COLOR);
	if(str)
	{
		fprintf (fp, "%s=%s\n", CALLTIP_BACK_COLOR, str);
		g_free (str);
	}

	str = sci_prop_get (se->props, SELECTION_FORE_COLOR);
	if(str)
	{
		fprintf (fp, "%s=%s\n", SELECTION_FORE_COLOR, str);
		g_free (str);
	}

	str = sci_prop_get (se->props, SELECTION_BACK_COLOR);
	if(str)
	{
		fprintf (fp, "%s=%s\n", SELECTION_BACK_COLOR, str);
		g_free (str);
	}
}
