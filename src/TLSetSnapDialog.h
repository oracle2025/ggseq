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
#ifndef _TLSETSNAPDIALOG_H_
#define _TLSETSNAPDIALOG_H_

class wxSpinEvent;
class wxSpinCtrl;

class TLSetSnapDialog: public wxDialog
{
	public:
		TLSetSnapDialog(wxWindow* parent, wxWindowID id,int SnapPosition, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
		~TLSetSnapDialog();
//		void OnHipHopButton(wxCommandEvent &event);
//		void OnOldSkoolButton(wxCommandEvent &event);
//		void OnFunkButton(wxCommandEvent &event);
		void OnSpin(wxSpinEvent &event);
		void OnSecondsText(wxCommandEvent &event);
		void OnBpmText(wxCommandEvent &event);

		void OnAddButton(wxCommandEvent &event);
		void OnDeleteButton(wxCommandEvent &event);
		void OnPresetNameText(wxCommandEvent &event);
		
		void OnPresetsList(wxCommandEvent &event);
		
		void OnClose(wxCloseEvent &event);
		wxString m_SnapSize;
		wxString m_bpmValue;
		int m_SnapPosition; //int m_frameSnapValue; /*Als Zentraler Wert*/
		wxSpinCtrl *m_FramesSpinCtrl;
		wxTextCtrl *m_SecondsTextCtrl;
		wxTextCtrl *m_bpmTextCtrl;
		wxListBox *m_presetsListBox;
		wxTextCtrl *m_presetNameText;
	private:
		void Save();
		void Load();
		void Select(wxString str, int length);
		void Modify(wxString str);
		void Modify(int length);
		bool m_Lock;
		void FromFramesToSeconds(int Frames=-1);
		void FromFramesToBpm();
		//void UpdateDisplaySeconds();
		//void UpdateDisplayFrames(int Frames); ??
		//void UpdateDisplayBpm();
		//void SetFrameSnapFromSeconds(double Seconds)
		//void SetFrameSnapFromBpm(double Bpm)
		DECLARE_EVENT_TABLE()

};
#define ID_TEXT 10000
#define ID_SPINCTRL 10001
#define ID_TEXTCTRL 10002
#define ID_BUTTON 10003
#define ID_LINE 10004
wxSizer *MyDialogFunc( TLSetSnapDialog *parent, bool call_fit = TRUE, bool set_sizer = TRUE );
#endif /*_TLSETSNAPDIALOG_H_*/
