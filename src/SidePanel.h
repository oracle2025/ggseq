/* SidePanel.h
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

#ifndef _SIDE_PANEL_H_
#define _SIDE_PANEL_H_

class wxStaticLine;

class SidePanel : public wxPanel
{
	public:
		SidePanel(wxWindow* parent,wxPanel* panel/*, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("panel")*/);
		void OnButton(wxCommandEvent& event);
	private:
		void Hide(bool hide=true);
		wxPanel* m_contentPanel;
		wxStaticLine *m_hLine;
		wxStaticLine *m_vLine;
		wxBoxSizer *m_hLineSizer;
		bool m_hidden;
		DECLARE_EVENT_TABLE()
};

#endif /* _SIDE_PANEL_H_ */
