/* TLTrackVolumeDial.cpp
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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "dial.h"
#include "TLTrack.h"
#include "TLTrackVolumeDial.h"

BEGIN_EVENT_TABLE(TLTrackVolumeDial, wxDial)
	EVT_SCROLL(TLTrackVolumeDial::OnScroll)
END_EVENT_TABLE()

TLTrackVolumeDial::TLTrackVolumeDial(wxWindow* parent, wxWindowID id, TLTrack* track, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: wxDial(parent, id, 100,0,100,pos,size,style,name)
{
	m_track=track;
}

void TLTrackVolumeDial::OnScroll(wxScrollEvent &event)
{
	m_track->SetVolume(((double)event.GetPosition())/100.);
}
