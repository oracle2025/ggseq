Summary : Gungirl Sequencer NOT a midi seqenser
Name: ggseq
Version: 0.2.0
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Packager: Richard Spindler <oracle2025@gmx.de>, Georg E Schneider <georg@georgs.org>
Source: http:/prdownload.sf.net/%{name}/%{name}-%{version}.tar.bz2
URL: http://ggseq.sourceforge.net/
Vendor: Richard  Spindler <oracle2025@gmx.de>
BuildRoot: %{_tmppath}/%{name}-root
Provides: ggseq
Requires: portaudio libsndfile wxGTK >= 2.4.0  libsamplerate >= 0.0.15
#Requires: libgtk-1.2.so.0 libgdk-1.2.so.0 libgmodule-1.2.so.0 libgthread-1.2.so.0 libglib-1.2.so.0 libpthread.so.0 libdl.so.2 libXext.so.6 libX11.so.6 libz.so.1 libm.so.6 libstdc++.so.5 libgcc_s.so.1 libc.so.6 ld-linux.so.2
BuildRequires: portaudio-devel libsndfile-devel wxGTK-devel >= 2.4.0 libsamplerate-devel >= 0.0.15

%description
Gungirl Sequencer is NOT a midi seqencer. 
But it is helpfull to sample different
PCM-audio-files (at the moment just WAV - files)

%description -l de
Gungirl Sequencer ist KEIN midi Sequencer.
Aber er ist sehr hilfreich um verschiedene
PCM-Audio-Dateien zu Sampeln (im Augenblick nur WAV - Dateien)

%prep

%setup -q
sh ./reconf

%configure --prefix=/usr

%install

%makeinstall
mkdir -p %{buildroot}/usr/share/icons/
mkdir -p %{buildroot}/usr/share/applications/
#mkdir -p %{buildroot}/usr/share/doc/
install -m 644 icons/ggseq_32.xpm %{buildroot}/usr/share/icons/
install -m 644 ggseq.desktop %{buildroot}/usr/share/applications/
#install -m 644 doc/ggseq.htb %{buildroot}/usr/doc/$NAME-$VERSION-$RELEASE
strip %{buildroot}/usr/bin/ggseq
cd %{buildroot}/usr/share/icons/;rm -f c* d* m* n* o* p* r* s* w*

%clean

%files
%doc README COPYING NEWS AUTHORS THANKS ChangeLog
/usr/bin/ggseq
/usr/share/icons/ggseq_32.xpm
/usr/share/applications/*
#/usr/share/doc/ggseq.htb

%changelog
* Mon Feb 16 2004 Richard Spindler <oracle2025@gmx.de>
- strip ggseq executable

* Sat Dec 27 2003 Georg E Schneider <georg@georgs.org> 
- removed not needed icons
- corrected some little mistakes

* Sun Dec 21 2003 Georg E Schneider <georg@georgs.org>
- initial build

