/* SampleExtendingHandler.cpp
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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

#include <iostream>
#include <cmath>

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "stuff.h"
#include "TLItem.h"
#include "TLView.h"
#include "TLSample.h"
#include "SampleExtendingHandler.h"

SampleExtendingHandler::SampleExtendingHandler( wxWindow* canvas, TLItem *item, TLView *view, int x, int y )
	: m_view( view ), m_canvas( canvas ), m_x( x ), m_item( item )
{
	m_view->ClearSelection();
	m_canvas->Refresh();
	m_canvas->Update();
	wxClientDC dc( m_canvas );
	dc.SetLogicalFunction( wxINVERT );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.SetPen( wxPen( *wxBLACK, 1, wxSOLID ) );


	m_track_y = item->GetTrack() * 30 + TOP_OFFSET_TRACKS - m_view->GetYScrollPosition();
	m_item_len = (long)( item->GetLen() / m_view->GetRealZoom() );
	m_item_end = m_view->FromTLtoScreenX( item->GetEndPosition() );
	
	dc.DrawRectangle( m_x, m_track_y, 2, 25 );
	
}
SampleExtendingHandler::~SampleExtendingHandler()
{
}
void SampleExtendingHandler::OnDrag( int x, int y )
{
	wxClientDC dc( m_canvas );
	dc.SetLogicalFunction( wxINVERT );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.SetPen( wxPen( *wxBLACK, 1, wxSOLID ) );
	dc.DrawRectangle( m_x, m_track_y, 2, 25 );
	int times = (int)roundf( (float)( x - m_item_end ) / (float)m_item_len );
	x = m_item_end + ( m_item_len * times );
	m_x = x;
	dc.DrawRectangle( m_x, m_track_y, 2, 25 );
}
void SampleExtendingHandler::OnDrop( int x, int y, bool copy )
{
	wxClientDC dc( m_canvas );
	dc.SetLogicalFunction( wxINVERT );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	dc.SetPen( wxPen( *wxBLACK, 1, wxSOLID ) );
	dc.DrawRectangle( m_x, m_track_y, 2, 25 );
	int times = (int)roundf( (float)( x - m_item_end ) / (float)m_item_len );
	m_item->SetExtended( 1 + times );
}

