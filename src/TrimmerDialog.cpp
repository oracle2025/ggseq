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


// WDR: class implementations

//----------------------------------------------------------------------------
// TrimmerDialog
//----------------------------------------------------------------------------

// WDR: event table for TrimmerDialog

BEGIN_EVENT_TABLE(TrimmerDialog,wxDialog)
END_EVENT_TABLE()

TrimmerDialog::TrimmerDialog( wxWindow *parent, TLItem *item /*, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style*/ ) :
    wxDialog( parent, -1, wxT("Trimmer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
    m_item = item;
    wxWindow *w = new WaveEditor( this, item, ID_WAVE_EDITOR );
    // WDR: dialog function TrimmerDialogFunc for TrimmerDialog
    TrimmerDialogFunc( this, TRUE ); 
}

TrimmerDialog::~TrimmerDialog()
{
}

// WDR: handler implementations for TrimmerDialog




