/* Ruler.cpp
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
#include <wx/dcbuffer.h>

#include "stuff.h"
#include "Ruler.h"

#define LEFT_OFFSET 55

BEGIN_EVENT_TABLE(Ruler, wxPanel)
	EVT_PAINT(Ruler::OnPaint)
#ifdef __WXMSW__
	EVT_ERASE_BACKGROUND(Ruler::OnEraseBackground)
#endif
	EVT_LEFT_DOWN(Ruler::OnLeftDown)
	EVT_LEFT_UP(Ruler::OnLeftUp)
	EVT_MOTION(Ruler::OnMouseMotion)
END_EVENT_TABLE()

Ruler::Ruler(wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxString& name)
	:wxPanel(parent, id, pos, size, style/*|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE*/ , name)
{
	m_snap=1;
	m_position=1;
	m_pos1=0;
	m_pos2=0;
	m_listener=0;
	m_zoom=( 117600.0 / 31.0 );
}
void Ruler::OnPaint(wxPaintEvent& event)
{
	int width, height;
	GetClientSize(&width, &height);
#ifdef __WXMSW__
	wxBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(GetBackgroundColour(),wxSOLID));
	dc.SetPen(*wxTRANSPARENT_PEN);
	int width, height;
	GetClientSize(&width, &height);
	dc.DrawRectangle(0,0,width,height);
#else
	wxPaintDC dc(this);
#endif
	if(m_pos1<m_pos2) {
		dc.SetBrush(*wxBLUE_BRUSH);
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawRectangle(m_pos1-m_position, 0, m_pos2-m_pos1, height);
	}
	long index= m_position/m_snap;
	long offs=m_position%m_snap;
	wxString index_str;
	dc.SetFont(*wxNORMAL_FONT);
	for (int i=0;m_snap*i-offs+LEFT_OFFSET<GetSize().GetWidth();i++) {
		index_str << index;
		dc.DrawText(index_str,m_snap*i-offs-3+LEFT_OFFSET,-2);
		index_str = wxT("");
		index++;
	}
}
void Ruler::OnLeftDown(wxMouseEvent& event)
{
	m_pos1=event.GetX()+m_position;
	CaptureMouse();
}
void Ruler::GetLoop(gg_tl_dat* pos1, gg_tl_dat* pos2)
{
	*pos1=m_pos1;
	*pos2=m_pos2;
}
void Ruler::OnLeftUp(wxMouseEvent& event)
{
	ReleaseMouse();
	m_pos2=event.GetX()+m_position;
	if (m_pos1<LEFT_OFFSET)
		m_pos1=LEFT_OFFSET;
	if (m_pos2<LEFT_OFFSET)
		m_pos2=LEFT_OFFSET;
	if (m_listener)
		m_listener->SetLoopSnaps((long long)((m_pos1-LEFT_OFFSET)* m_zoom), (long long)((m_pos2-LEFT_OFFSET)* m_zoom)  );
	Refresh();
}
void Ruler::OnMouseMotion(wxMouseEvent& event)
{
	if (!event.LeftIsDown())
		return;
	m_pos2=event.GetX()+m_position;
	Refresh();
	
}
void Ruler::SetSnap(long snap)
{
	m_snap=snap;
}
void Ruler::SetPosition(gg_tl_dat position)
{
	m_position=position;
	Refresh();
}
void Ruler::OnEraseBackground(wxEraseEvent& event)
{
//	event.Skip();
}
void Ruler::SetListener(LoopSetupListener *listener)
{
	m_listener=listener;
}


