/* FileInfoPanel.h
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
 
#ifndef _FILE_INFO_PANEL_H_
#define _FILE_INFO_PANEL_H_

class FileInfoListener
{
	public:
		virtual void SetInfo(const wxString &filename, long frames, long channels, long sampleRate)=0;
};

class FileInfoPanel : public wxPanel, public FileInfoListener
{
	public:
		FileInfoPanel(wxWindow* parent);
		~FileInfoPanel();
		void SetInfo(const wxString &filename, long frames, long channels, long sampleRate);
	private:
		wxString GenerateLengthString(long frames, long sampleRate);
		wxSizer *InfoPanel( wxWindow *parent, bool call_fit = true, bool set_sizer = true );
		wxTextCtrl *m_fileNameTC;
		wxTextCtrl *m_lengthTC;
		wxTextCtrl *m_framesTC;
		wxTextCtrl *m_channelsTC;
		wxTextCtrl *m_sampleRateTC;
};

#endif /* _FILE_INFO_PANEL_H_ */
