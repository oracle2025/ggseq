/* TrimmerDialog.h
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

#ifndef __TrimmerDialog_H__
#define __TrimmerDialog_H__

#ifdef __GNUG__
    #pragma interface "TrimmerDialog.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ggseq_ui_wdr.h"

// WDR: class declarations
class TLItem;

//----------------------------------------------------------------------------
// TrimmerDialog
//----------------------------------------------------------------------------

class TrimmerDialog: public wxDialog
{
public:
    // constructors and destructors
    TrimmerDialog( wxWindow *parent, TLItem *item /*, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE */);
    virtual ~TrimmerDialog();
    
    // WDR: method declarations for TrimmerDialog
    wxTextCtrl* GetTimestrechTextctrl()  { return (wxTextCtrl*) FindWindow( ID_TIMESTRECH_TEXTCTRL ); }
    
private:
    // WDR: member variable declarations for TrimmerDialog
    TLItem *m_item;
    bool m_playing;
    
private:
    // WDR: handler declarations for TrimmerDialog
    void OnLoopButton( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
