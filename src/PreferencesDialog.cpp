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

#ifdef __GNUG__
    #pragma implementation "PreferencesDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/colordlg.h>
#include <wx/dirctrl.h>
#include <wx/config.h>

// Include private headers
#include "TLColourManager.h"
#include "PreferencesDialog.h"


// Defines
#define SAMPLE_RATE 44100
#define HIP_HOP 117600
#define OLD_SKOOL 110250
#define FUNK 94500
#define UNNAMED wxT("Unnamed")
#define DISABLE_UPDATE PushEventHandler ( new NoSnapChangeHandler() );
#define ENABLE_UPDATE PopEventHandler( true );

// - NoChangeEvtHandler -
class NoSnapChangeHandler : public wxEvtHandler
{
    public:
        NoSnapChangeHandler() :wxEvtHandler(){}
        void OnEvent(wxCommandEvent &event){}
        void OnSpinEvent(wxSpinEvent &event){}
    private:
        DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(NoSnapChangeHandler, wxEvtHandler)
    EVT_TEXT(ID_FRAMES_CTRL, NoSnapChangeHandler::OnEvent)
    EVT_SPINCTRL(ID_FRAMES_CTRL, NoSnapChangeHandler::OnSpinEvent)
    EVT_TEXT(ID_SECONDS_CTRL, NoSnapChangeHandler::OnEvent)
    EVT_TEXT(ID_BPM_CTRL, NoSnapChangeHandler::OnEvent)
END_EVENT_TABLE()


// WDR: class implementations

//----------------------------------------------------------------------------
// PreferencesDialog
//----------------------------------------------------------------------------

// WDR: event table for PreferencesDialog

BEGIN_EVENT_TABLE(PreferencesDialog,wxDialog)
    EVT_SPINCTRL( ID_FRAMES_CTRL, PreferencesDialog::OnFrames )
    EVT_TEXT( ID_SECONDS_CTRL, PreferencesDialog::OnSeconds )
    EVT_TEXT( ID_BPM_CTRL, PreferencesDialog::OnBpm )
    EVT_LISTBOX( ID_PRESET_LISTBOX, PreferencesDialog::OnPresetList )
    EVT_TEXT( ID_PRESET_NAME_TEXTCTRL, PreferencesDialog::OnPresetName )
    EVT_BUTTON( ID_DELETE_BUTTON, PreferencesDialog::OnDeleteButton )
    EVT_BUTTON( ID_NEW_BUTTON, PreferencesDialog::OnNewButton )
    EVT_BUTTON( ID_COLOUR_ADD_BUTTON, PreferencesDialog::OnColourAddButton )
    EVT_BUTTON( ID_COLOUR_REMOVE_BUTTON, PreferencesDialog::OnColourRemoveButton )
    EVT_BUTTON( ID_COLOUR_BUTTON, PreferencesDialog::OnColourButton )
//    EVT_LISTBOX( ID_COLOUR_LISTBOX, PreferencesDialog::OnColourListbox )
    EVT_GRID_CELL_LEFT_CLICK( PreferencesDialog::OnColourGridClicked )
    EVT_GRID_SELECT_CELL( PreferencesDialog::OnColourGrid )
END_EVENT_TABLE()

PreferencesDialog::PreferencesDialog( wxWindow *parent, wxWindowID id, int frameSnap, TLColourManager* cm, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style|wxRESIZE_BORDER  )
{
    // WDR: dialog function PreferencesFunc for PreferencesDialog
    PreferencesFunc( this, TRUE ); 

    // Setting up GUI-Stuff
    // - Keyboard Shortcuts
    wxGrid *acGrid = GetAccelsGrid();
    acGrid->SetCellValue(0,0,wxT("Test"));
    acGrid->SetColLabelSize(0);
    acGrid->SetRowLabelSize(0);
    acGrid->EnableGridLines(false);
    acGrid->DisableDragGridSize();
    acGrid->EnableEditing(false);
    
    // - Snap
    int *len=new int;
    *len = frameSnap;
    m_frameSnap = frameSnap;
    DISABLE_UPDATE
    SetSnapFrames();
    SetSnapSeconds();
    SetSnapBpm();
    ENABLE_UPDATE
    GetPresetListbox()->Append( wxT("Custom ..."), (void*)len );
    GetPresetListbox()->SetSelection( 0 );
    EnableSnapKeys( false );
    
    LoadSnaps();
    // - Colours
    LoadColours( cm );
    wxGrid *colGrid = GetColourGrid();
    //colGrid->SetCellValue(0,0,wxT("Test"));
    colGrid->SetColLabelSize(0);
    colGrid->SetRowLabelSize(0);
    colGrid->EnableGridLines(false);
    colGrid->DisableDragGridSize();
    colGrid->EnableEditing(false);
    colGrid->AutoSizeColumns();
    SetColourButtons();
}
PreferencesDialog::~PreferencesDialog()
{
    SaveSnaps();
    for (int i=0;i<GetPresetListbox()->GetCount();i++) {
        int *t=(int*)GetPresetListbox()->GetClientData(i);
        delete t;
    }
/*    for (int i=GetColourListbox()->GetCount()-1; i>=0;i--) {
        wxColour *colour = (wxColour*)GetColourListbox()->GetClientData(i);
        delete colour;  
    }*/

}
void PreferencesDialog::LoadSnaps()
{
    wxConfigBase *conf=wxConfigBase::Get();
    if (!conf->Exists(wxT("/Snaps"))) {
        int *len=new int;
        *len=FUNK;
        GetPresetListbox()->Append(wxT("Funk"), (void*)len);
        len=new int;
        *len=HIP_HOP;
        GetPresetListbox()->Append(wxT("HipHop"), (void*)len);
        len=new int;
        *len=OLD_SKOOL;
        GetPresetListbox()->Append(wxT("OldSkool"), (void*)len);
        return;
    }
    conf->SetPath(wxT("/Snaps"));
    wxArrayString aNames;
    wxString str;
    long dummy;
    bool bCont = conf->GetFirstEntry(str, dummy);
    while ( bCont ) {
        aNames.Add(str);
        bCont = conf->GetNextEntry(str, dummy);
    }
    for (unsigned int i=0;i<aNames.Count();i++) {
        wxString str1=aNames.Item(i);
        int *len=new int;
        *len=conf->Read(str1,-1);
            GetPresetListbox()->Append(str1, (void*)len);
    }


}
void PreferencesDialog::SaveSnaps()
{
    wxConfigBase *conf=wxConfigBase::Get();
    if (conf->Exists(wxT("/Snaps"))) {
        conf->DeleteGroup(wxT("/Snaps"));
    }
    conf->SetPath(wxT("/Snaps"));
    for (int i=1;i<GetPresetListbox()->GetCount();i++) {
        int t=*(int*)GetPresetListbox()->GetClientData(i);
        conf->Write(GetPresetListbox()->GetString(i), t);
    }
    conf->Flush();

}
void PreferencesDialog::EnableSnapKeys( bool enabled, bool controls )
{
    GetPresetNameTextctrl()->Enable( enabled );
    GetDeleteButton()->Enable( enabled );
    if ( !enabled && controls ) {
        GetPresetNameTextctrl()->SetValue( wxT("Custom ...") );
    }
    GetBpmCtrl()->Enable( controls );
    GetSecondsCtrl()->Enable( controls );
    GetFramesCtrl()->Enable( controls );
}
void PreferencesDialog::SetSnapFrames()
{
    GetFramesCtrl()->SetValue( m_frameSnap );
}
void PreferencesDialog::SetSnapSeconds()
{
    GetSecondsCtrl()->Clear();
    *GetSecondsCtrl() << ( (double) m_frameSnap ) / SAMPLE_RATE;
}
void PreferencesDialog::SetSnapBpm()
{
    GetBpmCtrl()->Clear();
    *GetBpmCtrl() << 60. / ( ( (double) m_frameSnap ) / SAMPLE_RATE );
}
void PreferencesDialog::SetSnapListData()
{
    wxListBox *lb = GetPresetListbox();
    int index = lb->GetSelection();
    if ( index >= 0 ) {
        int* t = (int*) lb->GetClientData( index );
        *t = m_frameSnap;
    }
}
long GetColourValue(const wxColour &colour)
{
    long var = colour.Blue();
    var=var*256;
    var +=colour.Green();
    var=var*256;
    var +=colour.Red();
    return var;
}
void PreferencesDialog::LoadColourDlgData( wxColourData &data )
{
    wxConfigBase *conf=wxConfigBase::Get();
    conf->SetPath(wxT("/ColourDialog"));
    data.SetCustomColour(0,wxColour(conf->Read(wxT("colour0"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(1,wxColour(conf->Read(wxT("colour1"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(2,wxColour(conf->Read(wxT("colour2"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(3,wxColour(conf->Read(wxT("colour3"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(4,wxColour(conf->Read(wxT("colour4"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(5,wxColour(conf->Read(wxT("colour5"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(6,wxColour(conf->Read(wxT("colour6"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(7,wxColour(conf->Read(wxT("colour7"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(8,wxColour(conf->Read(wxT("colour8"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(9,wxColour(conf->Read(wxT("colour9"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(10,wxColour(conf->Read(wxT("colour10"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(11,wxColour(conf->Read(wxT("colour11"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(12,wxColour(conf->Read(wxT("colour12"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(13,wxColour(conf->Read(wxT("colour13"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(14,wxColour(conf->Read(wxT("colour14"),GetColourValue(*wxWHITE))));
    data.SetCustomColour(15,wxColour(conf->Read(wxT("colour15"),GetColourValue(*wxWHITE))));
}
void PreferencesDialog::SaveColourDlgData( wxColourData &data )
{
    wxConfigBase *conf=wxConfigBase::Get();
    conf->SetPath(wxT("/ColourDialog"));
    conf->Write(wxT("colour0"),(long)GetColourValue(data.GetCustomColour(0)));
    conf->Write(wxT("colour1"),(long)GetColourValue(data.GetCustomColour(1)));
    conf->Write(wxT("colour2"),(long)GetColourValue(data.GetCustomColour(2)));
    conf->Write(wxT("colour3"),(long)GetColourValue(data.GetCustomColour(3)));
    conf->Write(wxT("colour4"),(long)GetColourValue(data.GetCustomColour(4)));
    conf->Write(wxT("colour5"),(long)GetColourValue(data.GetCustomColour(5)));
    conf->Write(wxT("colour6"),(long)GetColourValue(data.GetCustomColour(6)));
    conf->Write(wxT("colour7"),(long)GetColourValue(data.GetCustomColour(7)));
    conf->Write(wxT("colour8"),(long)GetColourValue(data.GetCustomColour(8)));
    conf->Write(wxT("colour9"),(long)GetColourValue(data.GetCustomColour(9)));
    conf->Write(wxT("colour10"),(long)GetColourValue(data.GetCustomColour(10)));
    conf->Write(wxT("colour11"),(long)GetColourValue(data.GetCustomColour(11)));
    conf->Write(wxT("colour12"),(long)GetColourValue(data.GetCustomColour(12)));
    conf->Write(wxT("colour13"),(long)GetColourValue(data.GetCustomColour(13)));
    conf->Write(wxT("colour14"),(long)GetColourValue(data.GetCustomColour(14)));
    conf->Write(wxT("colour15"),(long)GetColourValue(data.GetCustomColour(15)));
}
void PreferencesDialog::SetColourButtons( int row )
{
#if 0
    wxListBox *lb = GetColourListbox();
    if ( lb->GetSelection() < 0 ) {
        GetColourButton()->Enable(false);
        GetColourRemoveButton()->Enable(false);
    } else {
        GetColourButton()->Enable();
        GetColourRemoveButton()->Enable();
        GetColourButton()->SetBackgroundColour( *(wxColour*)lb->GetClientData( lb->GetSelection() ) );
    }
#endif
	wxGrid *gr = GetColourGrid();
	if ( row < 0 ) {
		GetColourButton()->Enable(false);
		GetColourRemoveButton()->Enable(false);
	} else {
		GetColourButton()->Enable();
		GetColourRemoveButton()->Enable();
		GetColourButton()->SetBackgroundColour( gr->GetCellBackgroundColour(row,0) );
	}
    

}
void PreferencesDialog::LoadColours( TLColourManager* cm )
{
    //wxListBox *lb = GetColourListbox();
    wxGrid *gr = GetColourGrid();
    for ( TLDirColourList::Node *node = cm->GetColours(); node; node = node->GetNext() ) {
        TLDirColour *current = node->GetData();
        /*wxColour *colour = new wxColour(current->m_colour);
        lb->Append(current->m_directory,(void*)colour);*/
	gr->AppendRows();
	gr->SetCellValue( current->m_directory, gr->GetRows()-1, 0 );
	gr->SetCellBackgroundColour( gr->GetRows()-1, 0, current->m_colour );
    }

}
void PreferencesDialog::SaveColours( TLColourManager* cm )
{
    //wxListBox *lb = GetColourListbox();
    wxGrid *gr = GetColourGrid();
    cm->Clear();
    for (int i=gr->GetRows()-1; i>=0;i--) {
        cm->SetColour(gr->GetCellValue(i,0),gr->GetCellBackgroundColour(i,0));
    }

}
// WDR: handler implementations for PreferencesDialog

void PreferencesDialog::OnColourGridClicked( wxGridEvent &event )
{
	if ( !event.Selecting() ) {
		SetColourButtons();
	}
	event.Skip();
}
void PreferencesDialog::OnColourGrid( wxGridEvent &event )
{
	if ( event.Selecting() ) {
		SetColourButtons( event.GetRow() );
	} else {
		SetColourButtons();
	}
	event.Skip();
}

void PreferencesDialog::OnColourListbox( wxCommandEvent &event )
{
    //SetColourButtons();
}

void PreferencesDialog::OnColourButton( wxCommandEvent &event )
{
    wxGrid *gr = GetColourGrid();
    if ( !gr->IsSelection() /*lb->GetSelection() < 0*/ )
        return;
    //wxListBox *lb = GetColourListbox();
    wxColourData colData;
    LoadColourDlgData( colData );
    wxColourDialog dialog( this, &colData );
    if (dialog.ShowModal() == wxID_OK) {
        wxColour col = dialog.GetColourData().GetColour();
        // wxColour *col2 = (wxColour*) lb->GetClientData( lb->GetSelection() );
        // *col2 = col;
        gr->SetCellBackgroundColour(gr->GetGridCursorRow(),0,col);
        GetColourButton()->SetBackgroundColour(col);
        
        colData = dialog.GetColourData();
        SaveColourDlgData( colData );
    }
}

void PreferencesDialog::OnColourRemoveButton( wxCommandEvent &event )
{
    wxGrid *gr = GetColourGrid();
    if ( !gr->IsSelection() )
        return;
    gr->DeleteRows( gr->GetGridCursorRow() );
    SetColourButtons();

#if 0
    wxListBox *lb = GetColourListbox();
    int sel = lb->GetSelection();
    if ( sel >= 0 ) {
        wxColour *colour = (wxColour*) lb->GetClientData( sel );
        delete colour;
        lb->Delete( sel );
        SetColourButtons();
    }
#endif
}

void PreferencesDialog::OnColourAddButton( wxCommandEvent &event )
{
    wxGrid *gr = GetColourGrid();
    //wxListBox *lb = GetColourListbox();
    wxGenericDirCtrl *dt = GetColourDirtree();
    int len = gr->GetRows();
    for ( int i = 0; i < len; i++ ) {
        if ( dt->GetPath() == gr->GetCellValue(i,0) )
            return;
    }
    /*wxColour *colour = new wxColour( *wxRED );
    lb->Append( dt->GetPath(), (void*) colour );
    lb->SetSelection( lb->GetCount() - 1 );*/

    gr->AppendRows();
    gr->SetCellValue( dt->GetPath(), gr->GetRows()-1, 0 );
    gr->SetCellBackgroundColour( gr->GetRows()-1, 0, *wxRED );
    gr->SelectRow( gr->GetRows()-1 );
    gr->SetGridCursor( gr->GetRows()-1, 0 );
    SetColourButtons(gr->GetRows()-1);

}

void PreferencesDialog::OnNewButton( wxCommandEvent &event )
{
    int *t = new int;
    *t = m_frameSnap;
    wxListBox *lb = GetPresetListbox();
    lb->Append( UNNAMED, (void*)t );
    lb->SetSelection( lb->GetCount()-1 );
    GetPresetNameTextctrl()->SetValue( UNNAMED );
    EnableSnapKeys();
}

void PreferencesDialog::OnDeleteButton( wxCommandEvent &event )
{
    wxListBox *lb = GetPresetListbox();
    int index = lb->GetSelection();
    if ( index > 0 ) {
        int *t = (int*) lb->GetClientData( index );
        delete t;
        lb->Delete( index );
    }
    lb->SetSelection(0);
    EnableSnapKeys( false );
}

void PreferencesDialog::OnPresetName( wxCommandEvent &event )
{
    int index = GetPresetListbox()->GetSelection();
    if ( index > 0 ) {
        GetPresetListbox()->SetString( index, event.GetString() );
    }
}

void PreferencesDialog::OnPresetList( wxCommandEvent &event )
{
    wxListBox *lb = GetPresetListbox();
    GetPresetNameTextctrl()->SetValue( event.GetString() );
    EnableSnapKeys( event.GetInt() > 0 );
    int *t = (int*) lb->GetClientData( lb->GetSelection() );
    m_frameSnap = *t;
    DISABLE_UPDATE
    SetSnapFrames();
    SetSnapSeconds();
    SetSnapBpm();
    ENABLE_UPDATE
}

void PreferencesDialog::OnBpm( wxCommandEvent &event )
{
    if ( event.GetString().IsEmpty() )
        return;
    double a;
    event.GetString().ToDouble( &a );
    int b = (int)((60./a)*44100);
    m_frameSnap = b;
    DISABLE_UPDATE
    SetSnapFrames();
    SetSnapSeconds();
    ENABLE_UPDATE
    SetSnapListData();
}

void PreferencesDialog::OnSeconds( wxCommandEvent &event )
{
    if ( event.GetString().IsEmpty() )
        return;
    double a;
    event.GetString().ToDouble( &a );
    int b = (int) ( a * SAMPLE_RATE );
    m_frameSnap = b;
    DISABLE_UPDATE
    SetSnapFrames();
    SetSnapBpm();
    ENABLE_UPDATE
    SetSnapListData();
}

void PreferencesDialog::OnFrames( wxSpinEvent &event )
{
    m_frameSnap = event.GetInt();
    DISABLE_UPDATE
    SetSnapSeconds();
    SetSnapBpm();
    ENABLE_UPDATE
    SetSnapListData();
}




