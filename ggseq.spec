Summary : Gun Girl Sequenser NOT a midi seqenser
Name: ggseq
Version: 0.1
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Packager: Georg E Schneider <georg@georgs.org>
Source: http:/prdownload.sf.net/%{name}/%{name}-%{version}.tar.bz2
URL: http://ggseq.sourceforge.net/
Vendor: Richard  Spindler <oracle2025@gmx.de>
BuildRoot: %{_tmppath}/%{name}-root
Provides: ggseq
Requires: portaudio libsndfile wxGTK>=2.4.0

%description
Gun Girl Sequenser is NOT a midi seqenser. 
But its helpfull to sample different
audio-files (at the moment just 44100 kHz wav -files)

%prep
%setup -q
sh ./autogen.sh
%install
%makeinstall
%clean
%files

%changelog
* Mon Oct 13 2003 Georg E Schneider <georg@georgs.org>
- initial build

