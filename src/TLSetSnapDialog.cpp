/* TLSelectColourDialog.h
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
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/config.h>
#include <iostream>

#include "TLSetSnapDialog.h"

#define SAMPLE_RATE 44100
#define HIP_HOP 117600
#define OLD_SKOOL 110250
#define FUNK 94500
enum
{
	ID_FramesCtrl = 1,
	ID_SecondsCtrl,
	ID_HipHopButton,
	ID_OldSkoolButton,
	ID_FunkButton,
	ID_AddButton,
	ID_DeleteButton,
	ID_PresetNameText,
	ID_PresetsList,
};
BEGIN_EVENT_TABLE(TLSetSnapDialog, wxDialog)
	EVT_SPINCTRL(ID_FramesCtrl, TLSetSnapDialog::OnSpin)
	EVT_TEXT(ID_SecondsCtrl,TLSetSnapDialog::OnSecondsText)
	EVT_BUTTON(ID_AddButton, TLSetSnapDialog::OnAddButton)
	EVT_BUTTON(ID_DeleteButton, TLSetSnapDialog::OnDeleteButton)
	EVT_LISTBOX(ID_PresetsList,TLSetSnapDialog::OnPresetsList)
	EVT_TEXT(ID_PresetNameText, TLSetSnapDialog::OnPresetNameText)
	EVT_CLOSE(TLSetSnapDialog::OnClose)
END_EVENT_TABLE()

TLSetSnapDialog::TLSetSnapDialog(wxWindow* parent, wxWindowID id,int SnapPosition, const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxDialog(parent, -1, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER   )
{
	m_Lock=true;
	m_SnapSize.Clear();
	m_SnapSize << ((double)SnapPosition)/SAMPLE_RATE;
	MyDialogFunc(this,true);

	int *len=new int;
	*len=SnapPosition;
	m_presetsListBox->Append(wxT("Custom ..."), (void*)len);
/*	len=new int;
	*len=FUNK;
        m_presetsListBox->Append(wxT("Funk"), (void*)len);
	len=new int;
	*len=HIP_HOP;
        m_presetsListBox->Append(wxT("HipHop"), (void*)len);
	len=new int;
	*len=OLD_SKOOL;
        m_presetsListBox->Append(wxT("OldSkool"), (void*)len);*/

	Load();

	m_presetsListBox->SetSelection(0);
//	FromFramesToSeconds(SnapPosition);
	Select(wxT("Custom ..."),SnapPosition);
	FindWindowById(ID_DeleteButton)->Enable(false);
	m_presetNameText->Enable(false);
	m_Lock=false;
}
void TLSetSnapDialog::Save()
{
	wxConfig config(wxT("ggseq"));
	if (config.Exists(wxT("/Snaps"))) {
		config.DeleteGroup(wxT("/Snaps"));
	}
	config.Flush();
	config.SetPath(wxT("/Snaps"));
	for (int i=1;i<m_presetsListBox->GetCount();i++) {
		int *t=(int*)m_presetsListBox->GetClientData(i);
		config.Write(m_presetsListBox->GetString(i), *t);
	}

}
void TLSetSnapDialog::Load()
{
	wxConfig config(wxT("ggseq"));
	if (!config.Exists(wxT("/Snaps"))) {
		int *len=new int;
		*len=FUNK;
		m_presetsListBox->Append(wxT("Funk"), (void*)len);
		len=new int;
		*len=HIP_HOP;
		m_presetsListBox->Append(wxT("HipHop"), (void*)len);
		len=new int;
		*len=OLD_SKOOL;
		m_presetsListBox->Append(wxT("OldSkool"), (void*)len);
		return;
	}
	config.SetPath(wxT("/Snaps"));
	wxArrayString aNames;
	wxString str;
	long dummy;
	bool bCont = config.GetFirstEntry(str, dummy);
	while ( bCont ) {
		aNames.Add(str);
		bCont = config.GetNextEntry(str, dummy);
	}
	for (unsigned int i=0;i<aNames.Count();i++) {
		wxString str1=aNames.Item(i);
		int *len=new int;
		*len=config.Read(str1,-1);
     		m_presetsListBox->Append(str1, (void*)len);
	}

}
void TLSetSnapDialog::OnClose(wxCloseEvent &event)
{
	event.Skip();
}
TLSetSnapDialog::~TLSetSnapDialog()
{
	Save();
	for (int i=0;i<m_presetsListBox->GetCount();i++) {
		int *t=(int*)m_presetsListBox->GetClientData(i);
		delete t;
	}
}
void TLSetSnapDialog::OnSpin(wxSpinEvent &event)
{
	FromFramesToSeconds();
	Modify(m_SnapPosition);
}
void TLSetSnapDialog::OnAddButton(wxCommandEvent &event)
{
	int *t=new int;
	*t=m_SnapPosition;
	m_presetsListBox->Append(wxT("Unnamed"),(void*)t);
	m_presetsListBox->SetSelection(m_presetsListBox->GetCount()-1);
	Select(wxT("Unnamed"),*t);
	FindWindowById(ID_DeleteButton)->Enable();
	m_presetNameText->Enable();

}
void TLSetSnapDialog::OnDeleteButton(wxCommandEvent &event)
{
	int index = m_presetsListBox->GetSelection();
	if (index>=0) {
		int *t=(int*)m_presetsListBox->GetClientData(index);
		delete t;
		m_presetsListBox->Delete(index);
	}
}
void TLSetSnapDialog::OnPresetNameText(wxCommandEvent &event)
{
	Modify(event.GetString());
}
void TLSetSnapDialog::OnPresetsList(wxCommandEvent &event)
{
	if (event.GetInt()==0) {
		FindWindowById(ID_DeleteButton)->Enable(false);
		m_presetNameText->Enable(false);
	} else {
		FindWindowById(ID_DeleteButton)->Enable();
		m_presetNameText->Enable();
	}
	int* t=(int*)m_presetsListBox->GetClientData(m_presetsListBox->GetSelection());
	Select(event.GetString(), *t);
}
void TLSetSnapDialog::Select(wxString str, int length)
{
	m_presetNameText->SetValue(str);
	FromFramesToSeconds(length);
}
void TLSetSnapDialog::Modify(wxString str)
{
	int index = m_presetsListBox->GetSelection();
	if (index>=0) {
		m_presetsListBox->SetString(index,str);
	}
	
}
void TLSetSnapDialog::Modify(int length)
{
	int index = m_presetsListBox->GetSelection();
	if (index>=0) {
		int* t=(int*)m_presetsListBox->GetClientData(index);
		*t = length;
	}
}
void TLSetSnapDialog::FromFramesToSeconds(int Frames)
{
	m_Lock=true;
	if(Frames>0) {
		m_FramesSpinCtrl->SetValue(Frames);
	}
	m_SnapPosition=m_FramesSpinCtrl->GetValue();
	double a = ((double)m_SnapPosition)/SAMPLE_RATE;
	m_SecondsTextCtrl->Clear();
	*m_SecondsTextCtrl << a;
	m_Lock=false;
}
void TLSetSnapDialog::OnSecondsText(wxCommandEvent &event)
{
	if (!m_Lock) {
		double a;
		m_SecondsTextCtrl->GetValue().ToDouble(&a);
		int b = (int)(a*44100);
		m_FramesSpinCtrl->SetValue(b);
		m_SnapPosition=b;
	}
}
wxSizer *MyDialogFunc( TLSetSnapDialog *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item1 = new wxStaticText( parent, ID_TEXT, wxT("Snap Position"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->SetFont( wxFont( 20, wxDEFAULT, wxNORMAL, wxBOLD ) );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
    item1->SetSize( item1->GetBestSize() );
#endif
    item0->Add( item1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item4 = new wxStaticBox( parent, -1, wxT("Length") );
    wxStaticBoxSizer *item3 = new wxStaticBoxSizer( item4, wxVERTICAL );

    wxFlexGridSizer *item5 = new wxFlexGridSizer( 2, 5, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Frames"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTRE, 5 );

    parent->m_FramesSpinCtrl = new wxSpinCtrl( parent, ID_FramesCtrl, wxT("117600"), wxDefaultPosition, wxDefaultSize, 0, 0, 200000, 117600 );
    item5->Add( parent->m_FramesSpinCtrl, 0, wxALIGN_CENTRE, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_TEXT, wxT("Seconds"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item8, 0, wxALIGN_CENTRE, 5 );

    wxTextValidator tVal(wxFILTER_NUMERIC, &(parent->m_SnapSize));

    parent->m_SecondsTextCtrl = new wxTextCtrl( parent, ID_SecondsCtrl, parent->m_SnapSize, wxDefaultPosition, wxSize(80,-1), 0,tVal );
    item5->Add( parent->m_SecondsTextCtrl, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );


    item3->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxRIGHT, 5 );

    wxStaticBox *item11 = new wxStaticBox( parent, -1, wxT("Presets") );
    wxStaticBoxSizer *item10 = new wxStaticBoxSizer( item11, wxVERTICAL );


// -Neu
/*   wxString strs12[] = 
    {
        wxT("Custom ..."), 
        wxT("Funk"), 
        wxT("HipHop"), 
        wxT("OldSkool")
    };*/
    parent->m_presetsListBox = new wxListBox( parent, ID_PresetsList, wxDefaultPosition, wxSize(80,100), 0,NULL/*4, strs12*/, wxLB_SINGLE );
    item10->Add( parent->m_presetsListBox, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *_item13 = new wxBoxSizer( wxVERTICAL );

    parent->m_presetNameText = new wxTextCtrl( parent, ID_PresetNameText, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    _item13->Add( parent->m_presetNameText, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxGridSizer *_item15 = new wxGridSizer( 2, 5, 5 );

    wxButton *_item16 = new wxButton( parent, ID_DeleteButton, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    _item15->Add( _item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxButton *_item17 = new wxButton( parent, ID_AddButton, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
    _item15->Add( _item17, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    _item13->Add( _item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item10->Add( _item13, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5 );


// End -Neu


/*    wxButton *item12 = new wxButton( parent, ID_HipHopButton, wxT("HipHop"), wxDefaultPosition, wxDefaultSize, 0 );
#ifdef __WXMSW__
    item10->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
    item10->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
#endif
    wxButton *item13 = new wxButton( parent, ID_OldSkoolButton, wxT("Old Skool"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    wxButton *item14 = new wxButton( parent, ID_FunkButton, wxT("Funk"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
*/
    item2->Add( item10, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL, 5 );

    item0->Add( item2, 1, wxGROW|wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    wxStaticLine *item15 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item17 = new wxButton( parent, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );

    wxButton *item18 = new wxButton( parent, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item18->SetDefault();
#ifdef __WXMSW__ 
    item16->Add( item18, 0, wxALIGN_RIGHT|wxRIGHT|wxTOP, 5 );
    item16->Add( item17, 0, wxTOP, 5 );
#else
    item16->Add( item17, 0, wxALIGN_RIGHT|wxRIGHT|wxTOP, 5 );
    item16->Add( item18, 0, wxTOP, 5 );
#endif
    item0->Add( item16, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5 );

    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}
