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

#include <wx/filename.h>

#include <math.h>
#include <sndfile.h>
#include <soundtouch/SoundTouch.h>

#include "stuff.h"
#include "TLItem.h"
#include "TLTrack.h"
#include "TLSample.h"
#include "SampleEdit.h"
#include "TLView.h"
#include <iostream>



////#define ZOOM_FACTOR;
void GetRectsFromEnv( const NativeEnvData& e, gg_tl_dat SampleLength, wxRect *result )
{
	float yVariation = float ( TRACK_HEIGHT - RECT_LENGTH );
	float xVariation = float ( SampleLength / g_ggseqProps.GetZoom() ) - RECT_LENGTH;
	result[0] = wxRect( 0, (int)roundf(yVariation - e.leftFadeLevel * yVariation), RECT_LENGTH, RECT_LENGTH );
	result[1] = wxRect( (int)roundf(e.leftFadePos * xVariation), (int)roundf(yVariation - e.middleLevel * yVariation), RECT_LENGTH, RECT_LENGTH );
	result[2] = wxRect( (int)roundf(e.rightFadePos * xVariation), (int)roundf(yVariation - e.middleLevel * yVariation), RECT_LENGTH, RECT_LENGTH );
	result[3] = wxRect( int(xVariation), (int)roundf(yVariation - e.rightFadeLevel * yVariation), RECT_LENGTH, RECT_LENGTH );
}
void GetEnvFromRects( NativeEnvData& e, const wxRect* rect, gg_tl_dat SampleLength )
{
	float yVariation   = float (TRACK_HEIGHT - RECT_LENGTH);
	float xVariation   = float ( SampleLength / g_ggseqProps.GetZoom() ) - RECT_LENGTH;
	e.leftFadeLevel  = (yVariation - rect[0].y) / yVariation;
	e.leftFadePos    = rect[1].x / xVariation;
	e.middleLevel    = (yVariation - rect[1].y) / yVariation;
	e.rightFadeLevel = (yVariation - rect[3].y) / yVariation;
	e.rightFadePos   = rect[2].x / xVariation;
}
void GetSampleEnvelope( EnvelopePoint *result, const NativeEnvData& e, gg_tl_dat SampleLength )
{
	result[0].x = 0;
	result[0].y = e.leftFadeLevel;
	result[1].x = e.leftFadePos * SampleLength;
	result[1].y = e.middleLevel;
	result[2].x = e.rightFadePos * SampleLength;
	result[2].y = e.middleLevel;
	result[3].x = SampleLength;
	result[3].y = e.rightFadeLevel;
}

//TODO: trackNr überall eliminieren
TLItem::TLItem(TLSample *sample , gg_tl_dat position, long reference, GetItemTrackListener* trackListener )
{
	m_sample=sample;
	m_position=position;
	m_sample->Ref();
	m_selected=false;
	m_toggleEnvelope = false;
	m_referenceId = reference;
	m_trackListener = trackListener;
	m_nativeEnvData.leftFadeLevel  = 1.0;
	m_nativeEnvData.leftFadePos    = 0.5;
	m_nativeEnvData.middleLevel    = 1.0;
	m_nativeEnvData.rightFadeLevel = 1.0;
	m_nativeEnvData.rightFadePos   = 0.5;
	
	m_stretchedBuffer = 0;
	m_stretchedLen = 0;
	m_timestretch = 1.0;
	m_leftTrim = 0;
	m_rightTrim = m_sample->GetLength() / 2;
	GetRectsFromEnv( m_nativeEnvData, GetLen(), m_guiEnvData );
	GetSampleEnvelope( m_sampleEnvData, m_nativeEnvData, GetLen() );

}
TLItem::TLItem( TLSample *sample, const ItemEssentials& e, GetItemTrackListener* trackListener )
{
	m_sample=sample;
	m_position=e.position;
	m_sample->Ref();
	m_selected=false;
	m_toggleEnvelope = e.toggleEnvelope;
	m_referenceId = e.referenceId;
	m_trackListener = trackListener;
	m_nativeEnvData  = e.nativeEnvData;
	
	m_stretchedBuffer = 0;
	m_stretchedLen = 0;
/*	m_timestretch = 1.0;
	m_leftTrim = e.leftTrim;
	m_rightTrim = e.rightTrim;*/
	SetTrimNStretch( e.leftTrim, e.rightTrim, e.timestretch );
		 
	GetRectsFromEnv( m_nativeEnvData, GetLen(), m_guiEnvData );
	GetSampleEnvelope( m_sampleEnvData, m_nativeEnvData, GetLen() );
}
void DrawWxRect( wxDC &dc, const wxRect &rect ) // TODO make Helper Functions File
{
	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}
void TLItem::DrawEnvelope( wxDC &dc, int xOffset,
		int yOffset,bool updateEnv,
		wxRect *customFades,
		float zoom )
{
	if (updateEnv) {
		GetRectsFromEnv( m_nativeEnvData, GetLen(), m_guiEnvData );
	}
	dc.SetDeviceOrigin( xOffset, yOffset );
	wxPoint lines[4];/* = {
		wxPoint( m_leftFadeIn.x, m_leftFadeIn.y ),
		wxPoint( m_rightFadeIn.x, m_rightFadeIn.y ),
		wxPoint( m_leftFadeOut.x, m_leftFadeOut.y ),
		wxPoint( m_rightFadeOut.x, m_rightFadeOut.y ) };*/
	wxRect *fades = ( customFades ? customFades : m_guiEnvData );
	for ( int i = 0; i < 4; i++ ) {
		lines[i] = wxPoint( fades[i].x, fades[i].y );
	}
	dc.DrawLines( 4, lines, 3, 3 );
	dc.SetBrush(*wxWHITE_BRUSH);
	for ( int i = 0; i < 4; i++ ) {
		DrawWxRect( dc, fades[i] );
	}
/*	DrawWxRect( dc, m_leftFadeIn );
	DrawWxRect( dc, m_rightFadeIn );
	DrawWxRect( dc, m_leftFadeOut );
	DrawWxRect( dc, m_rightFadeOut );*/
	dc.SetDeviceOrigin( 0, 0 );
}
wxRect *TLItem::GetEnvelopeHandle( int x, int y, float zoom )
{
	for ( int i = 0; i < 4; i++ ) {
		if ( m_guiEnvData[i].Inside( x, y ) ) {
			return &m_guiEnvData[i];
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
		delete [] m_stretchedBuffer;
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

*/
#define LEN_ENVELOPE 4
float TLItem::GetEnvelopValue( int position )
{
	if ( !m_toggleEnvelope ) {
		return 1.0;
	}
	EnvelopePoint *envelope = m_sampleEnvData;
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
	GetEnvFromRects( m_nativeEnvData, m_guiEnvData, GetLen() );
}
unsigned int TLItem::FillBuffer(float* outBuffer, gg_tl_dat pos, unsigned int count, bool mute, double volume)
{
	unsigned int written=0;
	float *buffer;
	if (m_stretchedBuffer) {
		buffer = m_stretchedBuffer;
	} else {
		buffer = m_sample->GetBuffer();
	}
	//float env[count];
	if (m_position+GetLen()<pos)
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
	while(written<count && playingOffset<GetLen())
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
gg_tl_dat TLItem::GetLen()
{
	if (m_stretchedBuffer) {
		return m_stretchedLen;
	} else {
		return m_sample->GetLength();
	}
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
	return m_position+GetLen();
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

void TLItem::SetTrimNStretch( gg_tl_dat leftTrim,
		gg_tl_dat rightTrim, float timestretch )
//Stretch( float amount, gg_tl_dat trimStart, gg_tl_dat trimEnd )
{
	m_leftTrim = leftTrim;
	m_rightTrim = rightTrim;
	m_timestretch = timestretch;
	if ( m_stretchedBuffer ) {
		delete [] m_stretchedBuffer;
		m_stretchedBuffer = 0;
	}
	if ( leftTrim == 0 && rightTrim*2 == m_sample->GetLength() && m_timestretch == 1.0 ) {
		return;
	}
	SampleEdit pSampleEdit( m_sample->GetBuffer(), m_sample->GetLength() );
	pSampleEdit.SetTempo( timestretch );
	pSampleEdit.SetTrims( leftTrim, rightTrim );
	int len;
	m_stretchedBuffer = pSampleEdit.Convert( len );
	m_stretchedLen = len;
	GetRectsFromEnv( m_nativeEnvData, GetLen(), m_guiEnvData );
	GetSampleEnvelope( m_sampleEnvData, m_nativeEnvData, GetLen() );
}
void TLItem::GetEnvelopeData( NativeEnvData &env )
{
	env = m_nativeEnvData;
}
void TLItem::GetNewEnvelopeData( NativeEnvData &env,
		wxRect *handle )
{
	env = m_nativeEnvData;
	float yVariation   = float (TRACK_HEIGHT - RECT_LENGTH);
	float xVariation   = float ( GetLen() / g_ggseqProps.GetZoom() ) - RECT_LENGTH;
	if ( handle == (&(m_guiEnvData[0])) ) {
		env.leftFadeLevel  = (yVariation - handle->y) / yVariation;
	}
	else if ( handle == (&(m_guiEnvData[1])) ) {
		env.leftFadePos    = handle->x / xVariation;
		env.middleLevel    = (yVariation - handle->y) / yVariation;
		env.rightFadePos   = m_guiEnvData[2].x / xVariation;
	}
	else if ( handle == (&(m_guiEnvData[2])) ) {
		env.middleLevel    = (yVariation - handle->y) / yVariation;
		env.rightFadePos   = handle->x / xVariation;
		env.leftFadePos    = m_guiEnvData[1].x / xVariation;
	}
	else if ( handle == (&(m_guiEnvData[3])) ) {
		env.rightFadeLevel = (yVariation - handle->y) / yVariation;
	}
}
float *TLItem::GetTrimNStretchBuffer()
{
	return m_stretchedBuffer;
}
gg_tl_dat TLItem::GetLeftTrim()
{
	return m_leftTrim;
}
gg_tl_dat TLItem::GetRightTrim()
{
	return m_rightTrim;
}
float TLItem::GetTimestretch()
{
	return m_timestretch;
}
void TLItem::Draw( wxDC& dc, float zoom )
{
	wxBrush b1 = dc.GetBrush();
	int width = GetLen()/ zoom;//3793;
	TLView::Draw3dRect( &dc,0,0,width,25,m_sample->GetColour() );
	dc.SetPen( *wxBLACK_PEN );
	dc.SetClippingRegion( 0, 0, width, 25 );
	dc.SetFont( *wxSMALL_FONT );
	wxFileName fn( m_sample->GetFilename() );
	dc.DrawText( fn.GetName(), 0, 1 );
	dc.DestroyClippingRegion();
}
void TLItem::DragEnvelopeHandle( wxRect *handle )
{
	if ( handle == (&(m_guiEnvData[0])) ) {
		handle->x = 0;
	}
	if ( handle == (&(m_guiEnvData[3])) ) {
		handle->x = GetLen() / g_ggseqProps.GetZoom() - 7;
	}
	if ( handle == (&(m_guiEnvData[1])) ) {
		m_guiEnvData[2].y = handle->y;
		if (m_guiEnvData[2].x < m_guiEnvData[1].x ) {
			m_guiEnvData[2].x = m_guiEnvData[1].x;
		}
	}
	if ( handle == (&(m_guiEnvData[2])) ) {
		m_guiEnvData[1].y = handle->y;
		if (m_guiEnvData[2].x < m_guiEnvData[1].x ) {
			m_guiEnvData[1].x = m_guiEnvData[2].x;
		}
	}
}
float *TLItem::GetBuffer() { return m_sample->GetBuffer(); }
gg_tl_dat TLItem::GetBufferLen() { return m_sample->GetLength(); }
float *TLItem::GetStretchedBuffer() { return m_stretchedBuffer; }

void TLItem::SetEnvelopeData( const NativeEnvData &env )
{
	m_nativeEnvData = env;
	GetRectsFromEnv( m_nativeEnvData, GetLen(), m_guiEnvData );
	GetSampleEnvelope( m_sampleEnvData, m_nativeEnvData, GetLen() );
}
void TLItem::GetEssentials( ItemEssentials &e )
{
	e.position = m_position;
	e.toggleEnvelope = m_toggleEnvelope;
	e.referenceId = m_referenceId;
	e.nativeEnvData = m_nativeEnvData;
	e.timestretch = m_timestretch;
	e.leftTrim = m_leftTrim;
	e.rightTrim = m_rightTrim;
	e.filename = m_sample->GetFilename();
	e.trackId = GetTrack();
}
