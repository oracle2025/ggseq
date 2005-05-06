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

#include <wx/tglbtn.h>

#include "stuff.h"
#include "TrimmerDialog.h"
#include "WaveEditor.h"
#include "TLItem.h"
#include "PlayerInterface.h"
#include "SoundManager.h"
#include "dial.h"
#include "SampleEdit.h"

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
    EVT_TOGGLEBUTTON( ID_LOOP_BUTTON, TrimmerDialog::OnLoopButton )
    EVT_COMMAND_SCROLL( ID_TIMESTRETCH_DIAL, TrimmerDialog::OnTimestretch )
    EVT_COMMAND_SCROLL( ID_VOLUME_DIAL, TrimmerDialog::OnVolume )
    EVT_TIMER(-1,TrimmerDialog::OnTimer)
END_EVENT_TABLE()

TrimmerDialog::TrimmerDialog( wxWindow *parent,
		    float *buffer, gg_tl_dat len,
		    gg_tl_dat leftTrim,
		    gg_tl_dat rightTrim, 
		    float timestretch ) :
    wxDialog( parent, -1, wxT("Trimmer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
    m_timer       = new wxTimer(this);
    m_buffer      = buffer;
    m_len         = len;
    m_timestretch = timestretch;
    new WaveEditor( this, buffer, len, leftTrim, rightTrim, ID_WAVE_EDITOR );
    wxDial *o   = new wxDial( this, ID_TIMESTRETCH_DIAL, 50, 0, 100);
    o->NoPopup();
    
    // WDR: dialog function TrimmerDialogFunc for TrimmerDialog
    TrimmerDialogFunc2( this, TRUE ); 
    GetTimestrechTextctrl()->SetValue(
			wxString::Format( wxT("%.2f"), m_timestretch )
		);
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
	GetLoopButton()->SetValue( false );
        GetWaveEditor()->HideCaret();
        m_timer->Stop();
    } else {
        GetWaveEditor()->UpdateCaret(m_timestretch);
    }
}

void TrimmerDialog::OnVolume( wxScrollEvent &event )
{
}

void TrimmerDialog::OnTimestretch( wxScrollEvent &event )
{
	if ( GetLoopButton()->GetValue() ) {
		m_timer->Stop();
		g_ggseqProps.GetSoundManager()->Stop();
		GetLoopButton()->SetValue( false );
	}
	m_timestretch = calc( event.GetPosition() );
	GetTimestrechTextctrl()->SetValue(
			wxString::Format( wxT("%.2f"), m_timestretch )
		);
}

void TrimmerDialog::OnLoopButton( wxCommandEvent &event )
{
	WaveEditor *we = GetWaveEditor();
	if ( !GetLoopButton()->GetValue()/*m_playing*/ ) {
		g_ggseqProps.GetSoundManager()->Stop();
		m_timer->Stop();
		we->HideCaret();
	} else {
		SampleEdit pSampleEdit( m_buffer, m_len );
		pSampleEdit.SetTempo( m_timestretch );
		pSampleEdit.SetTrims( we->m_leftTrim, we->m_rightTrim );
		int stretchedLen;
		float *stretchedBuffer = pSampleEdit.Convert( stretchedLen );
		g_ggseqProps.GetSoundManager()->Play(
			new LoopPlayer( stretchedBuffer,
				stretchedLen )
			);
		m_timer->Start(100);
		we->ShowCaret();
	}
}



gg_tl_dat TrimmerDialog::GetLeftTrim()
{
	return GetWaveEditor()->m_leftTrim;
}
gg_tl_dat TrimmerDialog::GetRightTrim()
{
	return GetWaveEditor()->m_rightTrim;
}
float TrimmerDialog::GetTimestretch()
{
	return m_timestretch;
}

