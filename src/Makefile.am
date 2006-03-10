# Copyright (C) 2003 Richard Spindler <oracle2025@gmx.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = portaudio SoundTouch

bin_PROGRAMS = ggseq
ggseq_SOURCES = PreferencesDialog.h\
                PreferencesDialog.cpp\
		SampleEdit.cpp\
		SampleExtendingHandler.cpp\
		SampleExtendingHandler.h\
		Listeners.h\
		SampleEdit.h\
		PlayerInterface.h\
		PlayerInterface.cpp\
		ggseq_ui.cpp\
		MouseDragHandler.cpp\
		MouseDragHandler.h\
		SelectionDragHandler.cpp\
		SelectionDragHandler.h\
		RubberDragHandler.h\
		RubberDragHandler.cpp\
                ggseq_ui.h\
		EnvelopeDragHandler.h\
		EnvelopeDragHandler.cpp\
		TrimmerDialog.h\
		TrimmerDialog.cpp\
		WaveEditor.h\
		WaveEditor.cpp\
                ggseq_ui_wdr.cpp\
		colour_helper.h\
		colour_helper.cpp\
                ggseq_ui_wdr.h\
                ui_helper.cpp\
                ui_helper.h\
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
                GgseqGlobalProps.cpp\
                GgseqGlobalProps.h\
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
		ggEvtHandler.h\
		ggEvtHandler.cpp\
		BigScrollBar.cpp\
		BigScrollBar.h\
		GgseqDocManager.cpp\
		GgseqDocManager.h\
		ImportPackDialog.cpp\
		ImportPackDialog.h\
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
                TLSelectionSet.h\
                TLTrack.h\
                TLView.h\
                TLXmlLoader2.h\
		TLMuteButton.h

ggseq_LDADD = portaudio/libportaudio.a SoundTouch/libSoundTouch.a

AM_CPPFLAGS = @CPPFLAGS@ -DLOCALEDIR=\"$(NLSDIR)\"  -I$(top_srcdir)/icons -DINSTALL_PREFIX="\"$(prefix)\""
