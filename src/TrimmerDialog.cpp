/* TrimmerDialog.cpp
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

#ifdef __GNUG__
    #pragma implementation "TrimmerDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "stuff.h"
#include "TrimmerDialog.h"
#include "WaveEditor.h"
#include "TLItem.h"
#include "PlayerInterface.h"
#include "SoundManager.h"
#include "dial.h"

float calc( int x )
{
    return 0.5 + float(x) * 0.01;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// TrimmerDialog
//----------------------------------------------------------------------------

// WDR: event table for TrimmerDialog

BEGIN_EVENT_TABLE(TrimmerDialog,wxDialog)
    EVT_BUTTON( ID_LOOP_BUTTON, TrimmerDialog::OnLoopButton )
    EVT_COMMAND_SCROLL( ID_TIMESTRETCH_DIAL, TrimmerDialog::OnTimestretch )
    EVT_COMMAND_SCROLL( ID_VOLUME_DIAL, TrimmerDialog::OnVolume )
    EVT_TIMER(-1,TrimmerDialog::OnTimer)
END_EVENT_TABLE()

TrimmerDialog::TrimmerDialog( wxWindow *parent, TLItem *item /*, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style*/ ) :
    wxDialog( parent, -1, wxT("Trimmer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
    m_timer = new wxTimer(this);
    m_item = item;
    m_playing = 0;
    /*wxWindow *w = */new WaveEditor( this, item, ID_WAVE_EDITOR );
    new wxDial( this, ID_TIMESTRETCH_DIAL, 50, 0, 100); // 0 -> 100 : 0.5 -> 1.5
    new wxDial( this, ID_VOLUME_DIAL, 0, -50, 50);
    
    // WDR: dialog function TrimmerDialogFunc for TrimmerDialog
    TrimmerDialogFunc( this, TRUE ); 
}

TrimmerDialog::~TrimmerDialog()
{
    g_ggseqProps.GetSoundManager()->Stop();
    m_timer->Stop();
}

// WDR: handler implementations for TrimmerDialog
void TrimmerDialog::OnTimer(wxTimerEvent &event)
{
	if (g_ggseqProps.GetSoundManager()->Done()) {
		g_ggseqProps.GetSoundManager()->Stop();
		m_playing = false;
		GetWaveEditor()->HideCaret();
		m_timer->Stop();
	} else {
		//m_slider->SetValue(m_soundManager->GetPosition());
		//SetPosition(g_ggseqProps.GetSoundManager()->GetPosition());
		GetWaveEditor()->UpdateCaret();
	}
}

void TrimmerDialog::OnVolume( wxScrollEvent &event )
{
}

void TrimmerDialog::OnTimestretch( wxScrollEvent &event )
{
	if ( m_playing ) {
		m_timer->Stop();
		g_ggseqProps.GetSoundManager()->Stop();
		m_playing = false;
	}
    GetTimestrechTextctrl()->SetValue(
        wxString::Format( wxT("%.2f"), calc( event.GetPosition() ) )
        );
}

void TrimmerDialog::OnLoopButton( wxCommandEvent &event )
{
    double amount;
    if ( !GetTimestrechTextctrl()->GetValue().ToDouble( &amount ) )
        return;
    WaveEditor *we = GetWaveEditor();
    m_item->Stretch( amount , we->m_leftTrim, we->m_rightTrim );
    if ( m_playing ) {
        g_ggseqProps.GetSoundManager()->Stop();
	m_timer->Stop();
	we->HideCaret();
    } else {
//      wxLogError(wxT("playsnd"));
/*      g_ggseqProps.GetSoundManager()->Loop(
                m_item->m_stretchedBuffer,
                m_item->m_stretchedLen );*/
	
        g_ggseqProps.GetSoundManager()->Play(
                new LoopPlayer( m_item->m_stretchedBuffer,
                    m_item->m_stretchedLen )
                );
	m_timer->Start(100);
	we->ShowCaret();
    }
    m_playing = !m_playing;
}




