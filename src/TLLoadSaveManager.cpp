/* TLLoadSaveManager.cpp
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
#include <wx/config.h>
#include <wx/filename.h>
#include "stuff.h"

#include "TLLoadSaveManager.h"
#include "TLData.h"
#define GG_FILE_EXTS wxT("Gungirl files (*.ggseq)|*.ggseq")


TLLoadSaveManager::TLLoadSaveManager(wxWindow *parent, TLData *data)
{
	m_data=data;
	m_parent=parent;
}

void TLLoadSaveManager::New()
{
	if (m_data->UnsavedChanges()) {
		wxMessageDialog msg_dlg(m_parent,wxT("Save Changes?"), wxT("Unsaved Changes"), wxYES_NO | wxCANCEL|wxICON_QUESTION );
		int result = msg_dlg.ShowModal();
		switch (result) {
			case wxID_CANCEL:
				return;
			case wxID_YES:
				if (!Save())
					return;
				break;
		}
	}
	m_data->Clear();
}
bool TLLoadSaveManager::Save() /*return true on success*/
{
	if (m_data->GetFilename().IsEmpty()) {
		return SaveAs();
	} else {
		m_data->Save();
	}
	return true;
}
bool TLLoadSaveManager::SaveAs()/*Datei auf existenz prüfen*/
{
	wxConfig config(wxT("ggseq"));
	wxString lastFolder = config.Read(wxT("LastSaveFolder"), wxT(""));
	
	wxString filename = wxFileSelector(wxT("Save File as"),lastFolder,wxT(""),wxT("ggseq"),GG_FILE_EXTS,wxSAVE,m_parent);
	if ( !filename.empty() ) {
		if (wxFileExists(filename)) {
			wxMessageDialog msg_dlg(m_parent,wxT("File exists!\nOverride?"), wxT("Override File?"), wxYES_NO |wxICON_QUESTION );
			if (msg_dlg.ShowModal()==wxID_NO)
				return false;
		}
		wxString dir;
		wxFileName::SplitPath(filename, &dir, NULL, NULL);
		config.Write(wxT("LastSaveFolder"),dir);
		m_data->Save(filename);
		return true;

	}
	return false;
}
void TLLoadSaveManager::Load()
{


	if (m_data->UnsavedChanges()) {
		wxMessageDialog msg_dlg(m_parent,wxT("Save Changes?"), wxT("Unsaved Changes"), wxYES_NO | wxCANCEL|wxICON_QUESTION );

		int result = msg_dlg.ShowModal();
		switch (result) {
			case wxID_CANCEL:
				return;
			case wxID_YES:
				if (!Save())
					return;
				break;
		}
	}
	wxConfig config(wxT("ggseq"));
	wxString lastFolder = config.Read(wxT("LastLoadFolder"), wxT(""));

	wxString filename = wxFileSelector(wxT("Open File"),lastFolder,wxT(""),wxT("ggseq"),GG_FILE_EXTS,wxOPEN|wxHIDE_READONLY,m_parent);
	if ( !filename.empty() ) {
		wxString dir;
		wxFileName::SplitPath(filename, &dir, NULL, NULL);
		config.Write(wxT("LastLoadFolder"),dir);
		m_data->Load(filename);
	}
}
bool TLLoadSaveManager::SaveOnExit()
{
	if (m_data->UnsavedChanges()) {
		wxMessageDialog msg_dlg(m_parent,wxT("Save Changes?"), wxT("Unsaved Changes"), wxYES_NO | wxCANCEL|wxICON_QUESTION );
		int result = msg_dlg.ShowModal();
		switch (result) {
			case wxID_CANCEL:
				return false;
				break;
			case wxID_YES:
				if (Save())
					return true;
				return false;
				break;
			case wxID_NO:
				return true;
				break;
		}
	} else {
		return true;
	}
	wxASSERT_MSG(false,"This Point shoud not be reached");
	return false;
}

