# Copyright (C) 2003 Richard Spindler <oracle2025@gmx.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = ggseq
ggseq_SOURCES = MiniFilerTest.cpp\
                tinyxmlparser.cpp\
                TLMiniFiler2.cpp\
                TLSelectionSet.cpp\
                SoundManager.cpp\
                TLColourManager.cpp\
                TLPanel.cpp\
                TLTrack.cpp\
                tinystr.cpp\
                TLData.cpp\
                TLSample.cpp\
                TLView.cpp\
                tinyxml.cpp\
                TLItem.cpp\
                TLSampleManager.cpp\
                TLXmlLoader2.cpp\
                tinyxmlerror.cpp\
                TLLoadSaveManager.cpp\
                TLSelectColourDialog.cpp\
		TLSetSnapDialog.cpp\
		TLMuteButton.cpp\
		dial.cpp\
		dial.h\
		TLTrackVolumeDial.h\
		TLTrackVolumeDial.cpp\
		MiniPlayer.h\
		MiniPlayer.cpp\
		stuff.h\
		Ruler.h\
		Ruler.cpp\
		UpdateListener.cpp\
		UpdateListener.h\
		StatusProgressBar.cpp\
		StatusProgressBar.h\
		DisableListener.h\
		DisableListener.cpp\
		SidePanel.cpp\
		SidePanel.h\
		FileInfoPanel.cpp\
		FileInfoPanel.h\
		ShortcutsDialog.cpp\
		ShortcutsDialog.h\
                SoundManager.h\
                tinystr.h\
                tinyxml.h\
                TLColourManager.h\
                TLData.h\
                TLItem.h\
                TLLoadSaveManager.h\
                TLMiniFiler2.h\
                TLPanel.h\
                TLSample.h\
                TLSampleManager.h\
                TLSelectColourDialog.h\
                TLSelectionSet.h\
                TLTrack.h\
                TLView.h\
                TLXmlLoader2.h\
		TLSetSnapDialog.h\
		TLMuteButton.h

ggseq_LDFLAGS = $(CLIBS) @SNDFILE_LIBS@ @SAMPLERATE_LIBS@ -lportaudio `wx-config --libs`
AM_CPPFLAGS = @CPPFLAGS@ $(CPPFLAGS) @SNDFILE_CFLAGS@ @SAMPLERATE_CFLAGS@ -DLOCALEDIR=\"$(NLSDIR)\"  -I$(top_srcdir)/icons

