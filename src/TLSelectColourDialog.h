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
#ifndef _TLSELECTCOLOURDIALOG_H_
#define _TLSELECTCOLOURDIALOG_H_
class wxGenericDirCtrl;
class TLSelectColourDialog: public wxDialog
{
	public:
		TLSelectColourDialog(wxWindow* parent, wxWindowID id,TLColourManager* cm, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
		~TLSelectColourDialog();
		void OnAddButtonClick(wxCommandEvent& event);
		void OnRemoveButtonClick(wxCommandEvent& event);
		void OnColourButtonClick(wxCommandEvent& event);
		void OnDirListClick(wxCommandEvent& event);
		void SetColours(TLColourManager* cm);
		void SetPath(wxString dir);
	private:
		void SetButtonStates();
		wxGenericDirCtrl *m_dirTreeCtrl;
		wxListBox *m_dirListBox;
		wxButton *m_addButton;
		wxButton *m_removeButton;
		wxButton *m_colourButton;
		DECLARE_EVENT_TABLE()

};


#endif /*_TLSELECTCOLOURDIALOG_H_*/
