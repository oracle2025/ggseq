# Copyright (C) 2003 Richard Spindler <oracle2025@gmx.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = get_soundtouch_1.3.0 soundtouch_1.3.0.patch
noinst_LIBRARIES = libSoundTouch.a

libSoundTouch_a_SOURCES = AAFilter.h  cpu_detect.h  FIRFilter.h \
         RateTransposer.h  TDStretch.h \
				 AAFilter.cpp FIRFilter.cpp FIFOSampleBuffer.cpp \
				 mmx_gcc.cpp RateTransposer.cpp SoundTouch.cpp \
				 TDStretch.cpp cpu_detect_x86_gcc.cpp\
				 BPMDetect.h  FIFOSampleBuffer.h  FIFOSamplePipe.h \
				 SoundTouch.h  STTypes.h
