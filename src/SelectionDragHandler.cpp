/* SelectionDragHandler.cpp
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

#include <wx/dragimag.h>

#include "stuff.h"
#include "TLView.h"
#include "MouseDragHandler.h"
#include "SelectionDragHandler.h"

SelectionDragHandler::SelectionDragHandler( wxWindow* canvas, TLView *view,
                          int width, int height, int x, int y,
			  int xOffset, int yOffset
                        )
{
	m_canvas  = canvas;
	m_view    = view;
	m_xOffset = xOffset;
	m_yOffset = yOffset;
	int x_ = x - m_xOffset;
	int y_ = y;
	m_dragImage = GetDragImage( width, height );
	m_dragImage->BeginDrag( wxPoint( 0, 0 ), m_canvas );
	m_dragFramePos = wxRect( x_, y_, width, height );
	int trackNr = m_view->GetTrackByY( y_ );
	if ( trackNr >= 0 ) {
		m_dragFramePos.y = trackNr * 30 + TOP_OFFSET_TRACKS - m_view->GetYScrollPosition();
	}
	m_dragFramePos.x = m_view->GetScreenSnapPosition( x_ );
	m_dragImage->Move( wxPoint( m_dragFramePos.x, m_dragFramePos.y ) );
	m_dragImage->Show();
}
wxDragImage *SelectionDragHandler::GetDragImage( int width, int height )
{
	wxBitmap bitmap( width, height );
	wxMemoryDC *dc = new wxMemoryDC();
	dc->SelectObject( bitmap );
	wxPen pen( *wxBLACK, 1, wxLONG_DASH );
	dc->SetPen( pen );
	dc->SetBrush( *wxGREEN_BRUSH );
	dc->DrawRectangle( 0, 0, width, height );
	wxBrush ab;
	ab.SetStyle( wxTRANSPARENT );
	dc->SetBrush( ab );
	pen.SetColour( *wxWHITE );
	pen.SetWidth( 3 );
	pen.SetStyle( wxSOLID );
	dc->SetPen( pen );
	m_view->DrawSelection( dc );
	pen.SetWidth( 1 );
	pen.SetColour( *wxBLACK );
	pen.SetStyle( wxLONG_DASH );
	dc->SetPen( pen );
	m_view->DrawSelection( dc );
	delete dc;
	wxMask *mask = new wxMask( bitmap, *wxGREEN );
	bitmap.SetMask( mask );
	return new wxDragImage( bitmap );
}

  
void SelectionDragHandler::OnDrag( int x, int y )
{
  	int x_ = x - m_xOffset;
	int y_ = y;
	int trackNr = m_view->GetTrackByY( y_ );
	if ( trackNr < 0 ) {
		m_dragImage->Hide();
		return;
	}
	m_dragImage->Show();
	m_dragFramePos.y = trackNr * 30 + TOP_OFFSET_TRACKS - m_view->GetYScrollPosition(); //TODO
	m_dragFramePos.x = m_view->GetScreenSnapPosition( x_ );
	m_dragImage->Move( wxPoint( m_dragFramePos.x, m_dragFramePos.y ) );

}
void SelectionDragHandler::OnDrop( int x, int y, bool copy )
{
  	int x_ = x - m_xOffset;
	int y_ = y;
	m_dragImage->Hide();
	m_dragImage->EndDrag();
	delete m_dragImage;
	m_view->EndSelectionDrag( x_, y_, copy, m_xOffset );
	//ResetScrollBar();
	//Refresh();

}
