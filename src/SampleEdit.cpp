/* SampleEdit.cpp
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

#include "SoundTouch/SoundTouch.h"
//#include <soundtouch/SoundTouch.h>

#include "SampleEdit.h"

SampleEdit::SampleEdit( float *buffer, int len )
{
	m_buffer = buffer;
	m_len    = len;
	m_tempo  = 1.0;
	m_start  = -1;
	m_end    = -1;
}
SampleEdit::~SampleEdit()
{
}
void SampleEdit::SetTempo( float tempo )
{
	m_tempo = tempo;
}
void SampleEdit::SetTrims( int start, int end )
{
	m_start = start;
	m_end   = end;
}
float *SampleEdit::Convert( int &len )
{
//#ifdef __WXMSW__
#if 1
	soundtouch::SoundTouch pSoundTouch;
#else
	SoundTouch pSoundTouch;
#endif
	pSoundTouch.setSampleRate( 44100 );
	pSoundTouch.setChannels( 2 );
	pSoundTouch.setTempo( m_tempo );
	int tmp_len;
	float *buffer = Trim( tmp_len );
	len = int( tmp_len / m_tempo ) + 20;
	pSoundTouch.putSamples( buffer, tmp_len/2 );
	pSoundTouch.flush();
	float *new_buffer = new float[len];
	len = pSoundTouch.receiveSamples( new_buffer, len/2 );
	len = len*2;
	//TODO Buffer kleiner machen und kopieren ?
	return new_buffer;
}
float *SampleEdit::Trim( int &len )
{
	float *buffer;
	if ( m_start < 0 ) {
		m_start = 0;
	}
	if ( m_end < 0 || m_end > m_len / 2 ) {
		m_end = m_len / 2;
	} 
	buffer = &m_buffer[m_start*2];
	len = ( m_end * 2 ) - ( m_start * 2 );
	return buffer;
}
