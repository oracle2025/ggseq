/* TLMuteButton.h
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
 
#ifndef _TL_MUTE_BUTTON_H_
#define _TL_MUTE_BUTTON_H_

#ifdef __WXMSW__
class TLMuteButton : public wxToggleButton
#else
class TLMuteButton : public wxBitmapButton
#endif
{
	public:
#ifdef __WXMSW__
		TLMuteButton(wxWindow* parent, wxWindowID id,TLTrack* track, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("button"));
		void OnToggle(wxCommandEvent &event);
#else
		TLMuteButton(wxWindow* parent, wxWindowID id,TLTrack* track, const wxBitmap& bitmap, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("button"));
		void OnClick(wxCommandEvent &event);
#endif
		void SetMute(bool mute);
		void OnEraseBackground(wxEraseEvent &event) {}
	private:
		TLTrack *m_track;
		bool m_mute;
		DECLARE_EVENT_TABLE()
};


#endif /*_TL_MUTE_BUTTON_H_*/
