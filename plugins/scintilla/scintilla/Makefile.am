## Process this file with automake to produce Makefile.in

SUBDIRS = include

noinst_LTLIBRARIES = libanjuta-scintilla.la

AM_CPPFLAGS = \
	-I$(srcdir)/include \
	$(LIBANJUTA_CFLAGS) \
	$(GTHREAD_CFLAGS) \
	-DGTK -DSCI_LEXER -DUSE_XIM

AM_CXXFLAGS = -DGTK -DSCI_LEXER -DUSE_XIM

include $(srcdir)/lexers.make

libanjuta_scintilla_la_SOURCES =\
	AutoComplete.cxx \
	CallTip.cxx \
	CellBuffer.cxx \
	CharClassify.cxx \
	ContractionState.cxx \
	Decoration.cxx \
	DocumentAccessor.cxx \
	Document.cxx \
	Editor.cxx \
	ExternalLexer.cxx \
	Indicator.cxx \
	KeyMap.cxx \
	KeyWords.cxx \
	LineMarker.cxx \
	PerLine.cxx \
	PlatGTK.cxx \
	PositionCache.cxx \
	PropSet.cxx \
	RESearch.cxx \
	RunStyles.cxx \
	ScintillaBase.cxx \
	ScintillaGTK.cxx \
	Selection.cxx \
	StyleContext.cxx\
	Style.cxx \
	UniConversion.cxx \
	ViewStyle.cxx \
	WindowAccessor.cxx \
	XPM.cxx\
	AutoComplete.h \
	CallTip.h \
	CellBuffer.h \
	CharacterSet.h \
	CharClassify.h \
	ContractionState.h \
	Converter.h \
	Decoration.h \
	DocumentAccessor.h \
	Document.h \
	Editor.h \
	ExternalLexer.h \
	Indicator.h \
	KeyMap.h \
	LineMarker.h \
	Partitioning.h \
	PerLine.h \
	PositionCache.h \
	PropSetSimple.h \
	RESearch.h \
	RunStyles.h \
	ScintillaBase.h \
	Selection.h \
	SplitVector.h \
	StyleContext.h\
	Style.h \
	SVector.h \
	UniConversion.h \
	ViewStyle.h\
	XPM.h\
	scintilla-marshal.h\
	scintilla-marshal.c\
	$(LEXER_SRCS)

libanjuta_scintilla_la_LIBADD = \
	$(LIBANJUTA_LIBS) \
	$(GTHREAD_LIBS)

noinst_PROGRAMS = test-scintilla

test_scintilla_CXXFLAGS = -DGTK
test_scintilla_SOURCES = test-scintilla.cxx
test_scintilla_LDADD = libanjuta-scintilla.la

update-scintilla:
	cd $(srcdir); \
	cp scintilla/gtk/*.cxx .; \
	cp scintilla/gtk/*.h .; \
	cp scintilla/src/*.cxx .; \
	cp scintilla/src/*.h .; \
	cp scintilla/include/*.h include; \
	cp scintilla/include/*.py include; \
	cp scintilla/include/*.iface include; \
	echo "## Lexers make file" > lexers.make; \
	echo 'LEXER_OBJS = \' >> lexers.make; \
	echo -n '	StyleContext.o' >> lexers.make; \
	for lex in Lex*.cxx; do \
		lex=`echo $$lex | sed -e "s,.*/,," -e "s/cxx$$/o/"`; \
		echo '\' >> lexers.make; \
		echo -n "	$$lex" >> lexers.make; \
	done; \
	echo "" >> lexers.make; \
	echo "" >> lexers.make; \
	echo -n 'LEXER_SRCS = ' >> lexers.make; \
	for lex in Lex*.cxx; do \
		echo '\' >> lexers.make; \
		echo -n "	$$lex" >> lexers.make; \
	done; \
	echo "" >> lexers.make
	echo "Patching files ..."
	cd $(srcdir); \
	for patchfile in patches/*.diff; do \
		patch -p1 < $$patchfile; \
	done
	echo "Updating properties file..."
	cd $(srcdir); \
	./update-properties.pl ../properties/styles.properties scite/src

marshallers: scintilla-marshal.list
	glib-genmarshal --prefix scintilla_marshal scintilla-marshal.list --header > scintilla-marshal.h
	glib-genmarshal --prefix scintilla_marshal scintilla-marshal.list --body > scintilla-marshal.c 

EXTRA_DIST=lexers.make
