
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <Platform.h>
#include <Scintilla.h>
#include <ScintillaWidget.h>


static int IntFromHexDigit(int ch) {
        if ((ch >= '0') && (ch <= '9')) {
                return ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
                return ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'f') {
                return ch - 'a' + 10;
        } else {
                return 0;
        }
}

static int IntFromHexByte(const char *hexByte) {
        return IntFromHexDigit(hexByte[0]) * 16 + IntFromHexDigit(hexByte[1]);
}

typedef long Colour;
Colour ColourRGB(unsigned int red, unsigned int green, unsigned int blue)
{
	return red | (green << 8) | (blue << 16);
}

static Colour ColourFromString(const char *s) {
        if ((s != NULL) && (*s != '\0')) {
                int r = IntFromHexByte(s + 1);
                int g = IntFromHexByte(s + 3);
                int b = IntFromHexByte(s + 5);
                return ColourRGB(r, g, b);
        } else {
                return 0;
        }
}

void SetOneStyle(GtkWidget *sci, int style, const char *s) {
	char *val = strdup(s);
	char *opt = val;
	while (opt) {
		char *cpComma = strchr(opt, ',');
		if (cpComma)
			*cpComma = '\0';
		char *colon = strchr(opt, ':');
		if (colon)
			*colon++ = '\0';
		if (0 == strcmp(opt, "italics"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETITALIC, style, 1);
		if (0 == strcmp(opt, "notitalics"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETITALIC, style, 0);
		if (0 == strcmp(opt, "bold"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETBOLD, style, 1);
		if (0 == strcmp(opt, "notbold"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETBOLD, style, 0);
		if (0 == strcmp(opt, "font"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETFONT, style, reinterpret_cast<long>(colon));
		if (0 == strcmp(opt, "fore"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETFORE, style, ColourFromString(colon));
		if (0 == strcmp(opt, "back"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETBACK, style, ColourFromString(colon));
		if (0 == strcmp(opt, "size"))
				scintilla_send_message(SCINTILLA(sci), SCI_STYLESETSIZE, style, atoi(colon));
		if (0 == strcmp(opt, "eolfilled"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETEOLFILLED, style, 1);
		if (0 == strcmp(opt, "noteolfilled"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETEOLFILLED, style, 0);
		if (0 == strcmp(opt, "underlined"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETUNDERLINE, style, 1);
		if (0 == strcmp(opt, "notunderlined"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETUNDERLINE, style, 0);
		if (0 == strcmp(opt, "case")) {
			if (*colon == 'u') {
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETCASE, style, SC_CASE_UPPER);
			} else if (*colon == 'l') {
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETCASE, style, SC_CASE_LOWER);
			} else {
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETCASE, style, SC_CASE_MIXED);
			}
		}
		if (0 == strcmp(opt, "visible"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETVISIBLE, style, 1);
		if (0 == strcmp(opt, "notvisible"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETVISIBLE, style, 0);
		if (0 == strcmp(opt, "changeable"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETCHANGEABLE, style, 1);
		if (0 == strcmp(opt, "notchangeable"))
			scintilla_send_message(SCINTILLA(sci), SCI_STYLESETCHANGEABLE, style, 0);
		if (cpComma)
			opt = cpComma + 1;
		else
			opt = 0;
	}
	if (val)
		delete []val;
	// scintilla_send_message(SCINTILLA(sci), SCI_STYLESETCHARACTERSET, style, characterSet);
}

int main(int argc, char **argv)
{
	GtkWidget *sci;
	GtkWidget *win;
	FILE *fp;
	size_t read_bytes = 0;
	char buffer[1025];
	
	gtk_init (&argc, &argv);
	if (argc != 2)
	{
		printf ("Usage: test-scintilla filename\n");
		exit(1);
	}
	win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	sci = scintilla_new();
	SetOneStyle (sci, STYLE_DEFAULT,
		"font:bitstream vera sans mono,size:10,back:#EFEFEF");
	scintilla_send_message(SCINTILLA(sci), SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
	scintilla_send_message(SCINTILLA(sci), SCI_SETHSCROLLBAR, 0, 0);
	scintilla_send_message(SCINTILLA(sci), SCI_SETCODEPAGE, SC_CP_UTF8, 0);
	scintilla_send_message(SCINTILLA(sci), SCI_SETMARGINWIDTHN, 0, 40);
	scintilla_set_id(SCINTILLA(sci), 0);
	gtk_container_add (GTK_CONTAINER(win), sci);
	g_signal_connect (G_OBJECT (win), "delete-event",
					  G_CALLBACK (gtk_main_quit), NULL);
	gtk_widget_set_size_request (GTK_WIDGET (win), 500, 600);
	gtk_window_set_default_size (GTK_WINDOW (win), 500, 600);
	gtk_widget_show (sci);
	gtk_widget_show (win);
	fp = fopen (argv[1], "r");
	if (!fp)
	{
		perror ("Unable to open file\n");
		exit(1);
	}
	while ((read_bytes = fread (buffer, 1, 1024, fp)) > 0)
	{
		buffer[read_bytes] = '\0';
		scintilla_send_message(SCINTILLA(sci), SCI_APPENDTEXT, read_bytes, reinterpret_cast<long>(buffer));
	}
	fclose (fp);
	gtk_main();
	return 0;
}
