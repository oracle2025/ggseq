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
#include <soundtouch/SoundTouch.h>

#include "stuff.h"
#include "TLItem.h"
#include "TLTrack.h"
#include "TLSample.h"
#include "SampleEdit.h"
#include <iostream>

/*
typedef struct {
	Env realEnvelope[4];
	wxRect leftFadeIn;   //??
	wxRect rightFadeIn;   //??
	wxRect leftFadeOut;   //??
	wxRect rightFadeOut;   //??
	bool toggleEnvelope;
	float timeStretch
	gg_tl_dat trimStart;
	gg_tl_dat trimEnd;
} ItemMods;

 */

//TODO: trackNr überall eliminieren
TLItem::TLItem(TLSample *sample/*, int trackNr*/ , gg_tl_dat position, long reference, GetItemTrackListener* trackListener )
{
	m_sample=sample;
	m_position=position;
	m_sample->Ref();
	m_selected=false;
	m_toggleEnvelope = false;
	//m_trackNr = trackNr;
	m_referenceId = reference;
	m_trackListener = trackListener;
	//m_x_test = 10;
	//m_y_test = 10;
/*	m_leftFadeIn   = wxRect( 0, 0, 7, 7 );
	m_rightFadeIn  = wxRect( 10, 0, 7, 7 );
	m_leftFadeOut  = wxRect( 20, 0, 7, 7 );
	m_rightFadeOut = wxRect( (int)(m_sample->GetLength() / ( 117600.0 / 31.0 )-7), 0, 7, 7 );
	m_rightFadeIn.x = m_rightFadeOut.x / 2;
	m_leftFadeOut.x = m_rightFadeIn.x;*/
	m_fades[0]   = wxRect( 0, 0, 7, 7 );
	m_fades[1]  = wxRect( 10, 0, 7, 7 );
	m_fades[2]  = wxRect( 20, 0, 7, 7 );
	m_fades[3] = wxRect( (int)(m_sample->GetLength() / ( 117600.0 / 31.0 )-7), 0, 7, 7 );
	m_fades[1].x = m_fades[3].x / 2;
	m_fades[2].x = m_fades[1].x;
	GuiEnvToDataEnv();
	m_stretchedBuffer = 0;

}
void DrawWxRect( wxDC &dc, const wxRect &rect ) // TODO make Helper Functions File
{
	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}
void TLItem::DrawEnvelope( wxDC &dc, int xOffset, int yOffset, wxRect *customFades )
{
	dc.SetDeviceOrigin( xOffset, yOffset );
	wxPoint lines[4];/* = {
		wxPoint( m_leftFadeIn.x, m_leftFadeIn.y ),
		wxPoint( m_rightFadeIn.x, m_rightFadeIn.y ),
		wxPoint( m_leftFadeOut.x, m_leftFadeOut.y ),
		wxPoint( m_rightFadeOut.x, m_rightFadeOut.y ) };*/
	wxRect *fades = ( customFades ? customFades : m_fades );
	for ( int i = 0; i < 4; i++ ) {
		lines[i] = wxPoint( fades[i].x, fades[i].y );
	}
	dc.DrawLines( 4, lines, 3, 3 );
	for ( int i = 0; i < 4; i++ ) {
		DrawWxRect( dc, fades[i] );
	}
/*	DrawWxRect( dc, m_leftFadeIn );
	DrawWxRect( dc, m_rightFadeIn );
	DrawWxRect( dc, m_leftFadeOut );
	DrawWxRect( dc, m_rightFadeOut );*/
	dc.SetDeviceOrigin( 0, 0 );
}
wxRect *TLItem::TouchingEnvelopeCtrl( int x, int y )
{
	for ( int i = 0; i < 4; i++ ) {
		if ( m_fades[i].Inside( x, y ) ) {
			return &m_fades[i];
		}
	}
/*	if ( m_leftFadeIn.Inside( x, y ) ) {
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
	}*/
	return NULL;
}
TLItem::~TLItem()
{
//	std::cout << "Deleting Item: " << (const char*)m_sample->GetFilename().mb_str() << std::endl;
	if ( m_stretchedBuffer ) {
		delete m_stretchedBuffer;
		m_stretchedBuffer = 0;
	}
	m_sample->UnRef();
}
float *get_keys(int a, int b, int size)
{
	float *result = new float[size];
	float diff = (float)b-a;
	float inc = (float)diff / (size - 1);
	result[0] = (float)a;
	for ( int i = 1; i < size; ++i ) {
		result[i] = result[i-1]+inc;
	}
	return result;
}
/*
 *       
 *         <------------------>
 *              width
 *                             []
 *        []                   ^
 *        ^                    |end_level
 *        |                    |
 *        |start_level         |
 *        |                    |
 *        v____________________v______
 *              
 *                 size v
 *              |----------|
 *              ^position
 */
/*
float* Generate_envelope(float* env ,start_level, width, end_level, position, size)
{
	int a;
	int b;
	int diff = end_level - start_level;
	float inc = (float)diff / (float)width;
	a = start_level + ( position * inc );
	b = start_level + ( ( position + size ) * inc );
	float *keys = get_keys(a, b, size)
	for ( int i = 0; i < size; i++ )
		env[i] = keys[i];
	delete [] keys;
}
float TLItem::FadeInStartLevel() {}
float TLItem::FadeInEndLevel() {}
gg_tl_dat TLItem::FadeInEndPosition() {}
float TLItem::FadeOutStartLevel() {}
float TLItem::FadeOutEndLevel() {}
gg_tl_dat TLItem::FadeOutStartPosition() {}

//TODO: Stereo berücksichtigen
void TLItem::Envelope(float *buffer, gg_tl_dat position_in_sample, gg_tl_dat len )
{
  // Buffer:        |-------------|
  // Sample:  |--------------------------|
  //          +-----+ <- position_in_sample
  //                
  //                +-------------+ <- len
	if ( position_in_sample < FadeInEndPosition ) {
		Generate_envelope( buffer, FadeInStartLevel(),
		                   FadeInEndPosition(), FadeInEndLevel(),
		                   position_in_sample, len ); //TODO: Achtung schreibt über das Ende von EndPosition hinaus.
	}
	if ( position_in_sample + len > FadeOutStartPosition ) {
		Generate_envelope( buffer + ( FadeOutStartPosition() - position_in_sample ),
		                   FadeOutStartLevel(), m_length - FadeOutStartPosition(),
		                   FadeOutEndLevel(),position_in_sample + what_was_written_before,
				   len - what_was_written_before );
	}
} 
typdef struct {
	int x;    // Position in Samples
	float y;  // 0.0 bis 1.0
} Env;

void TLItem::GuiEnvToDataEnv()
{
	m_realEnvelope[0].x = 0;
	m_realEnvelope[0].y = float(m_leftFadeIn.y) / 18.0; //Trackhöhe;
	m_realEnvelope[1].x = m_rightFadeIn.x * ( 117600.0 / 31.0 );
	m_realEnvelope[1].y = float(m_rightFadeIn.y) / 18.0;
	m_realEnvelope[2].x = m_leftFadeOut.x * ( 117600.0 / 31.0 );
	m_realEnvelope[2].y = float(m_leftFadeOut.y)  / 18.0;
	m_realEnvelope[3].x = m_rightFadeOut.x * ( 117600.0 / 31.0 );
	m_realEnvelope[3].y = float(m_rightFadeOut.y)  / 18.0;
}

Env envelope[LEN_ENVELOPE];
*/
#define LEN_ENVELOPE 4
float TLItem::GetEnvelopValue( int position )
{
	if ( !m_toggleEnvelope ) {
		return 1.0;
	}
	EnvelopePoint *envelope = m_realEnvelope;
	int p = 0;
	while ( envelope[p + 1].x < position) {
		p++;
		if ( LEN_ENVELOPE - 1 == p ) {
			return 0.0; // Error
		}
	}
	float diff = envelope[p + 1].y - envelope[p].y;
	int width = envelope[p + 1].x - envelope[p].x;
	float inc = diff / float(width);
	return envelope[p].y + inc * ( position - envelope[p].x );
}
void TLItem::GuiEnvToDataEnv()
{
	m_realEnvelope[0].x = 0;
	m_realEnvelope[0].y = float(18 - m_fades[0].y) / 18.0; //Trackhöhe;
	m_realEnvelope[1].x = int(m_fades[1].x * ( 117600.0 / 31.0 ) );
	m_realEnvelope[1].y = float(18 - m_fades[1].y) / 18.0;
	m_realEnvelope[2].x = int(m_fades[2].x * ( 117600.0 / 31.0 ) );
	m_realEnvelope[2].y = float(18 - m_fades[2].y)  / 18.0;
	m_realEnvelope[3].x = m_sample->GetLength();//int(m_rightFadeOut.x * ( 117600.0 / 31.0 ) );
	m_realEnvelope[3].y = float(18 - m_fades[3].y)  / 18.0;
}
void TLItem::DataEnvToGuiEnv()
{
	m_fades[0].y = 18 - int( m_realEnvelope[0].y * 18.0 );
	m_fades[1].y = 18 - int( m_realEnvelope[1].y * 18.0 );
	m_fades[3].y = 18 - int( m_realEnvelope[3].y * 18.0 );
	m_fades[2].y = m_fades[1].y;
	m_fades[1].x = int( m_realEnvelope[1].x / ( 117600.0 / 31.0 ) );
	m_fades[2].x = int( m_realEnvelope[2].x / ( 117600.0 / 31.0 ) );
	//----
/*	m_fades[0]   = wxRect( 0, 0, 7, 7 );
	m_fades[1]  = wxRect( 10, 0, 7, 7 );
	m_fades[2]  = wxRect( 20, 0, 7, 7 );
	m_fades[3] = wxRect( (int)(m_sample->GetLength() / ( 117600.0 / 31.0 )-7), 0, 7, 7 );
	m_fades[1].x = m_fades[3].x / 2;
	m_fades[2].x = m_fades[1].x;*/
}
unsigned int TLItem::FillBuffer(float* outBuffer, gg_tl_dat pos, unsigned int count, bool mute, double volume)
{
	unsigned int written=0;
	float *buffer = m_sample->GetBuffer();
	//float env[count];
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
	if (playingOffset<0) //ASSERT: shouldn't happen ?
		playingOffset=0;
	//TODO: Generate Envelope
	/*
	 * Wir sind genau hier im OutputFrame:
	 *  |----------------|
	 *     ^playingOffset
	 *  Wenn rightFadeIn innerhalb des Frames liegt, dann nur bis dahin berechnen.
	 *
	 * (18 - m_leftFadeIn.y)/18 = start_level
	 * m_rightFadeIn.m_x* X = width
	 * (18 - m_rightFadeIn.m_y)/18 = end_level
	 * position
	 * size
	 * Rest mit 1 auffüllen
	 */
	while(written<count && playingOffset<GetLength())
	{
		if (mute) {
			outBuffer[written]=0.0;
		} else {
			//Generate_envelope(env, ...)
			outBuffer[written]=buffer[playingOffset] * volume * GetEnvelopValue(playingOffset);
			//TODO: implement Envelope here
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
void TLItem::Stretch( float amount, gg_tl_dat trimStart, gg_tl_dat trimEnd )
{
	SampleEdit pSampleEdit( m_sample->GetBuffer(), m_sample->GetLength() );
	pSampleEdit.SetTempo( amount );
	pSampleEdit.SetTrims( trimStart, trimEnd );
	int len;
	m_stretchedBuffer = pSampleEdit.Convert( len );
	m_stretchedLen = len;
}
