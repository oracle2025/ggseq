
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

#include "TLData.h"
#include "TLTrack.h"
#include "TLItem.h"
#include "TLSample.h"
#include "SoundManager.h"

DEFINE_EVENT_TYPE(wxEVT_DONE_SAMPLE_COMMAND)

static PortAudioStream* stream;

static int callback(void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
	float *out = (float*)outputBuffer;
	SoundManager *bf = (SoundManager*)userData;
	unsigned int res = bf->FillBuffer_TL(out,framesPerBuffer*2);
	if (res < framesPerBuffer*2) {
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
		return 1;
	}
	return 0;
}


BEGIN_EVENT_TABLE(SoundManager, wxEvtHandler)
	EVT_DONE_SAMPLE_COMMAND(-1,SoundManager::OnDonePlayingSample)
END_EVENT_TABLE()

SoundManager::SoundManager(TLData *TlData):wxEvtHandler()
{
	m_data = TlData;
	m_playing = false;
	m_samplePlaying=false;
//	m_evtHan=evtHan;
}
SoundManager::~SoundManager()
{
}
void SoundManager::Play_TL()
{
	m_data->Block();
	if (m_playing)
		return;
	if (m_samplePlaying)
		Stop_Sample();

	m_data->SortAll();
	m_position=m_data->GetPlaybackPosition();
	m_positionBetweenEv=0;
	m_playing = true;
	StartStream((void*)callback);
}
unsigned int SoundManager::FillBuffer_TL(float* outBuffer, unsigned int count)
{
	int rv=m_data->FillBuffer(outBuffer,count);
	m_position+=rv;
	if (m_position >m_positionBetweenEv) {
/*		wxCommandEvent eventCustom(wxEVT_UPDATE_CARET_COMMAND);
		eventCustom.SetInt(m_position);
		wxPostEvent(m_evtHan, eventCustom);*/
		m_positionBetweenEv=m_position+11760;
	}
	return rv;
}

void SoundManager::Stop_TL()
{
	if (!m_playing)
		return;
	puts("fertisch");

	m_playing = false;
	m_data->UnBlock();
//	std::cout << "written: " << m_position << std::endl;
	StopStream();
}

unsigned int SoundManager::FillBuffer_Sample(float* outBuffer, unsigned int count)
{
	float *buffer = m_sample->GetBuffer();
	int length = m_sample->GetLength();
	unsigned int i;
	for (i=0;i<count && length>m_position;i++) {
		outBuffer[i]=buffer[m_position];
		m_position++;
	}
	if (i<count) {
		wxCommandEvent eventCustom(wxEVT_DONE_SAMPLE_COMMAND);
		wxPostEvent(this, eventCustom);
	}
	return i;
}

void SoundManager::Play_Sample(wxString filename)
{
	if (m_playing)
		return;
	if (m_samplePlaying)
		Stop_Sample();
	m_sample = new TLSample(filename,0,NULL);
	if (!m_sample->IsValid()) {
		wxLogError(wxT("Couldn't load Samplefile \"%s\""),filename.c_str());
		delete m_sample;
		return;
	}
	m_position=0;
	m_samplePlaying=true;
	StartStream((void*)callback_sample);
}
void SoundManager::OnDonePlayingSample(wxCommandEvent& event)
{
	Stop_Sample();
}
void SoundManager::Stop_Sample()
{
	if (!m_samplePlaying)
		return;
	puts("OnDonePlayingSample");
	StopStream();
	m_samplePlaying=false;
	delete m_sample;
}
void SoundManager::StartStream(void* callback)
{
	PortAudioCallback *cb=(PortAudioCallback*)callback;
	PaError err;
	err = Pa_Initialize();
	if (err!=paNoError) goto error;
	err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, 0, cb, this);
	if( err != paNoError ) goto error;
	err = Pa_StartStream(stream);
	if( err != paNoError ) goto error;
	return;
	error:
	wxLogError(wxT("Soundoutput failed"));
	m_playing = false;
	m_samplePlaying = false;
//	std::cerr << "An error occured while using the portaudio stream\n";
//	std::cerr << "Error number: " << err << std::endl;
//	std::cerr << "Error message: " << Pa_GetErrorText( err ) << std::endl;
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
	wxLogError(wxT("Portaudio Error!"));
//	std::cerr << "An error occured while using the portaudio stream\n";
//	std::cerr << "Error number: " << err << std::endl;
//	std::cerr << "Error message: " << Pa_GetErrorText( err ) << std::endl;
	return;
}

int SoundManager::GetPlaybackPosition()
{
	return /*m_data->GetPlaybackPosition()+*/m_position;//(int)Pa_StreamTime(stream)*2;
//	return Pa_StreamTime(stream);
}
bool SoundManager::Done_PlayingTL()
{
	if (!m_playing)
		return true;
	if (Pa_StreamActive(stream)<=0)
		return true;
	else
		return false;
}
