/* MouseDragHandler.cpp
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
#include "TLItem.h"
#include "TLView.h"
#include "TLSample.h"
#include "MouseDragHandler.h"


void SetRubberFramePen( wxDC& dc )
{
	dc.SetLogicalFunction( wxINVERT );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.SetPen( wxPen( *wxBLACK, 1, wxSHORT_DASH ) );
}


SampleDragHandler::SampleDragHandler( wxWindow* canvas, TLItem *item, TLView *view, int x, int y )
{
	m_canvas = canvas;
	m_item = item;
	m_view = view;
	m_view->ClearSelection();
	m_canvas->Refresh();
	m_canvas->Update();
	m_dragImagePos = m_view->GetItemBoundaries( item ); /* ?? */
	m_xOffset = x - m_dragImagePos.x;
	m_yOffset = y - m_dragImagePos.y;
	m_dragImage = GenerateDragImg( item, m_dragImagePos.width );
	m_dragImage->BeginDrag( wxPoint( m_xOffset, m_yOffset ), m_canvas );
	m_dragImage->Move( wxPoint( x, y ) );
	m_dragImage->Show();
	m_frameVisible = false;
}
SampleDragHandler::~SampleDragHandler()
{
	delete m_dragImage;
}
wxDragImage *SampleDragHandler::GenerateDragImg( TLItem *item, int width )
{
	wxBitmap bmp1( width, 25 );
	wxMemoryDC dc;
	dc.SelectObject( bmp1 );
	item->Draw( dc, m_view->GetRealZoom() );
	//item->GetSample()->Draw( dc, m_view->GetRealZoom() );
	return new wxDragImage( bmp1 );
}
void SampleDragHandler::ToggleFrame( wxDC& dc )
{
  int width, height;
  m_canvas->GetSize( &width, &height );
  wxRegion region( 0, 0, width, height );
  region.Subtract( m_dragImagePos );
  dc.SetClippingRegion( region );
  SetRubberFramePen( dc );
  dc.DrawRectangle( m_dragFramePos.x, m_dragFramePos.y,
      m_dragFramePos.width, m_dragFramePos.height );
  dc.DestroyClippingRegion();
}
void SampleDragHandler::ShowFrame( wxDC& dc )
{
  ToggleFrame( dc );
  m_frameVisible = true;
}
void SampleDragHandler::HideFrame( wxDC& dc )
{
  if ( m_frameVisible )
    ToggleFrame( dc );
  m_frameVisible = false;
}
void SampleDragHandler::OnDrag( int x, int y )
{
	wxClientDC dc( m_canvas );
	HideFrame( dc );
	m_dragImagePos.x = x - m_xOffset;
	m_dragImagePos.y = y - m_yOffset;
	m_dragImage->Move( wxPoint( x, y ) );
	int trackNr = m_view->GetTrackByY( y );
	if ( trackNr < 0 )
		return; //No Frame Visible
	int xNew = m_view->GetScreenSnapPosition( x - m_xOffset );
	int yNew = trackNr * 30 + TOP_OFFSET_TRACKS - m_view->GetYScrollPosition();
	m_dragFramePos = wxRect( xNew, yNew, m_dragImagePos.width, 25 );
	ShowFrame( dc );
}
void SampleDragHandler::OnDrop( int x, int y, bool copy )
{
  m_dragImage->Hide();
	m_dragImage->EndDrag();

// Brauch ich das überhaupt wenn ich eh refresh mache?
#if 0
  wxClientDC dc( m_canvas );
  SetRubberFramePen( dc );
  HideFrame( m_sampleFrame, &dc );//FIXME Siehe oben
#endif
  m_view->DoDrop( x - m_xOffset, y, m_item, m_xOffset, copy );
  //m_canvas->ResetScrollBar();// Could also be done in panel??
  //m_canvas->Refresh();
}


