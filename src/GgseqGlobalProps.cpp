/* GgseqGlobalProps.cpp
 *
 *  Copyright (C) 2003 Richard Spindler <oracle2025@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "stuff.h"
#include "GgseqGlobalProps.h"
class wxDC;
class wxString;
class wxCoord;
class wxRect;
class wxColour;
class wxIcon;
#include "TLView.h"
 
GgseqGlobalProps g_ggseqProps;

GgseqGlobalProps::GgseqGlobalProps()
{
	m_soundManager = 0;
	m_miniPlayer = 0;
	m_fileInfoListener = 0;
}
float GgseqGlobalProps::GetZoom()
{
	return m_view->GetRealZoom();
}
SoundManager *GgseqGlobalProps::GetSoundManager()
{
	return m_soundManager;
}
void GgseqGlobalProps::SetSoundManager( SoundManager *soundManager )
{
	m_soundManager = soundManager;
}
MiniPlayerInterface *GgseqGlobalProps::GetMiniPlayer()
{
	return m_miniPlayer;
}
void GgseqGlobalProps::SetMiniPlayer( MiniPlayerInterface *miniPlayer )
{
	m_miniPlayer = miniPlayer;
}
void GgseqGlobalProps::SetFileInfoListener( FileInfoListener *fileInfoListener )
{
	m_fileInfoListener = fileInfoListener;
}
FileInfoListener *GgseqGlobalProps::GetFileInfoListener()
{
	return m_fileInfoListener;
}
void GgseqGlobalProps::SetDocManager( GgseqDocManager *docManager )
{
	m_docManager = docManager;
}
GgseqDocManager *GgseqGlobalProps::GetDocManager()
{
	return m_docManager;
}
void GgseqGlobalProps::SetView( TLView *view )
{
	m_view = view;
}
gg_tl_dat GgseqGlobalProps::GetSnap( gg_tl_dat x )
{
	return m_view->GetSnap( x );
}
gg_tl_dat GgseqGlobalProps::FromScreenXtoTL(long x)
{
	return m_view->FromScreenXtoTL( x );
}
long GgseqGlobalProps::FromTLtoScreenX(gg_tl_dat x)
{
	return m_view->FromTLtoScreenX( x );
}
