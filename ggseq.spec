Summary : Gungirl Sequencer
Name: ggseq
Version: 0.2.0
Release: cvs.20051902
Vendor: Richard  Spindler <oracle2025@gmx.de>
Packager: Richard Spindler <oracle2025@gmx.de>, Georg E Schneider <georg@georgs.org>
License: GPL
Group: Applications/Multimedia
Source: http:/dl.sf.net/%{name}/%{name}-%{version}.tar.bz2
URL: http://ggseq.sourceforge.net/
BuildRoot: %{_tmppath}/%{name}-root

BuildRequires: portaudio-devel libsndfile-devel wxGTK-devel >= 2.4.0 samplerate-devel >= 0.0.15 libxml2-devel

%description
Gungirl Sequencer is NOT a midi seqencer. 
But it is helpful to sample different
PCM audio files (at the moment just WAV files)

%description -l de
Gungirl Sequencer ist KEIN midi Sequencer.
Aber er ist sehr hilfreich um verschiedene
PCM Audio Dateien zu Sampeln (im Augenblick nur WAV Dateien)


%prep

%setup -q

%build 

%configure

make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT

%makeinstall
mkdir -p %{buildroot}%{_datadir}/icons/
mkdir -p %{buildroot}%{_datadir}/applications/
install -m 644 icons/ggseq_32.xpm %{buildroot}%{_datadir}/icons/
desktop-file-install --dir %{buildroot}%{_datadir}/applications/ --vendor="Richard Spindler <oracle2025@gmx.de>" -m 644 ggseq.desktop
cd %{buildroot}%{_datadir}/icons/;rm -f c* d* m* n* o* p* r* s* w*

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc README COPYING NEWS AUTHORS THANKS ChangeLog
%{_bindir}/ggseq
%{_datadir}/icons/ggseq_32.xpm
%{_datadir}/applications/*


%changelog
* Sat Feb 19 2005 Georg E Schneider <georg@georgs.org>
- build from cvs
- Added "include <math.h>" src/WaveEditor.cpp
- Added request for SoundTouch in configure.ac

* Wed Feb 16 2005 Georg E Schneider <georg@georgs.org>
- corrected horrible bugs in the spec-file

* Mon Feb 14 2005 Georg E Schneider <georg@georgs.org>
- rebuild for FC3
- Added "#include <math.h>" in src/TLView.cpp
- reonfigured *.desktop-file

* Mon Feb 16 2004 Richard Spindler <oracle2025@gmx.de>
- strip ggseq executable

* Sat Dec 27 2003 Georg E Schneider <georg@georgs.org> 
- removed not needed icons
- corrected some little mistakes

* Sun Dec 21 2003 Georg E Schneider <georg@georgs.org>
- initial build

