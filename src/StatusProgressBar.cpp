/* StatusProgressBar.cpp
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
#include "UpdateListener.h"
#include "StatusProgressBar.h"
#include "DisableListener.h"

enum
{
	ID_CancelButton = 1,
};

BEGIN_EVENT_TABLE(StatusProgressBar, wxStatusBar)
	EVT_SIZE(StatusProgressBar::OnSize)
	EVT_BUTTON(ID_CancelButton, StatusProgressBar::OnCancelButton)
#ifdef __WXMSW__
	EVT_ERASE_BACKGROUND(StatusProgressBar::OnEraseBackground)
#endif
END_EVENT_TABLE()


StatusProgressBar::StatusProgressBar(wxWindow* parent, wxWindowID id, long style, const wxString& name ) : wxStatusBar(parent,id,style/*|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN*/,name)
{
	m_gauge = new wxGauge(this, -1, 100, wxPoint(85+200,5),wxSize(100,16));
	m_gauge->SetValue(30);
	m_cancelButton = new wxButton(this,ID_CancelButton,wxT("Cancel"),wxPoint(40+200,4),wxSize(40,18));
	m_gauge->Show(false);
	m_cancelButton->Show(false);
	m_continue=true;
	m_disableListener=NULL;
}
StatusProgressBar::~StatusProgressBar()
{
	if (m_disableListener)
		delete m_disableListener;
}
void StatusProgressBar::OnEraseBackground(wxEraseEvent &event)
{
	wxDC *dc=event.GetDC(); /*Evil Hack to remove flicker from wxStatusBar*/
	int width, height;
	GetClientSize(&width, &height);
	dc->SetBrush(wxBrush(GetBackgroundColour(),wxSOLID));
	dc->SetPen(*wxTRANSPARENT_PEN);
	dc->DrawRectangle(0,0,width,2);
//	event.Skip();
}
bool StatusProgressBar::Update(int status)
{
	m_gauge->SetValue(status);
	wxYield();
	return m_continue;
}
void StatusProgressBar::StartUpdateProcess()
{
	if (m_disableListener)
		m_disableListener->DisableAll();
	m_gauge->Show(true);
	m_cancelButton->Show(true);
	m_continue=true;
}
void StatusProgressBar::EndUpdateProcess()
{
	if (m_disableListener)
		m_disableListener->EnableAll();
	m_gauge->Show(false);
	m_cancelButton->Show(false);
}
void StatusProgressBar::OnSize(wxSizeEvent &event)
{
	wxRect rect;
	GetFieldRect(0,rect);
	m_gauge->SetSize(wxSize(rect.GetWidth()-110-200,16));
}
void StatusProgressBar::OnCancelButton(wxCommandEvent &event)
{
	m_continue=false;
}

void StatusProgressBar::SetDisableListener(DisableListener *disableListener)
{
	m_disableListener=disableListener;
}
