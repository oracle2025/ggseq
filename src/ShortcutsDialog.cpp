/* ShortcutsDialog.cpp
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
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/accel.h>
#include <wx/config.h>
#include "ShortcutsDialog.h"
#include "stuff.h"


enum
{
	ID_TEXT = 1,
	ID_LISTCTRL,
	ID_CHOICE,
	ID_LINE,
	ID_COMBO,
};
BEGIN_EVENT_TABLE(ShortcutsDialog, wxDialog)
	EVT_LIST_ITEM_SELECTED(ID_LISTCTRL,ShortcutsDialog::OnItemSelected)
	EVT_LIST_ITEM_DESELECTED(ID_LISTCTRL,ShortcutsDialog::OnItemDeselected)
	EVT_TEXT(ID_COMBO,ShortcutsDialog::OnComboText)
	EVT_COMBOBOX(ID_COMBO, ShortcutsDialog::OnCombo)
	EVT_CHOICE(ID_CHOICE, ShortcutsDialog::OnChoice)
END_EVENT_TABLE()

ShortcutsDialog::ShortcutsDialog(wxWindow* parent) : wxDialog(parent, -1,wxT("Setup Keyboard Shortcuts"),wxDefaultPosition,wxDefaultSize, wxDEFAULT_DIALOG_STYLE |wxRESIZE_BORDER)
{
	CreateDialog();
	m_listCtrl->InsertColumn(0,wxT("Command"));
	m_listCtrl->InsertColumn(1,wxT("Key"));

	AppendListEntry(wxT("Play"),wxACCEL_NORMAL, WXK_SPACE,ID_PLAY);
	AppendListEntry(wxT("Stop"),wxACCEL_NORMAL,'S',ID_STOP);
	AppendListEntry(wxT("Rewind"),wxACCEL_NORMAL,'R',ID_REWIND);
	AppendListEntry(wxT("New File"),wxACCEL_CTRL, 'N',wxID_NEW);
	AppendListEntry(wxT("Open"),wxACCEL_CTRL,'O',wxID_OPEN);
	AppendListEntry(wxT("Save"),wxACCEL_CTRL,'S',wxID_SAVE);
	AppendListEntry(wxT("Exit"),wxACCEL_CTRL,'Q',wxID_EXIT);
	if (m_listCtrl->GetSelectedItemCount()==0) {
		m_keyCombo->Enable(false);
		m_modifierChoice->Enable(false);
	}
}

ShortcutsDialog::~ShortcutsDialog()
{
	for(int i=0; i<m_listCtrl->GetItemCount(); i++) {
		wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(i);
		delete entry;
	}
}
void ShortcutsDialog::AppendListEntry(wxString cmdName, int modifier, int keyCode, int cmd)
{
	wxAcceleratorEntry *entry = new wxAcceleratorEntry(modifier, keyCode,cmd);
	int index=m_listCtrl->GetItemCount();
	m_listCtrl->InsertItem(index,cmdName);

	wxString KeyString;
	KeyString << PrintModifier(entry->GetFlags());
	KeyString << PrintKey(entry->GetKeyCode());

	m_listCtrl->SetItem(index,1,KeyString);
	m_listCtrl->SetItemData(index,(long)entry);
}
void ShortcutsDialog::OnItemSelected(wxListEvent &event)
{
	wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(event.GetIndex());
	int flags = entry->GetFlags();
	switch (flags) {
		case wxACCEL_NORMAL:
			m_modifierChoice->SetSelection(0);
			break;
		case wxACCEL_ALT:
			m_modifierChoice->SetSelection(1);
			break;
		case wxACCEL_CTRL:
			m_modifierChoice->SetSelection(2);
			break;
		case wxACCEL_SHIFT:
			m_modifierChoice->SetSelection(3);
			break;
	}
	m_keyCombo->SetValue(PrintKey(entry->GetKeyCode()));
	m_keyText->SetLabel(event.GetLabel());
	if (m_listCtrl->GetSelectedItemCount()>0) {
		m_keyCombo->Enable(true);
		m_modifierChoice->Enable(true);
	}

}
void ShortcutsDialog::OnItemDeselected(wxListEvent &event)
{
	if (m_listCtrl->GetSelectedItemCount()==0) {
		m_keyCombo->Enable(false);
		m_modifierChoice->Enable(false);
	}
}
void ShortcutsDialog::OnChoice(wxCommandEvent &event)
{
	int modifier=m_modifierChoice->GetSelection();
	long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if ( modifier<0 || item<0 )
		return;
	wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(item);
	switch (modifier) {
		case 0:
			entry->Set(wxACCEL_NORMAL,entry->GetKeyCode(),entry->GetCommand());
			break;
		case 1:
			entry->Set(wxACCEL_ALT,entry->GetKeyCode(),entry->GetCommand());
			break;
		case 2:
			entry->Set(wxACCEL_CTRL,entry->GetKeyCode(),entry->GetCommand());
			break;
		case 3:
			entry->Set(wxACCEL_SHIFT,entry->GetKeyCode(),entry->GetCommand());
			break;
	}
	SetListEntry(item, entry);
}
void ShortcutsDialog::OnCombo(wxCommandEvent &event)
{
	long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if ( item<0 )
		return;
	wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(item);
	entry->Set(entry->GetFlags(),CalcKeyCode(event.GetString()), entry->GetCommand());
	
	SetListEntry(item, entry);
}
void ShortcutsDialog::OnComboText(wxCommandEvent &event)
{
	long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if ( item<0 )
		return;
	wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(item);
	entry->Set(entry->GetFlags(),CalcKeyCode(event.GetString()), entry->GetCommand());
	
	SetListEntry(item, entry);
}
void ShortcutsDialog::SetListEntry(long item, wxAcceleratorEntry *entry)
{
	wxString KeyString;
	KeyString << PrintModifier(entry->GetFlags());
	KeyString << PrintKey(entry->GetKeyCode());
	m_listCtrl->SetItem(item,1,KeyString);
}
void ShortcutsDialog::Save()
{
	wxConfig conf(wxT("ggseq"));
	conf.SetPath(wxT("/Keyboard_Shortcuts"));
	wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(0);
	conf.Write(wxT("PLAY_FLAGS"), entry->GetFlags());
	conf.Write(wxT("PLAY_KEYCODE"), entry->GetKeyCode());

	entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(1);
	conf.Write(wxT("STOP_FLAGS"), entry->GetFlags());
	conf.Write(wxT("STOP_KEYCODE"), entry->GetKeyCode());
	entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(2);
	conf.Write(wxT("REWIND_FLAGS"), entry->GetFlags());
	conf.Write(wxT("REWIND_KEYCODE"), entry->GetKeyCode());
	entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(3);
	conf.Write(wxT("NEW_FLAGS"), entry->GetFlags());
	conf.Write(wxT("NEW_KEYCODE"), entry->GetKeyCode());
	entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(4);
	conf.Write(wxT("OPEN_FLAGS"), entry->GetFlags());
	conf.Write(wxT("OPEN_KEYCODE"), entry->GetKeyCode());
	entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(5);
	conf.Write(wxT("SAVE_FLAGS"), entry->GetFlags());
	conf.Write(wxT("SAVE_KEYCODE"), entry->GetKeyCode());
	entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(6);
	conf.Write(wxT("EXIT_FLAGS"), entry->GetFlags());
	conf.Write(wxT("EXIT_KEYCODE"), entry->GetKeyCode());
}
wxString ShortcutsDialog::PrintModifier(int flags)
{
	switch (flags) {
		case wxACCEL_NORMAL:
			return wxT("");
		case wxACCEL_ALT:
			return wxT("ALT-");
		case wxACCEL_CTRL:
			return wxT("CTRL-"); /*TODO i18n ? */
		case wxACCEL_SHIFT:
			return wxT("SHIFT-");
	}
	return wxT("");
}
wxString ShortcutsDialog::PrintKey(int keycode)
{
	switch (keycode) {
		case WXK_BACK:
			return wxT("[BACK]");
		case WXK_TAB:
			return wxT("[TAB]");
		case WXK_RETURN:
			return wxT("[RETURN]");
		case WXK_ESCAPE:
			return wxT("[ESCAPE]");
		case WXK_SPACE:
			return wxT("[SPACE]");
		case WXK_DELETE:
			return wxT("[DELETE]");
		case WXK_END:
			return wxT("[END]");
		case WXK_HOME:
			return wxT("[HOME]");
		case WXK_LEFT:
			return wxT("[LEFT]");
		case WXK_UP:
			return wxT("[UP]");
		case WXK_RIGHT:
			return wxT("[RIGHT]");
		case WXK_DOWN:
			return wxT("[DOWN]");
		case WXK_F1:
			return wxT("[F1]");
		case WXK_F2:
			return wxT("[F2]");
		case WXK_F3:
			return wxT("[F3]");
		case WXK_F4:
			return wxT("[F4]");
		case WXK_F5:
			return wxT("[F5]");
		case WXK_F6:
			return wxT("[F6]");
		case WXK_F7:
			return wxT("[F7]");
		case WXK_F8:
			return wxT("[F8]");
		case WXK_F9:
			return wxT("[F9]");
		case WXK_F10:
			return wxT("[F10]");
		case WXK_F11:
			return wxT("[F11]");
		case WXK_F12:
			return wxT("[F12]");
		default:
			wxString tmp;
			tmp << (wxChar)keycode;
			return tmp;
	}
 /*WXK_BACK    = 8
 WXK_TAB     = 9
 WXK_RETURN  = 13
 WXK_ESCAPE  = 27
 WXK_SPACE   = 32
 WXK_DELETE  = 127

 WXK_START   = 300
 WXK_LBUTTON
 WXK_RBUTTON
 WXK_CANCEL
 WXK_MBUTTON
 WXK_CLEAR
 WXK_SHIFT
 WXK_CONTROL
 WXK_MENU
 WXK_PAUSE
 WXK_CAPITAL
 WXK_PRIOR
 WXK_NEXT
 WXK_END
 WXK_HOME
 WXK_LEFT
 WXK_UP
 WXK_RIGHT
 WXK_DOWN
 WXK_SELECT
 WXK_PRINT
 WXK_EXECUTE
 WXK_SNAPSHOT
 WXK_INSERT
 WXK_HELP
 WXK_NUMPAD0
 WXK_NUMPAD1
 WXK_NUMPAD2
 WXK_NUMPAD3
 WXK_NUMPAD4
 WXK_NUMPAD5
 WXK_NUMPAD6
 WXK_NUMPAD7
 WXK_NUMPAD8
 WXK_NUMPAD9
 WXK_MULTIPLY
 WXK_ADD
 WXK_SEPARATOR
 WXK_SUBTRACT
 WXK_DECIMAL
 WXK_DIVIDE
 WXK_F1
 WXK_F2
 WXK_F3
 WXK_F4
 WXK_F5
 WXK_F6
 WXK_F7
 WXK_F8
 WXK_F9
 WXK_F10
 WXK_F11
 WXK_F12
 WXK_F13
 WXK_F14
 WXK_F15
 WXK_F16
 WXK_F17
 WXK_F18
 WXK_F19
 WXK_F20
 WXK_F21
 WXK_F22
 WXK_F23
 WXK_F24
 WXK_NUMLOCK
 WXK_SCROLL */
}
int ShortcutsDialog::CalcKeyCode(wxString str)
{
	if (str==wxT("[BACK]"))
		return WXK_BACK;
	else if (str==wxT("[TAB]"))
		return WXK_TAB;
	else if (str==wxT("[RETURN]"))
		return WXK_RETURN;
	else if (str==wxT("[ESCAPE]"))
		return WXK_ESCAPE;
	else if (str==wxT("[SPACE]"))
		return WXK_SPACE;
	else if (str==wxT("[DELETE]"))
		return WXK_DELETE;
	else if (str==wxT("[END]"))
		return WXK_END;
	else if (str==wxT("[HOME]"))
		return WXK_HOME;
	else if (str==wxT("[LEFT]"))
		return WXK_LEFT;
	else if (str==wxT("[UP]"))
		return WXK_UP;
	else if (str==wxT("[RIGHT]"))
		return WXK_RIGHT;
	else if (str==wxT("[DOWN]"))
		return WXK_DOWN;
	else if (str==wxT("[F1]"))
		return WXK_F1;
	else if (str==wxT("[F2]"))
		return WXK_F2;
	else if (str==wxT("[F3]"))
		return WXK_F3;
	else if (str==wxT("[F4]"))
		return WXK_F4;
	else if (str==wxT("[F5]"))
		return WXK_F5;
	else if (str==wxT("[F6]"))
		return WXK_F6;
	else if (str==wxT("[F7]"))
		return WXK_F7;
	else if (str==wxT("[F8]"))
		return WXK_F8;
	else if (str==wxT("[F9]"))
		return WXK_F9;
	else if (str==wxT("[F10]"))
		return WXK_F10;
	else if (str==wxT("[F11]"))
		return WXK_F11;
	else if (str==wxT("[F12]"))
		return WXK_F12;
	wxChar tmp = str[(unsigned int)0];
	return (int)tmp;
}
void ShortcutsDialog::CreateDialog()
{

   wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item1 = new wxStaticText( this, ID_TEXT, wxT("Keyboard Shortcuts"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->SetFont( wxFont( 20, wxSWISS, wxNORMAL, wxBOLD ) );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
    item1->SetSize( item1->GetBestSize() );
#endif
    item0->Add( item1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item3 = new wxStaticBox( this, -1, wxT("Command") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );

    //wxListCtrl *item4 = new wxListCtrl( this, ID_LISTCTRL, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER );
    m_listCtrl = new wxListView( this, ID_LISTCTRL, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item2->Add( m_listCtrl, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item2, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item6 = new wxStaticBox( this, -1, wxT("Key") );
    wxStaticBoxSizer *item5 = new wxStaticBoxSizer( item6, wxVERTICAL );

    wxBoxSizer *item7 = new wxBoxSizer( wxVERTICAL );

    m_keyText = new wxStaticText( this, ID_TEXT, wxT("Play"), wxDefaultPosition, wxDefaultSize, 0 );
    m_keyText->SetFont( wxFont( 12, wxSWISS, wxNORMAL, wxBOLD ) );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
    m_keyText->SetSize( item8->GetBestSize() );
#endif
    item7->Add( m_keyText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item10 = new wxStaticText( this, ID_TEXT, wxT("Modifier"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs11[] = 
    {
        wxT("None"), 
        wxT("Alt"), 
        wxT("Ctrl"), 
        wxT("Shift")
    };
    m_modifierChoice = new wxChoice( this, ID_CHOICE, wxDefaultPosition, wxDefaultSize, 4, strs11, 0 );
    item9->Add( m_modifierChoice, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item12 = new wxStaticText( this, ID_TEXT, wxT("Key"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs13[] = 
    {
	wxT("[BACK]"),
	wxT("[TAB]"),
	wxT("[RETURN]"),
	wxT("[ESCAPE]"),
	wxT("[SPACE]"),
	wxT("[DELETE]"),
	wxT("[END]"),
	wxT("[HOME]"),
	wxT("[LEFT]"),
	wxT("[UP]"),
	wxT("[RIGHT]"),
	wxT("[DOWN]"),
	wxT("[F1]"),
	wxT("[F2]"),
	wxT("[F3]"),
	wxT("[F4]"),
	wxT("[F5]"),
	wxT("[F6]"),
	wxT("[F7]"),
	wxT("[F8]"),
	wxT("[F9]"),
	wxT("[F10]"),
	wxT("[F11]"),
	wxT("[F12]")
    };
    m_keyCombo = new wxComboBox( this, ID_COMBO, wxT(""), wxDefaultPosition, wxDefaultSize, 24, strs13, wxCB_DROPDOWN );
    item9->Add( m_keyCombo, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item7->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5->Add( item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

/*    wxButton *item18 = new wxButton(this, -1, wxT("Restore Defaults"));

    item0->Add( item18, 0, wxALIGN_RIGHT|wxALL, 5 );*/

    wxStaticLine *item14 = new wxStaticLine( this, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item15 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item16 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item16, 0, wxALIGN_CENTRE|wxRIGHT, 5 );

    wxButton *item17 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item17->SetDefault();
    item15->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5 );

	this->SetAutoLayout( TRUE );
	this->SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
}

wxAcceleratorTable ShortcutsDialog::GetAccelTable()
{
	int length=m_listCtrl->GetItemCount();
	wxAcceleratorEntry table[length];
	for (int i=0;i<length;i++) {
		wxAcceleratorEntry *entry=(wxAcceleratorEntry*)m_listCtrl->GetItemData(i);
		table[i]=*entry;
	}
	wxAcceleratorTable t1(length,table);
	return t1/*wxNullAcceleratorTable*/;
}
