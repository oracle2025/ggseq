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

#include "SoundManager.h"
#include "MiniPlayer.h"

#include "play_12.xpm"
#include "stop_12.xpm"
enum
{
	ID_Play = 1,
	ID_Stop,
};

BEGIN_EVENT_TABLE(MiniPlayer, wxPanel)
	EVT_BUTTON(ID_Stop,MiniPlayer::OnStop)
	EVT_BUTTON(ID_Play,MiniPlayer::OnPlay)
	EVT_TIMER(-1,MiniPlayer::OnTimer)
END_EVENT_TABLE()

MiniPlayer::MiniPlayer(wxWindow* parent, SoundManager *soundManager,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxString& name)
		:wxPanel(parent, id, pos, size, style , name)
{
	MakeMiniPlayerWindow(this);
	m_soundManager=soundManager;
	m_timer = new wxTimer(this);
}
void MiniPlayer::MakeMiniPlayerWindow(wxWindow *parent)
{
	wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("") );
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
}
void MiniPlayer::Play()
{
	m_stopButton->Enable(true);
	m_playButton->Enable(false);
	wxLogStatus(wxT("Playing Sample: %s"),m_filename.c_str());
	long length;
	m_soundManager->Play(m_filename,length);
	m_slider->SetRange(0,length);
	m_slider->SetValue(0);
	m_timer->Start(100);
	
}
void MiniPlayer::OnTimer(wxTimerEvent &event)
{
	if (m_soundManager->Done()) {
		Stop();
	} else {
		m_slider->SetValue(m_soundManager->GetPosition());
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
}

