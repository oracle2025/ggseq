/* ShortcutsDialog.h
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

#ifndef _SHORTCUTS_DIALOG_H_
#define _SHORTCUTS_DIALOG_H_

class ShortcutsDialog: public wxDialog
{
	public:
		ShortcutsDialog(wxWindow* parent);
		~ShortcutsDialog();
		wxAcceleratorTable GetAccelTable();
		void OnItemSelected(wxListEvent &event);
		void OnItemDeselected(wxListEvent &event);
		void OnChoice(wxCommandEvent &event);
		void OnCombo(wxCommandEvent &event);
		void OnComboText(wxCommandEvent &event);
		void Save();
	private:
		wxString PrintModifier(int flags);
		wxString PrintKey(int keycode);
		int CalcKeyCode(wxString str);
		void CreateDialog();
		void SetListEntry(long item, wxAcceleratorEntry *entry);
		void AppendListEntry(wxString cmdName, int modifier, int keyCode, int cmd);
		wxListView *m_listCtrl;
		wxComboBox *m_keyCombo;
		wxChoice *m_modifierChoice;
		wxStaticText *m_keyText;
		DECLARE_EVENT_TABLE()
};


#endif /*_SHORTCUTS_DIALOG_H_ */
