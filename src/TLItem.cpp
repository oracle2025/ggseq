/* TLItem.cpp
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
#include <sndfile.h>

#include "stuff.h"
#include "TLItem.h"
#include "TLTrack.h"
#include "TLSample.h"
#include <iostream>
//TODO: trackNr überall eliminieren
TLItem::TLItem(TLSample *sample/*, int trackNr*/ , gg_tl_dat position, long reference, GetItemTrackListener* trackListener )
{
	m_sample=sample;
	m_position=position;
	m_sample->Ref();
	m_selected=false;
	//m_trackNr = trackNr;
	m_referenceId = reference;
	m_trackListener = trackListener;
	m_x_test = 10;
	m_y_test = 10;
	m_leftFadeIn   = wxRect( 0, 0, 7, 7 );
	m_rightFadeIn  = wxRect( 10, 0, 7, 7 );
	m_leftFadeOut  = wxRect( 20, 0, 7, 7 );
	m_rightFadeOut = wxRect( (int)(m_sample->GetLength() / ( 117600.0 / 31.0 )-7), 0, 7, 7 );

}
void DrawWxRect( wxDC &dc, const wxRect &rect ) // TODO make Helper Functions File
{
	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}
void TLItem::DrawEnvelope( wxDC &dc, int xOffset, int yOffset )
{
	dc.SetDeviceOrigin( xOffset, yOffset );
	wxPoint lines[4] = {
		wxPoint( m_leftFadeIn.x, m_leftFadeIn.y ),
		wxPoint( m_rightFadeIn.x, m_rightFadeIn.y ),
		wxPoint( m_leftFadeOut.x, m_leftFadeOut.y ),
		wxPoint( m_rightFadeOut.x, m_rightFadeOut.y ) };
	dc.DrawLines( 4, lines, 3, 3 );
	DrawWxRect( dc, m_leftFadeIn );
	DrawWxRect( dc, m_rightFadeIn );
	DrawWxRect( dc, m_leftFadeOut );
	DrawWxRect( dc, m_rightFadeOut );
	dc.SetDeviceOrigin( 0, 0 );
}
wxRect *TLItem::TouchingEnvelopeCtrl( int x, int y )
{
	if ( m_leftFadeIn.Inside( x, y ) ) {
		return &m_leftFadeIn;
	}
	if ( m_rightFadeIn.Inside( x, y ) ) {
		return &m_rightFadeIn;
	}
	if ( m_leftFadeOut.Inside( x, y ) ) {
		return &m_leftFadeOut;
	}
	if ( m_rightFadeOut.Inside( x, y ) ) {
		return &m_rightFadeOut;
	}
	return NULL;
}
TLItem::~TLItem()
{
//	std::cout << "Deleting Item: " << (const char*)m_sample->GetFilename().mb_str() << std::endl;
	m_sample->UnRef();
}
unsigned int TLItem::FillBuffer(float* outBuffer, gg_tl_dat pos, unsigned int count, bool mute, double volume)
{
	unsigned int written=0;
	float *buffer = m_sample->GetBuffer();
	if (m_position+GetLength()<pos)
	{
		return 0;
	}
	if (m_position>=pos)
	{
		for (unsigned int i=pos; i < m_position && written < count; i++)
		{
			outBuffer[i-pos]=0.0;
			written++;
		}
	}
	gg_tl_dat playingOffset = pos-m_position;
	if (playingOffset<0)
		playingOffset=0;
	while(written<count && playingOffset<GetLength())
	{
		if (mute) {
			outBuffer[written]=0.0;
		} else {
			outBuffer[written]=buffer[playingOffset]*volume;
		}
		written++;
		playingOffset++;
	}
	return written;

}
gg_tl_dat TLItem::GetLength()
{
	return m_sample->GetLength();
}
int TLItem::GetTrack()
{
	return m_trackListener->GetTrackNr();//m_trackNr;
}
gg_tl_dat TLItem::GetPosition()
{
	return m_position;
}
gg_tl_dat TLItem::GetEndPosition()
{
	return m_position+m_sample->GetLength();
}
void TLItem::SetPosition(gg_tl_dat position)
{
	if (position%2) /*ist das notwendig um Stereo korrekt wiederzugeben ?*/
		m_position=position+1;
	else
		m_position=position;
}
TLSample *TLItem::GetSample()
{
	return m_sample;
}
void TLItem::Select()
{
	m_selected=true;
}
void TLItem::UnSelect()
{
	m_selected=false;
}
bool TLItem::IsSelected()
{
	return m_selected;
}
long TLItem::GetReference() { return m_referenceId; }

