/* PlayerInterface.cpp
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

#include "stuff.h"
#include "TLItem.h"
#include "TLData.h"
#include "TLSample.h"
#include "UpdateListener.h"
#include "PlayerInterface.h"

TimelinePlayer::TimelinePlayer( TLData *data )
{
	m_data = data;
	m_data->Block();
	m_data->SortAll();
}
TimelinePlayer::~TimelinePlayer()
{
	m_data->UnBlock();
}
int TimelinePlayer::FillBuffer( float *buffer, unsigned long frames )
{
//	float *out = (float*)outputBuffer;
//	SoundManager *bf = (SoundManager*)userData;
//	unsigned int res = bf->FillBuffer_TL(out,framesPerBuffer*2);
//	{
		int written_samples = m_data->FillLoopBuffer( buffer, frames * 2 );
		//m_position += written_samples;
		unsigned int res = written_samples;
//	}
	if ( res < frames * 2 ) {
		//for (int i=res;i<framesPerBuffer*2;i++) {
		//	out[i]=0.0;
		//}
		return 1;
	}
	return 0;
}
gg_tl_dat TimelinePlayer::GetPosition(){ return m_data->m_position; }
/* --------- */
SamplePlayer::SamplePlayer( TLSample *sample )
{
	m_sample   = sample;
	m_position = 0;
}
SamplePlayer::SamplePlayer( wxString filename, long &length,
		long &frames, long &channels,
		long &sampleRate,
		UpdateListener *updateListener )
{
	if (updateListener)
		updateListener->StartUpdateProcess();
	TLSample *sample = new TLSample(filename,0,NULL,updateListener);
	if (updateListener)
		updateListener->EndUpdateProcess();
	if (!sample->IsValid()) {
		if (updateListener->Update(100)==true) {
			wxLogError(wxT("Couldn't load Samplefile \"%s\""),filename.c_str());
		}
		delete sample;
		m_sample = 0;
		return;
	}
	length=sample->GetLength();
	frames=sample->m_infoFrames;
	channels=sample->m_infoChannels;
	sampleRate=sample->m_infoSampleRate;
	m_sample = sample;
	m_position = 0;
}
SamplePlayer::~SamplePlayer()
{
	if ( m_sample && m_sample->GetRefCount() == 0 ) {
		delete m_sample;
		m_sample = 0;
	}

}
int SamplePlayer::FillBuffer( float *buffer, unsigned long frames )
{
	//float *out = (float*)outputBuffer;
	unsigned int res = FillBuffer_Sample( buffer, frames * 2 );
	if (res < frames*2) {
		//for (int i=res;i<framesPerBuffer*2;i++) {
		//	out[i]=0.0;
		//}
		return 1;
	}
	return 0;
}
unsigned int SamplePlayer::FillBuffer_Sample(float* outBuffer, unsigned int count)
{
	float *buffer = m_sample->GetBuffer();
	int length = m_sample->GetLength();
	unsigned int i;
	for (i=0;i<count && length>m_position;i++) {
		outBuffer[i]=buffer[m_position];
		m_position++;
	}
	return i;
}

/* --------- */
LoopPlayer::LoopPlayer( float* sample, gg_tl_dat len )
{
	m_loopSample = sample;
	m_loopLen = len;
	m_loopPos = 0;
}
LoopPlayer::~LoopPlayer()
{
	if ( m_loopSample ) {
		delete [] m_loopSample;
		m_loopSample = 0;
	}
}
int LoopPlayer::FillBuffer( float *buffer, unsigned long frames )
{
	unsigned long count = 0;
	while ( count < frames ) {
		if ( m_loopPos >= m_loopLen ) {
			m_loopPos = 0;
		}
		buffer[count*2] = m_loopSample[m_loopPos];
		buffer[count*2+1] = m_loopSample[m_loopPos+1];
		count++;
		m_loopPos += 2;
	}
	return 0;
}

