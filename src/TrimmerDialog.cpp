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

#include "TrimmerDialog.h"
#include "WaveEditor.h"
#include "stuff.h"
#include "TLItem.h"
#include "PlayerInterface.h"
#include "SoundManager.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// TrimmerDialog
//----------------------------------------------------------------------------

// WDR: event table for TrimmerDialog

BEGIN_EVENT_TABLE(TrimmerDialog,wxDialog)
    EVT_BUTTON( ID_LOOP_BUTTON, TrimmerDialog::OnLoopButton )
END_EVENT_TABLE()

TrimmerDialog::TrimmerDialog( wxWindow *parent, TLItem *item /*, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style*/ ) :
    wxDialog( parent, -1, wxT("Trimmer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
    m_item = item;
    m_playing = 0;
    /*wxWindow *w = */new WaveEditor( this, item, ID_WAVE_EDITOR );
    // WDR: dialog function TrimmerDialogFunc for TrimmerDialog
    TrimmerDialogFunc( this, TRUE ); 
}

TrimmerDialog::~TrimmerDialog()
{
	g_ggseqProps.GetSoundManager()->Stop();
}

// WDR: handler implementations for TrimmerDialog

void TrimmerDialog::OnLoopButton( wxCommandEvent &event )
{
	double amount;
	if ( !GetTimestrechTextctrl()->GetValue().ToDouble( &amount ) )
		return;
	m_item->Stretch( amount );
	if ( m_playing ) {
		g_ggseqProps.GetSoundManager()->Stop();
	} else {
//		wxLogError(wxT("playsnd"));
/*		g_ggseqProps.GetSoundManager()->Loop(
				m_item->m_stretchedBuffer,
				m_item->m_stretchedLen );*/
		g_ggseqProps.GetSoundManager()->Play(
				new LoopPlayer( m_item->m_stretchedBuffer,
					m_item->m_stretchedLen )
				);
	}
	m_playing = !m_playing;
}




