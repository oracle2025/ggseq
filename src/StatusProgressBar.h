/* StatusProgressBar.h
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
#ifndef _STATUSPROGRESSBAR_H_
#define _STATUSPROGRESSBAR_H_
class DisableListener;

class StatusProgressBar: public wxStatusBar, public UpdateListener
{
	public:
		StatusProgressBar(wxWindow* parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString& name = wxT("statusBar"));
		~StatusProgressBar();
		virtual bool Update(int status);
		virtual void StartUpdateProcess();
		virtual void EndUpdateProcess();
		void OnSize(wxSizeEvent &event);
		void OnCancelButton(wxCommandEvent &event);
		void SetDisableListener(DisableListener *disableListener);
	private:
		wxGauge *m_gauge;
		wxButton *m_cancelButton;
		bool m_continue;
		DisableListener *m_disableListener;
		DECLARE_EVENT_TABLE()
};

#endif /* _STATUSPROGRESSBAR_H_ */
