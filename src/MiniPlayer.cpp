/* MiniPlayer.cpp
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
#include "SoundManager.h"
#include "MiniPlayer.h"
#include "TLSample.h"
#include "FileInfoPanel.h"
#include <iostream>

#include "play_12.xpm"
#include "stop_12.xpm"
enum
{
	ID_Play = 1,
	ID_Stop,
	ID_StaticBox,
};

BEGIN_EVENT_TABLE(MiniPlayer, wxPanel)
	EVT_BUTTON(ID_Stop,MiniPlayer::OnStop)
	EVT_BUTTON(ID_Play,MiniPlayer::OnPlay)
	EVT_TIMER(-1,MiniPlayer::OnTimer)
END_EVENT_TABLE()

MiniPlayer::MiniPlayer(wxWindow* parent, SoundManager *soundManager, UpdateListener *updateListener, FileInfoListener *fiListener,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxString& name)
		:wxPanel(parent, id, pos, size, style|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN , name)
{
	MakeMiniPlayerWindow(this);
	m_soundManager=soundManager;
	m_timer = new wxTimer(this);
	m_updateListener = updateListener;
	m_fileInfoListener = fiListener;
	m_sample=NULL;
	m_length=100;
}

/*class tmpEventHandler : public wxEvtHandler
{
	public:
		tmpEventHandler() :wxEvtHandler(){}
		void OnEraseBackground(wxPaintEvent& event){}
	private:
		DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(tmpEventHandler, wxEvtHandler)
	EVT_ERASE_BACKGROUND(tmpEventHandler::OnEraseBackground)
	EVT_CUSTOM(wxEVT_PAINT,ID_StaticBox,tmpEventHandler::OnEraseBackground)
END_EVENT_TABLE()*/
void MiniPlayer::MakeMiniPlayerWindow(wxWindow *parent)
{
	wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox *item2 = new wxStaticBox( parent, ID_StaticBox, wxT("") ,wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE);
//	item2->PushEventHandler(new tmpEventHandler());
	
	wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxHORIZONTAL );
	wxStaticText *item3 = new wxStaticText( parent, -1, wxT("Miniplayer"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( item3, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );

	m_playButton = new wxBitmapButton( parent,ID_Play , wxBitmap(play_12_xpm), wxDefaultPosition, wxSize(20,20) );
	item1->Add( m_playButton, 0, wxALIGN_CENTRE|wxRIGHT, 5 );

	m_stopButton = new wxBitmapButton( parent, ID_Stop, wxBitmap(stop_12_xpm), wxDefaultPosition, wxSize(20,20) );
	item1->Add( m_stopButton, 0, wxALIGN_CENTRE|wxRIGHT, 5 );
	m_stopButton->Enable(false);

	m_slider = new wxSlider( parent, -1,0,0, 100, wxDefaultPosition, wxSize(100,-1), 0 );
	m_slider->SetValue( 0 );
	m_slider->Enable(false);
	item1->Add( m_slider, 1, wxALIGN_CENTRE|wxRIGHT, 5 );

	item0->Add( item1, 0, wxEXPAND );

	parent->SetAutoLayout( TRUE );
	parent->SetSizer( item0 );
	item0->SetSizeHints( parent );

}
void MiniPlayer::SetFilename(wxString filename)
{
	m_filename = filename;
	m_sample=NULL;
}
void MiniPlayer::SetSample(TLSample *sample)
{
	m_sample = sample;
}
void MiniPlayer::Play()
{
	TLSample *sample = m_sample;
	Stop();
	m_sample=sample;
	m_stopButton->Enable(true);
	m_playButton->Enable(false);
	if (m_sample) {
		wxLogStatus(wxT("Playing Sample: %s"),m_sample->GetFilename().c_str());
		if (m_fileInfoListener) {m_fileInfoListener->SetInfo(m_sample->GetFilename(),m_sample->m_infoFrames,m_sample->m_infoChannels,m_sample->m_infoSampleRate);}
//		m_slider->SetRange(0,m_sample->GetLength());
		m_length=m_sample->GetLength();
		m_soundManager->Play(m_sample);
	} else {
		wxLogStatus(wxT("Playing Sample: %s"),m_filename.c_str());
		long length;
		long frames;
		long channels;
		long sampleRate;
		m_soundManager->Play(m_filename,length,frames,channels,sampleRate,m_updateListener);
		if (m_fileInfoListener) {m_fileInfoListener->SetInfo(m_filename,frames,channels,sampleRate);}
//		std::cout << "length: " << length << std::endl;
//		m_slider->SetRange(0,length);
		m_length=length;
	}
	m_slider->SetValue(0);
	m_timer->Start(100);
}
void MiniPlayer::OnTimer(wxTimerEvent &event)
{
	if (m_soundManager->Done()) {
		Stop();
	} else {
		//m_slider->SetValue(m_soundManager->GetPosition());
		SetPosition(m_soundManager->GetPosition());
	}
}
void MiniPlayer::OnPlay(wxCommandEvent *event)
{
	if (m_filename!=wxT(""))
		Play();
}
void MiniPlayer::OnStop(wxCommandEvent *event)
{
	Stop();
}
void MiniPlayer::Stop()
{
	m_stopButton->Enable(false);
	m_playButton->Enable(true);
	m_timer->Stop();
	m_slider->SetValue(0);
	m_soundManager->Stop();
	m_sample=NULL;
}
void MiniPlayer::SetPosition(gg_tl_dat position)
{
	m_slider->SetValue((position*100)/m_length);
}
