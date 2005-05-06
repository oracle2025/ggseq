# Copyright (C) 2003 Richard Spindler <oracle2025@gmx.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = contents.html keyboard.html tutorial.html sound_formats.html ggseq.hhp ggseq.hhk ggseq.hhc ggseq.htb make_help_file
SUBDIRS = pics

# Use Variable instead of ggseq below

install-data-local:
	$(mkinstalldirs) $(prefix)/share/doc/ggseq/
	$(INSTALL_DATA) $(srcdir)/ggseq.htb $(prefix)/share/doc/ggseq/ggseq.htb

#$(sharedstatedir)
