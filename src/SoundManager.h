/* SoundManager.h
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

#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_
class TLData;
class TLSample;

/*BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_DONE_SAMPLE_COMMAND, 7778)
END_DECLARE_EVENT_TYPES()

#define EVT_DONE_SAMPLE_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_DONE_SAMPLE_COMMAND, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
    ),*/
class SoundManager// : public wxEvtHandler
{
	public:
		SoundManager(TLData *TlData);
		~SoundManager();
//		void Play_TL();
//		void Play_Sample(wxString filename);
//		void Stop_TL();
//		void Stop_Sample();
//		bool Done_PlayingTL();
//		void OnDonePlayingSample(wxCommandEvent& event);
//		int GetPlaybackPosition();
		void Play();
		void Play(wxString filename, long &length);
		void Stop();
		bool Done();
		long GetPosition();
		
		unsigned int FillBuffer_TL(float* outBuffer, unsigned int count);
		unsigned int FillBuffer_Sample(float* outBuffer, unsigned int count);
	private:
		void StartStream(void* callback);
		void StopStream();
		TLData *m_data;
		bool m_tlPlaying;
		bool m_samplePlaying;
		long m_position;
		TLSample *m_sample;
//		DECLARE_EVENT_TABLE()
};

#endif /*_SOUNDMANAGER_H_*/
