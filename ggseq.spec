Summary : Gungirl Sequencer NOT a midi seqenser
Name: ggseq
Version: 0.2pre1
Release: 1.1
Copyright: GPL
Group: Applications/Multimedia
Packager: Georg E Schneider <georg@georgs.org>
Source: http:/prdownload.sf.net/%{name}/%{name}-%{version}.tar.bz2
URL: http://ggseq.sourceforge.net/
Vendor: Richard  Spindler <oracle2025@gmx.de>
BuildRoot: %{_tmppath}/%{name}-root
Provides: ggseq
Requires: portaudio libsndfile wxGTK >= 2.4.0  libsamplerate >= 0.0.15
BuildRequires: portaudio-devel libsndfile-devel wxGTK-devel >= 2.4.0 libsamplerate-devel >= 0.0.15

%description
Gungirl Sequencer is NOT a midi seqenser. 
But it is helpfull to sample different
PCM-audio-files (at the moment just WAV - files)

%description -l de
Gungirl Sequencer ist KEIN midi Sequenser.
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
install -m 644 icons/ggseq_32.xpm %{buildroot}/usr/share/icons/
install -m 644 ggseq.desktop %{buildroot}/usr/share/applications/
cd %{buildroot}/usr/share/icons/;rm -f c* d* m* n* o* p* r* s* w*

%clean

%files
/usr/bin/ggseq
/usr/share/icons/ggseq_32.xpm
/usr/share/applications/*

%changelog

* Sat Dec 27 2003 Georg E Schneider <georg@georgs.org> 
- removed not needed icons
- corrected some little mistakes

* Sun Dec 21 2003 Georg E Schneider <georg@georgs.org>
- initial build

