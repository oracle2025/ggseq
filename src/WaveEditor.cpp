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

#include <wx/dcbuffer.h>

#include "WaveEditor.h"
#include "stuff.h"
#include "TLItem.h"
#include "TLSample.h"

// event table

BEGIN_EVENT_TABLE(WaveEditor, wxPanel)
	EVT_PAINT(WaveEditor::OnPaint)
	EVT_ERASE_BACKGROUND(WaveEditor::OnEraseBackground)
END_EVENT_TABLE()


WaveEditor::WaveEditor( wxWindow* parent, TLItem *item, wxWindowID id )
	:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
	m_item = item;
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
//		dc.DrawLine( i, height / 2, i, (height / 2) +
//				( (height / 2)*buffer[ int( (float(i) / float(width)) * float(len)) ] ) );
	}
	dc.DrawLine( 0, height / 2, width, height / 2 );
	/* Taken from MixMagic
	 	for (i = 0; i < width;i++) {
		value = index_get_average (block->sample->index, 
					   offset, block->start / INDEX_RATIO,
					   block->track->song->x_scale / INDEX_RATIO, 
					   block->sample->length / INDEX_RATIO / 2) / ((float)0x7f / height);
		gdk_draw_line (block->pixmap,
			       block->wav_gc,
			       i, height / 2 + value,
			       i, height / 2 - value);
		
		offset += block->track->song->x_scale / INDEX_RATIO;
	}

	 */
}
void WaveEditor::OnEraseBackground( wxEraseEvent& event )
{
	
}
