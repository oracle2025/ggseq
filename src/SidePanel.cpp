/* SidePanel.cpp
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
#include <wx/statline.h>
#include "SidePanel.h"

enum
{
    ID_Button = 1
};
BEGIN_EVENT_TABLE(SidePanel, wxPanel)
    EVT_BUTTON(ID_Button, SidePanel::OnButton)
END_EVENT_TABLE()

SidePanel::SidePanel(wxWindow* parent,wxPanel* panel, wxBoxSizer *sizer/*, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name*/)
//	: wxPanel(parent,id,pos,size,style|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN,name)
	: wxPanel(parent,-1,wxDefaultPosition,wxDefaultSize,wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN)
{
	m_sidebarSizer=sizer;
	m_hidden = true;
	m_contentPanel = panel;
	m_contentPanel->Reparent(this);
	m_contentPanel->Show(false);
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	m_hLineSizer = new wxBoxSizer(wxHORIZONTAL);

	m_hLine = new wxStaticLine( this, -1, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
	m_vLine = new wxStaticLine(this,-1,wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	m_hLine->Show(false);
	
	wxButton *button = new wxButton(this, ID_Button, wxT(""),wxDefaultPosition, wxSize(10,10), 0 );
	m_hLineSizer->Add(button, 0, wxALIGN_CENTRE, 5);
	
	panelSizer->Add(m_hLineSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	panelSizer->Add(m_vLine,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL,5);

	SetSizer(panelSizer);
	panelSizer->Fit(this);
}
void SidePanel::OnButton(wxCommandEvent& event)
{
	m_hidden=!m_hidden;
	Hide(m_hidden);
}
void SidePanel::Hide(bool hide)
{
	wxBoxSizer *sizer = m_sidebarSizer;
	if (hide) {
		m_vLine->Show(true);
		m_hLine->Show(false);
		m_contentPanel->Show(false);
		GetSizer()->Remove(m_contentPanel);
		m_hLineSizer->Remove(m_hLine);
		GetSizer()->Add(m_vLine,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL,5);
		GetSizer()->Fit(this);
		sizer->Remove(this);
		sizer->Add(this,0,wxEXPAND);
		sizer->Layout();
	} else {
		m_vLine->Show(false);
		m_hLine->Show(true);
		m_contentPanel->Show(true);
		GetSizer()->Remove(m_vLine);
		GetSizer()->Add(m_contentPanel, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
		m_hLineSizer->Prepend(m_hLine, 1, wxALIGN_CENTRE|wxRIGHT, 5);
		
		GetSizer()->Fit(this);
		sizer->Remove(this);
		sizer->Add(this,0,wxEXPAND);
		sizer->Layout();
	}
}
