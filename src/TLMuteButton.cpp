/* TLMuteButton.cpp
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

#include "TLTrack.h"
#include "TLMuteButton.h"
#include "mute_off.xpm"
#include "mute_on.xpm"

BEGIN_EVENT_TABLE(TLMuteButton, wxBitmapButton)
	EVT_BUTTON(-1,TLMuteButton::OnClick)
END_EVENT_TABLE()


TLMuteButton::TLMuteButton(wxWindow* parent, wxWindowID id,TLTrack* track, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
	: wxBitmapButton(parent, id, bitmap, pos ,size , style , validator , name)
{
	m_track=track;
	m_mute=FALSE;
}


void TLMuteButton::OnClick(wxCommandEvent &event)
{
	m_mute=!m_mute;
	m_track->SetMute(m_mute);
	if(m_mute) {
		wxBitmap bmp(mute_on_xpm);
		SetBitmapLabel(bmp);
		SetBitmapFocus(bmp);
		SetBitmapSelected(bmp);
	} else {
		wxBitmap bmp(mute_off_xpm);
		SetBitmapLabel(bmp);
		SetBitmapFocus(bmp);
		SetBitmapSelected(bmp);
	}
}

void TLMuteButton::SetMute(bool mute)
{
	m_mute=mute;
	m_track->SetMute(m_mute);
	if(m_mute) {
		wxBitmap bmp(mute_on_xpm);
		SetBitmapLabel(bmp);
		SetBitmapFocus(bmp);
		SetBitmapSelected(bmp);
	} else {
		wxBitmap bmp(mute_off_xpm);
		SetBitmapLabel(bmp);
		SetBitmapFocus(bmp);
		SetBitmapSelected(bmp);
	}

}
