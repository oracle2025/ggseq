
/* SoundManager.cpp
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

#include <portaudio.h>
#include "stuff.h"
#include "TLData.h"
#include "TLTrack.h"
#include "TLItem.h"
#include "TLSample.h"
#include "SoundManager.h"
#include "UpdateListener.h"
#include "PlayerInterface.h"

//DEFINE_EVENT_TYPE(wxEVT_DONE_SAMPLE_COMMAND)

/*
PlayerInterface
	TimelinePlayer
	SamplePlayer
	LoopEditPlayer
class PlayerInterface {
	int FillBuffer( float *, unsigned long frames );
}
*/


static PortAudioStream* stream;

static int callback(void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
	PlayerInterface *PlIface = (PlayerInterface*)userData;
	return PlIface->FillBuffer( (float*)outputBuffer, framesPerBuffer );
}
/*
static int callback(void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
	float *out = (float*)outputBuffer;
	SoundManager *bf = (SoundManager*)userData;
	unsigned int res = bf->FillBuffer_TL(out,framesPerBuffer*2);
	if (res < framesPerBuffer*2) {
		//for (int i=res;i<framesPerBuffer*2;i++) {
		//	out[i]=0.0;
		//}
		return 1;
	}
	return 0;
}
static int callback_sample(void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
	float *out = (float*)outputBuffer;
	SoundManager *bf = (SoundManager*)userData;
	unsigned int res = bf->FillBuffer_Sample(out,framesPerBuffer*2);
	if (res < framesPerBuffer*2) {
		//for (int i=res;i<framesPerBuffer*2;i++) {
		//	out[i]=0.0;
		//}
		return 1;
	}
	return 0;
}
static int callback_loop(void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
	float *out = (float*)outputBuffer;
	SoundManager *bf = (SoundManager*)userData;
	unsigned long count = 0;
	while ( count < framesPerBuffer ) {
		if ( bf->m_loopPos >= bf->m_loopLen ) {
			bf->m_loopPos = 0;
		}
		out[count*2] = bf->m_loopSample[bf->m_loopPos];
		out[count*2+1] = bf->m_loopSample[bf->m_loopPos+1];
		count++;
		bf->m_loopPos += 2;
	}
	return 0;
}*/

/*BEGIN_EVENT_TABLE(SoundManager, wxEvtHandler)
	EVT_DONE_SAMPLE_COMMAND(-1,SoundManager::OnDonePlayingSample)
END_EVENT_TABLE()*/

SoundManager::SoundManager(TLData *TlData)//:wxEvtHandler()
{
	m_data = TlData;
	//m_tlPlaying = false;
	//m_samplePlaying=false;
	//m_loopPlaying=false;
	//m_position = 0;
	g_ggseqProps.SetSoundManager( this );
	m_playerInterface = 0;
}
SoundManager::~SoundManager()
{
}
void SoundManager::Play( PlayerInterface *playerIface )
{
	Stop();
	m_playerInterface = playerIface;
	StartStream((void*)callback);
}
/*void SoundManager::Play()
{
	m_data->Block();
	if (m_tlPlaying)
		return;
	if (m_samplePlaying)
		Stop();

	m_data->SortAll();
	m_position=m_data->GetPlaybackPosition();
	m_tlPlaying = true;
	StartStream((void*)callback);
}
unsigned int SoundManager::FillBuffer_TL(float* outBuffer, unsigned int count)
{
	int written_samples=m_data->FillLoopBuffer(outBuffer,count);
	m_position+=written_samples;
	return written_samples;
}*/
void SoundManager::Stop()
{
	if ( !m_playerInterface )
		return;
	StopStream();
	delete m_playerInterface;
	m_playerInterface = 0;
}
/*void SoundManager::Stop()
{
	if (!m_tlPlaying && !m_samplePlaying && !m_loopPlaying)
		return;
	StopStream();
	if (m_samplePlaying) {
		if (m_sample->GetRefCount()==0) {
			delete m_sample;
		}
	}
	if (m_tlPlaying)
		m_data->UnBlock();
	m_tlPlaying = false;
	m_samplePlaying=false;
	m_loopPlaying=false;
}*/
/*unsigned int SoundManager::FillBuffer_Sample(float* outBuffer, unsigned int count)
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

void SoundManager::Play(wxString filename, long &length,long &frames,long &channels, long &sampleRate, UpdateListener *updateListener)
{
	if (m_tlPlaying)
		return;
	if (m_samplePlaying||m_loopPlaying)
		Stop();
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
		return;
	}
	
	length=sample->GetLength();
	frames=sample->m_infoFrames;
	channels=sample->m_infoChannels;
	sampleRate=sample->m_infoSampleRate;
	Play(sample);
}
void SoundManager::Play(TLSample *sample)
{
	if (m_tlPlaying)
		return;
	if (m_samplePlaying||m_loopPlaying)
		Stop();
	m_sample = sample;//new TLSample(filename,0,NULL);
	m_position=0;
	m_samplePlaying=true;
	StartStream((void*)callback_sample);
}
void SoundManager::Loop( float* sample, gg_tl_dat len )
{
	m_loopPos = 0;
	m_loopSample = sample;
	m_loopLen = len;
	m_loopPlaying = true;
	StartStream((void*)callback_loop);
}*/
void SoundManager::StartStream(void* callback)
{
	PortAudioCallback *cb=(PortAudioCallback*)callback;
	PaError err;
	err = Pa_Initialize();
	if (err!=paNoError) goto error;
	err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, 0, cb, m_playerInterface /*this*/);
	if( err != paNoError ) goto error;
	err = Pa_StartStream(stream);
	if( err != paNoError ) goto error;
	return;
	error:
	wxLogError(wxT("Soundoutput failed: \"%s\""),Pa_GetErrorText( err ));
	//m_tlPlaying = false;
	//m_samplePlaying = false;
	//m_loopPlaying = false;
	return;
}
void SoundManager::StopStream()
{
	PaError err;
	err = Pa_StopStream(stream);
	if(err!=paNoError) goto error;
	err = Pa_CloseStream(stream);
	if(err!=paNoError) goto error;
	Pa_Terminate();
	return;
	error:
	wxLogError(wxT("Portaudio Error: \"%s\""),Pa_GetErrorText( err ));
	return;
}

gg_tl_dat SoundManager::GetPosition()
{
	if ( m_playerInterface ) {
		return m_playerInterface->GetPosition();
	}
	return 0;
/*	if (m_tlPlaying)
		return m_data->m_position;
	return m_position;*/
} /*TODO beim TL-Data die Position aus data auslesen*/
bool SoundManager::Done()
{
	if ( !m_playerInterface )
		return true;
	/*if (!m_tlPlaying && !m_samplePlaying)
		return true;*/
	if ( Pa_StreamActive(stream) <= 0 )
		return true;
	else
		return false;
}
