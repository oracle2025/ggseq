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
        [0.2.0cvs],
        [Richard Spindler oracle2025@gmx.de],
        [ggseq])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl then we have to ask if any compatible compiler and installer is installed
dnl -------------------------------------------------------------------------

dnl Deine software laesst sich auch fein mit gcc compilieren
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

dnl next we have to ask for the standard libaries and headers
dnl ---------------------------------------------------------------------

AC_HEADER_STDC

AC_LANG(C++)
dnl needed for portaudio
AC_CHECK_LIB(pthread, pthread_create)


dnl now extentions X soundcard etc..
dnl ---------------------------------------------------------------------

dnl Soundkarte sndfile und portaudio

 AC_CHECK_HEADER(sys/soundcard.h,,
  AC_MSG_ERROR([*** sys/soundcard.h not found - this program requires the Open Sound System (http://4front-tech.com) or compatible (http://www.alsa-project.com) ***]))

#AC_CHECK_HEADERS(sndfile.h,,
#  AC_MSG_ERROR([*** sndfile.h not found - this program requires libsndfile http://www.zip.com.au/~erikd/libsndfile/ ***]))

PKG_CHECK_MODULES(SNDFILE, sndfile > 1.0.0,,
  AC_MSG_ERROR([*** libsndfile not found - this program requires libsndfile http://www.mega-nerd.com/libsndfile/ ***]))

CPPFLAGS="$CPPFLAGS $SNDFILE_CFLAGS"
LDFLAGS="$LDFLAGS $SNDFILE_LIBS"

PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.0.15,, AC_MSG_ERROR([*** libsamplerate not found - this program requires libsamplerate  http://www.mega-nerd.com/SRC/***]))

CPPFLAGS="$CPPFLAGS $SAMPLERATE_CFLAGS"
LDFLAGS="$LDFLAGS $SAMPLERATE_LIBS"

AC_CHECK_HEADERS(portaudio.h,,
  AC_MSG_ERROR([*** portaudio.h not found - this program requires portaudio http://www.portaudio.com/ ***]))

AC_CHECK_LIB([portaudio],[Pa_StreamActive],[],
  AC_MSG_ERROR([*** libportaudio not found or wrong version this program requires portaudio v18 http://www.portaudio.com/ ***]))
  
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

#AC_PATH_PROG(WXWIN_CONFIG,wx-config,no)
#if test "x$WXWIN_CONFIG" == "xno";then
#  AC_MSG_ERROR([*** wxWidgets not installed but needed - http://www.wxwidgets.org/***])
#fi

AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(2.4.2, wxWin=1)
if test "$wxWin" != 1; then
AC_MSG_ERROR([
	wxWindows must be installed on your system
	but wx-config script couldn't be found.

	Please check that wx-config is in path, the directory
	where wxWindows libraries are installed (returned by
	'wx-config --libs' command) is in LD_LIBRARY_PATH or
	equivalent variable and wxWindows version is 2.3.4 or above.
])
fi
CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LDFLAGS="$LDFLAGS $WX_LIBS"


dnl checking for needed features

 AC_CHECK_HEADER(soundtouch/SoundTouch.h,,
  AC_MSG_ERROR([*** soundtouch/SoundTouch.h not found - this program requires the SoundTouch Audio Processing Library http://www.iki.fi/oparviai/soundtouch ***]))

AC_CHECK_LIB([SoundTouch],[soundtouch_ac_test],[],[])
AC_CHECK_LIB([soundtouch],[soundtouch_ac_test],[],[])

if test "x$HAVE_LIBSoundTouch" == "xno";then
  if test "x$HAVE_LIBsoundtouch" == "xno"; then
    AC_MSG_ERROR([*** libsoundtouch not found - this program requieres the SoundTouch Audio Processing Library http://www.iki.fi/oparviai/soundtouch ***])
  fi
fi


# AC_CHECK_HEADER(soundtouch/SoundTouch.h,,
#  AC_MSG_ERROR([*** soundtouch/SoundTouch.h is needed - //sky.prohosting.com/oparviai/soundtouch/***]))

#AM_PATH_SOUNDTOUCH(,, AC_MSG_ERROR([*** SoundTouch not found - this program requires the SoundTouch SoundTouch Audio Processing Library http://www.iki.fi/oparviai/soundtouch ***]))

CPPFLAGS="$CPPFLAGS $SOUNDTOUCH_CXXFLAGS"

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


#CXXFLAGS="$CXXFLAGS -ggdb -Wall -O2 `wx-config --cxxflags`"
#CPPFLAGS="$CPPFLAGS -ggdb -Wall -O2 `wx-config --cxxflags`" 
#CFLAGS="$CFLAGS -ggdb -Wall -O2 `wx-config --cxxflags`"
#CLIBS="$CLIBS `pkg-config --libs sndfile`  -lportaudio `wx-config --libs`"

dnl at last now we can Build the Makefile
dnl --------------------------------------------------------------------------





AC_CONFIG_FILES([
   Makefile
   src/Makefile
   config/Makefile
   icons/Makefile
   doc/Makefile
   doc/pics/Makefile
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


#echo -n "SoundTouch cxxflags ........... "
#echo "$SOUNDTOUCH_CXXFLAGS"
#echo -n "SoundTouch libs     ........... "
#echo "$SOUNDTOUCH_LIBS"

echo ""
echo "now you can run make to build Gungirl Sequencer"
echo "and make intall to install it" 
