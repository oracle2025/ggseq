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

#include "stuff.h"
#include "WaveEditor.h"
#include "TLItem.h"
#include "TLSample.h"
#include "SoundManager.h"
// event table

BEGIN_EVENT_TABLE(WaveEditor, wxPanel)
	EVT_PAINT(WaveEditor::OnPaint)
	EVT_ERASE_BACKGROUND(WaveEditor::OnEraseBackground)
	EVT_LEFT_DOWN(WaveEditor::OnMouseDown)
	EVT_MOTION(WaveEditor::OnMouseMotion)
	EVT_LEFT_UP(WaveEditor::OnMouseUp)
	EVT_SIZE(WaveEditor::OnSize)
END_EVENT_TABLE()


WaveEditor::WaveEditor( wxWindow* parent, TLItem *item, wxWindowID id )
	:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
	m_item = item;
	m_marker[0] = wxRect( -5, 0, 10, 5 );
	m_marker[1] = wxRect( GetClientSize().GetWidth() - 5, 0, 10, 5 );
	m_leftTrim = 0;
	m_rightTrim = m_item->GetLength() / 2;
	m_dragOffset = 0;
	m_dragMarker = 0;
	m_caretVisible = false;
	m_caretOldPosition = 0;
}
WaveEditor::~WaveEditor()
{
}
void WaveEditor::ShowCaret()
{
	if (m_caretVisible)
		return;
	wxClientDC dc(this);
	dc.SetLogicalFunction( wxINVERT );
	dc.SetPen( *wxTRANSPARENT_PEN );
	m_caretOldPosition = m_marker[0].x + 5;
	dc.DrawRectangle( m_caretOldPosition, 0, 1, GetSize().GetHeight() );
	m_caretVisible = true;
}
void WaveEditor::HideCaret()
{
	if (!m_caretVisible)
		return;
	wxClientDC dc(this);
	dc.SetLogicalFunction( wxINVERT );
	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle( m_caretOldPosition, 0, 1, GetSize().GetHeight() );
	m_caretVisible = false;
}
void WaveEditor::UpdateCaret()
{
	if (!m_caretVisible)
		return;
	wxClientDC dc(this);
	dc.SetLogicalFunction( wxINVERT );
	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle( m_caretOldPosition, 0, 1, GetSize().GetHeight() );
	m_caretOldPosition = m_marker[0].x + 5 + TrimToMark(g_ggseqProps.GetSoundManager()->GetPosition()/2);
	dc.DrawRectangle( m_caretOldPosition, 0, 1, GetSize().GetHeight() );
}
gg_tl_dat WaveEditor::MarkToTrim( int x )
{
	return ( gg_tl_dat(x+5) * (m_item->GetLength() / 2) ) / GetClientSize().GetWidth();
}
int WaveEditor::TrimToMark( gg_tl_dat x )
{
	return ( ( x * GetClientSize().GetWidth() ) / (m_item->GetLength() / 2) ) - 5; 
}
float get_average( float* buffer, unsigned int len )
{
	float sum = 0;
	for ( int i = 0; i < len; i+=2 ) {
		sum += fabsf(buffer[i]);
	}
	return sum / float(len / 2);
}
int resize( int oldX, int oldWidth, int newWidth )
{
	return ( ( (oldX + 5) * newWidth ) / oldWidth ) -5;
}
/*void WaveEditor::OnSize( wxSizeEvent& event )
{
	//m_marker[0].x = resize( m_marker[0].x, m_oldWidth, event.GetSize().GetWidth() );
	//m_marker[1].x = resize( m_marker[1].x, m_oldWidth, event.GetSize().GetWidth() );
	//m_oldWidth = event.GetSize().GetWidth();
}*/
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
	m_marker[0].x = TrimToMark( m_leftTrim );
	m_marker[1].x = TrimToMark( m_rightTrim );
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
	g_ggseqProps.GetSoundManager()->Stop();
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
	if ( m_dragMarker == &m_marker[0] ) {
		m_leftTrim = MarkToTrim( m_dragMarker->x );
	} else if ( m_dragMarker == &m_marker[1] ) {
		m_rightTrim = MarkToTrim( m_dragMarker->x );
	}
	if ( m_leftTrim > m_rightTrim ) {
		gg_tl_dat t = m_leftTrim;
		m_leftTrim = m_rightTrim;
		m_rightTrim = t;
	}
	Refresh();
}

void WaveEditor::OnMouseUp( wxMouseEvent& event )
{
	if ( m_dragMarker ) {
		ReleaseMouse();
	}
	m_dragMarker = 0;
}
void WaveEditor::GetTrims( gg_tl_dat &start, gg_tl_dat &end )
{
	
}
		

