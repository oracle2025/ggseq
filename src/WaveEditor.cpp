/* WaveEditor.cpp
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

#ifdef __GNUG__
    #pragma implementation "WaveEditor.cpp"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <math.h>

#include <wx/dcbuffer.h>
#include <math.h>

#include "WaveEditor.h"
#include "stuff.h"
#include "TLItem.h"
#include "TLSample.h"

// event table

BEGIN_EVENT_TABLE(WaveEditor, wxPanel)
	EVT_PAINT(WaveEditor::OnPaint)
	EVT_ERASE_BACKGROUND(WaveEditor::OnEraseBackground)
	EVT_LEFT_DOWN(WaveEditor::OnMouseDown)
	EVT_MOTION(WaveEditor::OnMouseMotion)
	EVT_LEFT_UP(WaveEditor::OnMouseUp)
END_EVENT_TABLE()


WaveEditor::WaveEditor( wxWindow* parent, TLItem *item, wxWindowID id )
	:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
	m_item = item;
	m_marker[0] = wxRect( 0, 0, 10, 5 );
	m_marker[1] = wxRect( 20, 0, 10, 5 );
	m_dragOffset = 0;
	m_dragMarker = 0;
}
WaveEditor::~WaveEditor()
{
}
float get_average( float* buffer, unsigned int len )
{
	float sum = 0;
	for ( int i = 0; i < len; i+=2 ) {
		sum += fabsf(buffer[i]);
	}
	return sum / float(len / 2);
}
void WaveEditor::OnPaint( wxPaintEvent& event )
{
	int width, height;
	float *buffer;
	gg_tl_dat len;
	wxPoint marker[3];
	marker[0] = wxPoint( 0, 0 );
	marker[1] = wxPoint( 10, 0 );
	marker[2] = wxPoint( 5, 5 );
	// Paint a WaveForm here
	wxBufferedPaintDC dc( this );
	GetClientSize( &width, &height );
	buffer = m_item->GetSample()->GetBuffer();
	len = m_item->GetLength();
	
	// DrawBackground
	dc.SetBrush( wxBrush( GetBackgroundColour(), wxSOLID ) );
	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle( 0, 0, width, height );
	dc.SetPen( *wxBLACK_PEN );
	int inc = len / ( width + 4 );
	for ( int i=0; i < width; i ++ ) {
		int h = ( get_average( &buffer[i * inc], inc * 4 ) * float( height  ) * 2 );
		dc.DrawLine( i, ( height / 2 ) - h, i, ( height / 2 ) + h );
	}
	dc.DrawLine( 0, height / 2, width, height / 2 );
	dc.SetPen( *wxRED_PEN );
	dc.DrawLine( m_marker[0].x + 5, 0, m_marker[0].x + 5, height );
	dc.DrawLine( m_marker[1].x + 5, 0, m_marker[1].x + 5, height );
	dc.SetBrush( *wxBLACK_BRUSH );
	dc.SetPen( *wxBLACK_PEN );
	dc.DrawPolygon( 3, marker, m_marker[0].x, m_marker[0].y );
	dc.DrawPolygon( 3, marker, m_marker[1].x, m_marker[1].y );
}
void WaveEditor::OnEraseBackground( wxEraseEvent& event )
{
	
}

void WaveEditor::OnMouseDown( wxMouseEvent& event )
{
	for ( int i = 0; i < 2; i++ ) {
		if ( m_marker[i].Inside( event.GetPosition() ) ) {
			m_dragMarker = &m_marker[i];
			m_dragOffset = event.m_x - m_marker[i].x;
			CaptureMouse();
			break;
		}
	}
}

void fit_between( int *value, int min, int max )
{
	if ( *value < min ) {
		*value = min;
	} else if ( *value > max ) {
		*value = max;
	}
}

void WaveEditor::OnMouseMotion( wxMouseEvent& event )
{
	if ( !m_dragMarker )
		return;
	m_dragMarker->x = event.m_x - m_dragOffset;
	fit_between( &m_dragMarker->x, - 5, GetClientSize().GetWidth() - 5 );
	Refresh();
}

void WaveEditor::OnMouseUp( wxMouseEvent& event )
{
	if ( m_dragMarker ) {
		ReleaseMouse();
	}
	m_dragMarker = 0;
}
		

