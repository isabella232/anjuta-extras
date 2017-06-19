## Process this file with automake to produce Makefile.in

SUBDIRS = include

noinst_LTLIBRARIES = libanjuta-scintilla.la

AM_CPPFLAGS = \
	-I$(srcdir)/include \
	-I$(srcdir)/src \
	-I$(srcdir)/lexlib \
	$(LIBANJUTA_CFLAGS) \
	$(GTHREAD_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-DGTK -DSCI_LEXER -DUSE_XIM

AM_CXXFLAGS = -DGTK -DSCI_LEXER -DUSE_XIM -std=c++11

include $(srcdir)/lexers.make

libanjuta_scintilla_la_SOURCES =\
	src/AutoComplete.cxx \
	src/AutoComplete.h \
	src/CallTip.cxx \
	src/CallTip.h \
	src/CaseConvert.cxx \
	src/CaseConvert.h \
	src/CaseFolder.cxx \
	src/CaseFolder.h \
	src/UnicodeFromUTF8.h \
	src/Catalogue.cxx \
	src/Catalogue.h \
	src/CellBuffer.cxx \
	src/CellBuffer.h \
	src/CharClassify.cxx \
	src/CharClassify.h \
	src/ContractionState.cxx \
	src/ContractionState.h \
	src/Decoration.cxx \
	src/Decoration.h \
	src/Document.cxx \
	src/Document.h \
	src/EditModel.cxx \
	src/EditModel.h \
	src/Editor.cxx \
	src/Editor.h \
	src/EditView.cxx \
	src/EditView.h \
	src/ExternalLexer.cxx \
	src/ExternalLexer.h \
	src/Indicator.cxx \
	src/Indicator.h \
	src/KeyMap.cxx \
	src/KeyMap.h \
	src/LineMarker.cxx \
	src/LineMarker.h \
	src/MarginView.cxx \
	src/MarginView.h \
	src/Partitioning.h \
	src/PerLine.h \
	src/PerLine.cxx \
	src/PositionCache.cxx \
	src/PositionCache.h \
	src/RESearch.cxx \
	src/RESearch.h \
	src/RunStyles.cxx \
	src/RunStyles.h \
	src/ScintillaBase.cxx \
	src/ScintillaBase.h \
	src/Selection.cxx \
	src/Selection.h \
	src/SplitVector.h \
	src/Style.cxx \
	src/Style.h \
	src/SVector.h \
	src/UniConversion.cxx \
	src/UniConversion.h \
	src/ViewStyle.cxx \
	src/ViewStyle.h\
	src/XPM.cxx\
	src/XPM.h\
	lexlib/Accessor.cxx \
	lexlib/Accessor.h \
	lexlib/CharacterSet.cxx \
	lexlib/CharacterSet.h \
	lexlib/CharacterCategory.cxx \
	lexlib/CharacterCategory.h \
	lexlib/LexAccessor.h \
	lexlib/LexerBase.cxx \
	lexlib/LexerBase.h \
	lexlib/LexerModule.cxx \
	lexlib/LexerModule.h \
	lexlib/LexerNoExceptions.cxx \
	lexlib/LexerNoExceptions.h \
	lexlib/LexerSimple.cxx \
	lexlib/LexerSimple.h \
	lexlib/OptionSet.h \
	lexlib/PropSetSimple.cxx \
	lexlib/PropSetSimple.h \
	lexlib/SparseState.h \
	lexlib/StyleContext.cxx\
	lexlib/StyleContext.h\
	lexlib/SubStyles.h \
	lexlib/WordList.cxx \
	lexlib/WordList.h \
	gtk/Converter.h \
	gtk/PlatGTK.cxx \
	gtk/ScintillaGTK.cxx \
	gtk/ScintillaGTKAccessible.cxx \
	$(LEXER_SRCS) \
	$(BUILT_SOURCES)

libanjuta_scintilla_la_LIBADD = \
	$(LIBANJUTA_LIBS) \
	$(GTHREAD_LIBS) \
	$(GMODULE_LIBS)

noinst_PROGRAMS = test-scintilla

test_scintilla_CXXFLAGS = -DGTK
test_scintilla_SOURCES = test-scintilla.cxx
test_scintilla_LDADD = libanjuta-scintilla.la

$(srcdir)/lexers.make:
	cd $(srcdir); \
	echo "## Lexers make file" > $(notdir $@); \
	echo -n 'LEXER_OBJS = ' >> $(notdir $@); \
	for lex in lexers/*.cxx; do \
		echo '\' >> $(notdir $@); \
		lex=`echo $$lex | sed -e "s/cxx$$/o/"`; \
		echo -n "	$$lex" >> $(notdir $@); \
	done; \
	echo "" >> $(notdir $@); \
	echo "" >> $(notdir $@); \
	echo -n 'LEXER_SRCS = ' >> $(notdir $@); \
	for lex in lexers/*.cxx; do \
		echo '\' >> $(notdir $@); \
		echo -n "	$$lex" >> $(notdir $@); \
	done; \
	echo "" >> $(notdir $@)

update-scintilla:
	cd $(srcdir); \
	cp scintilla/gtk/*.cxx gtk; \
	cp scintilla/gtk/*.h gtk; \
	cp scintilla/gtk/*.list gtk; \
	cp scintilla/lexers/*.cxx lexers; \
	cp scintilla/lexlib/*.cxx lexlib; \
	cp scintilla/lexlib/*.h lexlib; \
	cp scintilla/src/*.cxx src; \
	cp scintilla/src/*.h src; \
	cp scintilla/include/*.h include; \
	cp scintilla/include/*.py include; \
	cp scintilla/include/*.iface include; \
	echo "Patching files ..."
	cd $(srcdir); \
	for patchfile in patches/*.diff; do \
		patch -p0 < $$patchfile || true; \
	done
	echo "Updating properties file..."
	cd $(srcdir); \
	./update-properties.pl ../properties/styles.properties scite/src

BUILT_SOURCES=scintilla-marshal.c scintilla-marshal.h


scintilla-marshal.h: gtk/scintilla-marshal.list
	$(AM_V_GEN)@GLIB_GENMARSHAL@  --prefix scintilla_marshal $(srcdir)/gtk/scintilla-marshal.list --header > xgen-gmc \
        && cp xgen-gmc $(@F) \
        && rm -f xgen-gmc

scintilla-marshal.c: gtk/scintilla-marshal.list
	$(AM_V_GEN)echo "#include \"scintilla-marshal.h\"" > xgenc-gmc \
	    && @GLIB_GENMARSHAL@  --prefix scintilla_marshal $(srcdir)/gtk/scintilla-marshal.list --body >> xgenc-gmc \
        && cp xgenc-gmc $(@F) \
        && rm -f xgenc-gmc

EXTRA_DIST=lexers.make \
	gtk/scintilla-marshal.list
