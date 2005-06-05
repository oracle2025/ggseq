# Copyright 1999-2004 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /cvsroot/ggseq/ggseq/ggseq-0.3.1.ebuild,v 1.1 2005/06/05 20:36:09 oracle2025 Exp $

DESCRIPTION="Gungirl Sequencer is an easy to use Audiosequencer"
HOMEPAGE="http://ggseq.sourceforge.net"
SRC_URI="mirror://sourceforge/ggseq/${P}.tar.gz"

LICENSE="GPL"
SLOT="0"
KEYWORDS="~x86"

IUSE=""
DEPEND="media-libs/portaudio
		>=x11-libs/wxGTK-2.4.2
		>=media-libs/libsamplerate-0.1.0
		>=media-libs/portaudio-18.1
		>=media-libs/libsndfile-1.0.10
		media-libs/libsamplerate
		>=media-libs/libsoundtouch-1.2.1"

src_unpack() {
	unpack ${A} || die "Unpacking the source failed"
	cd ${S} || die "Could not change directory."
}

src_compile() {
	econf || die "Configure failed"
	emake || die "Make failed"
}

src_install() {
	einstall || die "Install failed"
}


