/* PreferencesDialog.h
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

#ifndef __PreferencesDialog_H__
#define __PreferencesDialog_H__

#ifdef __GNUG__
    #pragma interface "PreferencesDialog.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ggseq_ui_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// PreferencesDialog
//----------------------------------------------------------------------------

class PreferencesDialog: public wxDialog
{
public:
    // constructors and destructors
    PreferencesDialog( wxWindow *parent, wxWindowID id, int frameSnap,
        TLColourManager* cm,
        const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    ~PreferencesDialog();

    // WDR: method declarations for PreferencesDialog
    wxGrid* GetColourGrid()  { return (wxGrid*) FindWindow( ID_COLOUR_GRID ); }
//    wxListBox* GetColourListbox()  { return (wxListBox*) FindWindow( ID_COLOUR_LISTBOX ); }
    wxButton* GetColourButton()  { return (wxButton*) FindWindow( ID_COLOUR_BUTTON ); }
    wxButton* GetColourRemoveButton()  { return (wxButton*) FindWindow( ID_COLOUR_REMOVE_BUTTON ); }
    wxGenericDirCtrl* GetColourDirtree()  { return (wxGenericDirCtrl*) FindWindow( ID_COLOUR_DIRTREE ); }
    wxButton* GetNewButton()  { return (wxButton*) FindWindow( ID_NEW_BUTTON ); }
    wxButton* GetDeleteButton()  { return (wxButton*) FindWindow( ID_DELETE_BUTTON ); }
    wxTextCtrl* GetBpmCtrl()  { return (wxTextCtrl*) FindWindow( ID_BPM_CTRL ); }
    wxTextCtrl* GetSecondsCtrl()  { return (wxTextCtrl*) FindWindow( ID_SECONDS_CTRL ); }
    wxSpinCtrl* GetFramesCtrl()  { return (wxSpinCtrl*) FindWindow( ID_FRAMES_CTRL ); }
    wxTextCtrl* GetPresetNameTextctrl()  { return (wxTextCtrl*) FindWindow( ID_PRESET_NAME_TEXTCTRL ); }
    wxListBox* GetPresetListbox()  { return (wxListBox*) FindWindow( ID_PRESET_LISTBOX ); }
    wxGrid* GetAccelsGrid()  { return (wxGrid*) FindWindow( ID_ACCELS_GRID ); }
    void LoadSnaps();
    void SaveSnaps();
    void EnableSnapKeys( bool enabled=true, bool controls=true );
    void SetSnapFrames();
    void SetSnapSeconds();
    void SetSnapBpm();
    void SetSnapListData();
    void LoadColourDlgData( wxColourData &data );
    void SaveColourDlgData( wxColourData &data );
    void SetColourButtons( int row = -1 );
    int GetFrameSnap() { return m_frameSnap; }
    void LoadColours( TLColourManager* cm );
    void SaveColours( TLColourManager* cm );
    
private:
    // WDR: member variable declarations for PreferencesDialog
    int m_frameSnap;
    
private:
    // WDR: handler declarations for PreferencesDialog
    void OnColourGridClicked( wxGridEvent &event );
    void OnColourGrid( wxGridEvent &event );
    void OnColourListbox( wxCommandEvent &event );
    void OnColourButton( wxCommandEvent &event );
    void OnColourRemoveButton( wxCommandEvent &event );
    void OnColourAddButton( wxCommandEvent &event );
    void OnNewButton( wxCommandEvent &event );
    void OnDeleteButton( wxCommandEvent &event );
    void OnPresetName( wxCommandEvent &event );
    void OnPresetList( wxCommandEvent &event );
    void OnBpm( wxCommandEvent &event );
    void OnSeconds( wxCommandEvent &event );
    void OnFrames( wxSpinEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
