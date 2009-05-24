# Distribute but don't install glade2schema.pl
dist-hook:
	cp $(srcdir)/glade2schema.pl $(distdir)
	chmod +x $(distdir)/glade2schema.pl
	cp $(srcdir)/builder2schema.pl $(distdir)
	chmod +x $(distdir)/builder2schema.pl
-include $(top_srcdir)/git.mk
