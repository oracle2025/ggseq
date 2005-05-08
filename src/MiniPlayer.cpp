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
#include "ggEvtHandler.h"
#include "FileInfoPanel.h"
#include "PlayerInterface.h"
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

MiniPlayer::MiniPlayer(wxWindow* parent, wxWindowID id, UpdateListener *updateListener/*, FileInfoListener *fiListener*/)
		:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN,wxT("MiniPlayer"))
{
	MakeMiniPlayerWindow(this);
	//m_soundManager=soundManager;
	m_timer = new wxTimer(this);
	m_updateListener = updateListener;
	//m_fileInfoListener = fiListener;
	m_sample=NULL;
	m_length=100;
	g_ggseqProps.SetMiniPlayer(this);
}
MiniPlayer::~MiniPlayer()
{
//	delete m_noBgHandler;
#ifdef __WXMSW__
	FindWindow(wxT("item3"))->PopEventHandler(true);
	FindWindow(ID_Play)->PopEventHandler(true);
	FindWindow(ID_Stop)->PopEventHandler(true);
#endif
}
	
void MiniPlayer::MakeMiniPlayerWindow(wxWindow *parent)
{
	wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

#ifdef __WXMSW__
	wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
#else
	wxStaticBox *item2 = new wxStaticBox( parent, ID_StaticBox, wxT("") ,wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE);
	wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxHORIZONTAL );
#endif
	wxStaticText *item3 = new wxStaticText( parent, -1, wxT("Miniplayer"), wxDefaultPosition, wxDefaultSize, 0,wxT("item3") );
#ifdef __WXMSW__
	item3->PushEventHandler(new NoBgEvtHandler());
#endif
	item1->Add( item3, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );

	m_playButton = new wxBitmapButton( parent,ID_Play , wxBitmap(play_12_xpm), wxDefaultPosition, wxSize(20,20) );
	item1->Add( m_playButton, 0, wxALIGN_CENTRE|wxRIGHT, 5 );

	m_stopButton = new wxBitmapButton( parent, ID_Stop, wxBitmap(stop_12_xpm), wxDefaultPosition, wxSize(20,20) );
#ifdef __WXMSW__
	m_playButton->PushEventHandler(new NoBgEvtHandler());
	m_stopButton->PushEventHandler(new NoBgEvtHandler());
#endif
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
	Stop();
	m_stopButton->Enable(true);
	m_playButton->Enable(false);
	if (m_sample) {
		wxLogStatus(wxT("Playing Sample: %s"),m_sample->GetFilename().c_str());
		if (g_ggseqProps.GetFileInfoListener()) {g_ggseqProps.GetFileInfoListener()->SetInfo(m_sample->GetFilename(),m_sample->m_infoFrames,m_sample->m_infoChannels,m_sample->m_infoSampleRate);}
//		m_slider->SetRange(0,m_sample->GetLength());
		m_length=m_sample->GetLength();
		g_ggseqProps.GetSoundManager()->Play( new SamplePlayer( m_sample ) );
	} else {
		if (m_filename.IsEmpty()) {
			return;
		}
		wxLogStatus(wxT("Playing Sample: %s"),m_filename.c_str());
		long length;
		long frames;
		long channels;
		long sampleRate;
		g_ggseqProps.GetSoundManager()->Play(
				new SamplePlayer( m_filename, length,
					frames, channels, sampleRate, 
					m_updateListener )
				);
		if (g_ggseqProps.GetFileInfoListener()) {g_ggseqProps.GetFileInfoListener()->SetInfo(m_filename,frames,channels,sampleRate);}
//		std::cout << "length: " << length << std::endl;
//		m_slider->SetRange(0,length);
		m_length=length;
	}
	m_slider->SetValue(0);
	m_timer->Start(100);
}
void MiniPlayer::OnTimer(wxTimerEvent &event)
{
	if (g_ggseqProps.GetSoundManager()->Done()) {
		Stop();
	} else {
		//m_slider->SetValue(m_soundManager->GetPosition());
		SetPosition(g_ggseqProps.GetSoundManager()->GetPosition());
	}
}

void MiniPlayer::OnPlay(wxCommandEvent &event)
{
	if (m_filename!=wxT(""))
		Play();
}
void MiniPlayer::OnStop(wxCommandEvent &event)
{
	Stop();
}
void MiniPlayer::Stop()
{
	m_stopButton->Enable(false);
	m_playButton->Enable(true);
	m_timer->Stop();
	m_slider->SetValue(0);
	g_ggseqProps.GetSoundManager()->Stop();
	m_sample=NULL;
}
void MiniPlayer::SetPosition(gg_tl_dat position)
{
	m_slider->SetValue((position*100)/m_length);
}
