/* MiniPlayer.h
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

#ifndef _MINIPLAYER_H_
#define _MINIPLAYER_H_

class TLSample;
class UpdateListener;
class FileInfoListener;

class MiniPlayerInterface
{
	public:
		virtual void SetFilename(wxString filename)=0;
		virtual void SetSample(TLSample *sample)=0;
		virtual void Play()=0;
		virtual void Stop()=0;
};

class MiniPlayer : public wxPanel, public MiniPlayerInterface
{
	public:
		MiniPlayer(wxWindow* parent, SoundManager *soundManager,
				UpdateListener *updateListener=NULL, FileInfoListener *fiListener=NULL);
		~MiniPlayer();
		void MakeMiniPlayerWindow(wxWindow *parent);
		void SetFilename(wxString filename);
		void SetSample(TLSample *sample);
		void Play();
		void Stop();
		void OnPlay(wxCommandEvent *event);
		void OnStop(wxCommandEvent *event);
		void OnTimer(wxTimerEvent &event);
	private:
		void SetPosition(gg_tl_dat position);
		wxButton *m_playButton;
		wxButton *m_stopButton;
		wxSlider *m_slider;
		wxTimer *m_timer;
		wxString m_filename;
		TLSample *m_sample;
		gg_tl_dat m_length;
		SoundManager *m_soundManager;
		UpdateListener *m_updateListener;
		FileInfoListener *m_fileInfoListener;
		DECLARE_EVENT_TABLE()

};
#endif /* _MINIPLAYER_H_ */
