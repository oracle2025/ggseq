
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

#include "portaudio/portaudio.h"
#include "stuff.h"
#include "TLItem.h"
#include "TLData.h"
#include "TLTrack.h"
#include "TLSample.h"
#include "SoundManager.h"
#include "UpdateListener.h"
#include "PlayerInterface.h"


static PortAudioStream* stream;

static int callback(void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
{
	PlayerInterface *PlIface = (PlayerInterface*)userData;
	return PlIface->FillBuffer( (float*)outputBuffer, framesPerBuffer );
}
SoundManager::SoundManager(TLData *TlData)//:wxEvtHandler()
{
	m_data = TlData;
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
void SoundManager::Stop()
{
	if ( !m_playerInterface )
		return;
	StopStream();
	delete m_playerInterface;
	m_playerInterface = 0;
}
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
} bool SoundManager::Done()
{
	if ( !m_playerInterface )
		return true;
	if ( Pa_StreamActive(stream) <= 0 )
		return true;
	else
		return false;
}
