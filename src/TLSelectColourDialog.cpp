/* TLSelectColourDialog.cpp
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

#include <wx/dirctrl.h>
#include <wx/colordlg.h>
#include <wx/statline.h>
#include <wx/config.h>

#include "TLColourManager.h"
#include "TLSelectColourDialog.h"

enum
{
	ID_AddButton = 1,
	ID_RemoveButton,
	ID_ColourButton,
	ID_DirListBox,
};
BEGIN_EVENT_TABLE(TLSelectColourDialog, wxDialog)
	EVT_BUTTON(ID_AddButton,TLSelectColourDialog::OnAddButtonClick)
	EVT_BUTTON(ID_RemoveButton,TLSelectColourDialog::OnRemoveButtonClick)
	EVT_BUTTON(ID_ColourButton,TLSelectColourDialog::OnColourButtonClick)
	EVT_LISTBOX(ID_DirListBox,TLSelectColourDialog::OnDirListClick)
END_EVENT_TABLE()

TLSelectColourDialog::TLSelectColourDialog(wxWindow* parent, wxWindowID id,TLColourManager* cm, const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxDialog(parent, -1, title, pos, size, wxDEFAULT_DIALOG_STYLE |wxRESIZE_BORDER  )
{
	wxBoxSizer *MainSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticText *item1 = new wxStaticText( this, -1, wxT("Colours"), wxDefaultPosition, wxDefaultSize, 0 );
//	item1->SetFont( wxFont( 20, wxDEFAULT, wxNORMAL, wxBOLD ) );
	item1->SetFont( wxFont( 20, wxSWISS, wxNORMAL, wxBOLD ) );
	MainSizer->Add( item1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
	wxBoxSizer *Sizer1 = new wxBoxSizer( wxHORIZONTAL );
	MainSizer->Add(Sizer1,1,wxEXPAND);

	wxStaticLine *sl = new wxStaticLine(this,-1);
	MainSizer->Add(sl,0,wxEXPAND|wxALL,5);
	wxBoxSizer *ButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	MainSizer->Add(ButtonSizer,0,wxEXPAND|wxALL,5);

	wxButton *button1 = new wxButton(this,wxID_CANCEL,wxT("Cancel"));
	wxButton *button2 = new wxButton(this,wxID_OK,wxT("Ok"));
	button2->SetDefault();
	ButtonSizer->Add(0,0,1);
#ifdef __WXMSW__ 
	ButtonSizer->Add(button2,0,wxALIGN_RIGHT|wxRIGHT,5);
	ButtonSizer->Add(button1);
#else
	ButtonSizer->Add(button1,0,wxALIGN_RIGHT|wxRIGHT,5);
	ButtonSizer->Add(button2);
#endif	
	m_dirTreeCtrl = new wxGenericDirCtrl(this,-1,wxDirDialogDefaultFolderStr,wxDefaultPosition,wxDefaultSize,wxDIRCTRL_DIR_ONLY|wxSUNKEN_BORDER );
	Sizer1->Add(m_dirTreeCtrl,1,wxEXPAND|wxALL,5);
	
	wxBoxSizer *Sizer2 = new wxBoxSizer( wxVERTICAL );
	Sizer1->Add(Sizer2,1,wxEXPAND|wxRIGHT|wxTOP|wxBOTTOM,5);
	
	m_dirListBox = new wxListBox(this,ID_DirListBox);
//	lb->Append(wxT("/usr/local/"));
	for ( TLDirColourList::Node *node = cm->GetColours(); node; node = node->GetNext() ) {
		TLDirColour *current = node->GetData();
		wxColour *colour = new wxColour(current->m_colour);
		m_dirListBox->Append(current->m_directory,(void*)colour);
	}



	Sizer2->Add(m_dirListBox,1,wxEXPAND|wxBOTTOM,5);
	
	wxBoxSizer *Sizer3 = new wxBoxSizer( wxHORIZONTAL );
	Sizer2->Add(Sizer3,0,wxEXPAND|wxBOTTOM,5);
	
	m_addButton = new wxButton(this,ID_AddButton,wxT("+"));
	m_removeButton = new wxButton(this,ID_RemoveButton,wxT("-"));
	Sizer3->Add(m_addButton,1,wxRIGHT,5);
	Sizer3->Add(m_removeButton,1);
	
	m_colourButton = new wxButton(this,ID_ColourButton,wxT("Colour"));
//	m_colourButton->SetBackgroundColour(*wxRED);
	Sizer2->Add(m_colourButton,0,wxEXPAND);
	
	SetSizer( MainSizer );
	MainSizer->SetSizeHints(this);
	SetButtonStates();
}
TLSelectColourDialog::~TLSelectColourDialog()
{
	for (int i=0; i<m_dirListBox->GetCount();i++) {
		wxColour *colour = (wxColour*)m_dirListBox->GetClientData(i);
		delete colour;	
	}
}
void TLSelectColourDialog::OnAddButtonClick(wxCommandEvent& event)
{
	for (int i=0; i<m_dirListBox->GetCount();i++) {
		if (m_dirTreeCtrl->GetPath()==m_dirListBox->GetString(i))
			return;
	}
	wxColour *colour = new wxColour(*wxRED);
	m_dirListBox->Append(m_dirTreeCtrl->GetPath(),(void*)colour);
	m_dirListBox->SetSelection(m_dirListBox->GetCount()-1);
	SetButtonStates();
}
void TLSelectColourDialog::OnRemoveButtonClick(wxCommandEvent& event)
{
	if (m_dirListBox->GetSelection()>=0) {
		wxColour *colour = (wxColour*)m_dirListBox->GetClientData(m_dirListBox->GetSelection());
		delete colour;
		m_dirListBox->Delete(m_dirListBox->GetSelection());
	}
	SetButtonStates();
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
void TLSelectColourDialog::OnColourButtonClick(wxCommandEvent& event)
{
	wxColourData retData;
	if (m_dirListBox->GetSelection()<0)
		return;
	if (1) {
		wxConfigBase *conf=wxConfigBase::Get();
//		wxConfig config(wxT("ggseq"));
		conf->SetPath(wxT("/ColourDialog"));
		retData.SetCustomColour(0,wxColour(conf->Read(wxT("colour0"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(1,wxColour(conf->Read(wxT("colour1"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(2,wxColour(conf->Read(wxT("colour2"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(3,wxColour(conf->Read(wxT("colour3"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(4,wxColour(conf->Read(wxT("colour4"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(5,wxColour(conf->Read(wxT("colour5"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(6,wxColour(conf->Read(wxT("colour6"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(7,wxColour(conf->Read(wxT("colour7"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(8,wxColour(conf->Read(wxT("colour8"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(9,wxColour(conf->Read(wxT("colour9"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(10,wxColour(conf->Read(wxT("colour10"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(11,wxColour(conf->Read(wxT("colour11"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(12,wxColour(conf->Read(wxT("colour12"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(13,wxColour(conf->Read(wxT("colour13"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(14,wxColour(conf->Read(wxT("colour14"),GetColourValue(*wxWHITE))));
		retData.SetCustomColour(15,wxColour(conf->Read(wxT("colour15"),GetColourValue(*wxWHITE))));
	}
	wxColourDialog dialog(this, &retData);
	if (dialog.ShowModal() == wxID_OK)
	{
		retData = dialog.GetColourData();
		wxColour col = retData.GetColour();
		wxColour *col2 = (wxColour*)m_dirListBox->GetClientData(m_dirListBox->GetSelection());
		*col2 = col;
		m_colourButton->SetBackgroundColour(*col2);
		wxConfigBase *conf=wxConfigBase::Get();
//		wxConfig config(wxT("ggseq"));
		conf->SetPath(wxT("/ColourDialog"));
		conf->Write(wxT("colour0"),(long)GetColourValue(retData.GetCustomColour(0)));
		conf->Write(wxT("colour1"),(long)GetColourValue(retData.GetCustomColour(1)));
		conf->Write(wxT("colour2"),(long)GetColourValue(retData.GetCustomColour(2)));
		conf->Write(wxT("colour3"),(long)GetColourValue(retData.GetCustomColour(3)));
		conf->Write(wxT("colour4"),(long)GetColourValue(retData.GetCustomColour(4)));
		conf->Write(wxT("colour5"),(long)GetColourValue(retData.GetCustomColour(5)));
		conf->Write(wxT("colour6"),(long)GetColourValue(retData.GetCustomColour(6)));
		conf->Write(wxT("colour7"),(long)GetColourValue(retData.GetCustomColour(7)));
		conf->Write(wxT("colour8"),(long)GetColourValue(retData.GetCustomColour(8)));
		conf->Write(wxT("colour9"),(long)GetColourValue(retData.GetCustomColour(9)));
		conf->Write(wxT("colour10"),(long)GetColourValue(retData.GetCustomColour(10)));
		conf->Write(wxT("colour11"),(long)GetColourValue(retData.GetCustomColour(11)));
		conf->Write(wxT("colour12"),(long)GetColourValue(retData.GetCustomColour(12)));
		conf->Write(wxT("colour13"),(long)GetColourValue(retData.GetCustomColour(13)));
		conf->Write(wxT("colour14"),(long)GetColourValue(retData.GetCustomColour(14)));
		conf->Write(wxT("colour15"),(long)GetColourValue(retData.GetCustomColour(15)));
	}
}
void TLSelectColourDialog::OnDirListClick(wxCommandEvent& event)
{
	SetButtonStates();
}
void TLSelectColourDialog::SetButtonStates()
{
	if (m_dirListBox->GetSelection()<0) {
		m_removeButton->Enable(false);
		m_colourButton->Enable(false);
	} else{
		m_removeButton->Enable(true);
		m_colourButton->Enable(true);
		m_colourButton->SetBackgroundColour(*(wxColour*)m_dirListBox->GetClientData(m_dirListBox->GetSelection()));
	}
}
void TLSelectColourDialog::SetColours(TLColourManager* cm)
{
	cm->Clear();
	for (int i=0; i<m_dirListBox->GetCount();i++) {
		cm->SetColour(m_dirListBox->GetString(i),*(wxColour*)m_dirListBox->GetClientData(i));
	}
}
void TLSelectColourDialog::SetPath(wxString dir)
{
	m_dirTreeCtrl->SetPath(dir);
}
