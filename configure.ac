# Copyright (C) 2003 Richard Spindler <oracle2025@gmx.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([ggseq],
        [0.2.0pre1],
        [Richard Spindler oracle2025@gmx.de],
        [ggseq])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

dnl then we have to ask if any compatible compiler and installer is installed
dnl -------------------------------------------------------------------------

dnl Deine software laesst sich auch fein mit gcc compilieren
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

dnl next we have to ask for the standard libaries and headers
dnl ---------------------------------------------------------------------

AC_HEADER_STDC



dnl now extentions X soundcard etc..
dnl ---------------------------------------------------------------------

dnl Soundkarte sndfile und portaudio

 AC_CHECK_HEADER(sys/soundcard.h,,
  AC_MSG_ERROR([*** sys/soundcard.h not found - this program requires the Open Sound System (http://4front-tech.com) or compatible (http://www.alsa-project.com) ***]))

#AC_CHECK_HEADERS(sndfile.h,,
#  AC_MSG_ERROR([*** sndfile.h not found - this program requires libsndfile http://www.zip.com.au/~erikd/libsndfile/ ***]))

PKG_CHECK_MODULES(SNDFILE, sndfile > 1.0.0,,
  AC_MSG_ERROR([*** libsndfile not found - this program requires libsndfile http://www.zip.com.au/~erikd/libsndfile/ ***]))

PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.0.15,, AC_MSG_ERROR([*** libsamplerate not found - this program requires libsamplerate  http://www.mega-nerd.com/SRC/***]))

AC_CHECK_HEADERS(portaudio.h,,
  AC_MSG_ERROR([*** portaudio.h not found - this program requires portaudio http://www.portaudio.com/ ***]))
  
dnl and last check  your(our) own things like wxWindows ....
dnl ----------------------------------------------------------------------

dnl Check for XML support
dnl ---------------------

AC_PATH_PROG(XML_CONFIG,xml2-config,no)
if test "x$XML_CONFIG" == "xno";then
  AC_MSG_ERROR([*** xml2-config not found - but needed look at http://xmlsoft.org/***])
fi

dnl Check for wxWIN
dnl ---------------

AC_PATH_PROG(WXWIN_CONFIG,wx-config,no)
if test "x$WXWIN_CONFIG" == "xno";then
  AC_MSG_ERROR([*** wxWIN not installed but needed - http://www.wxwindows.org/***])
fi

dnl checking for optional featues (sould we compile for i386 or i686 etc..)
dnl ----------------------------------------------------------------------
AC_PATH_PROG(SOX,sox,no)
dnl Check for Sound Exchange
dnl Abfragen tue ichs jetzt schon sox ist auch OS-unabhaengig und wuerde
dnl die vorteile von Import und export in mehrere Soundformate bieten.
dnl die Kompressionsformate muessen unter linux zumindest gesondert
dnl unterstuetzt werden fuer mp3 kans lame bladeenc oder mad sein
dnl wobei mad wegen der direkten einbettung in sox vorzuziehen ist
dnl das ogg-vorbis Format wenns installiert ist soll auch eingebettet werden 
AC_PATH_PROG(BLADEE,bladeenc,no)
AC_PATH_PROG(MAD,madplay,no)
AC_PATH_PROG(LAME,lame,no)
AC_PATH_PROG(OGGENC,oggenc,no)
AC_PATH_PROG(OGGDEC,oggdec,no)
dnl now how to configure it all
dnl --------------------------------------------------------------------------

CXXFLAGS="$CXXFLAGS -Wall -O2 `wx-config --cxxflags`"
CPPFLAGS="$CPPFLAGS -Wall -O2 `wx-config --cxxflags`" 
CFLAGS="$CFLAGS -Wall -O2 `wx-config --cxxflags`"
#CLIBS="$CLIBS `pkg-config --libs sndfile`  -lportaudio `wx-config --libs`"

dnl at last now we can Build the Makefile
dnl --------------------------------------------------------------------------





AC_CONFIG_FILES([
   Makefile
   src/Makefile
   icons/Makefile
])

AC_OUTPUT

dnl weils nett aussieht meldungen zum schluss

echo "Seems to be OK ;-)"
echo "=================="
echo
echo -n "sox support ........... "
if test "x$SOX" == "xno";then
 echo "no"
else
 echo "yes"
fi
echo -n "mad support ........... "
if test "x$MAD" == "xno";then
 echo "no"
else
 echo "yes"
fi
echo -n "lame support .......... "
if test "x$LAME" == "xno";then
 echo "no"
else
 echo "yes"
fi
echo -n "bladeenc support ...... "
if test "x$BLADEE" == "xno";then
 echo "no"
else
 echo "yes"
fi
echo -n "oggenc support ........ "
if test "x$OGGENC" == "xno";then
 echo "no"
else
 echo "yes"
fi
echo -n "oggdec support ........ "
if test "x$OGGDEC" == "xno";then
 echo "no"
else
 echo "yes"
fi
echo ""
echo "at the moment it does not make a sense if anything of it is installed or not"
echo "at later versions it should support different Sound Formats just for"
echo "import and export"
echo ""

echo "now you can run make to build Gungirl Sequencer"
echo "and make intall to install it" 
