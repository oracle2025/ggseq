/* TLSampleManager.h
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

#ifndef _TLSAMPLEMANAGER_H_
#define _TLSAMPLEMANAGER_H_

class TLSample;
class TLColourManager;
class TiXmlElement;
class UpdateListener;

WX_DECLARE_LIST(TLSample, TLSampleList);

class TLSampleManager
{
	public:
		TLSampleManager();
		~TLSampleManager();
		TLSample *GetSample(wxString filename, UpdateListener* updateListener=NULL);
		TLSample *GetSample(int id); /*Fürs XML laden*/
		TLSample *AddSample(wxString filename, int id, UpdateListener* updateListener=NULL);
		void addXmlData(TiXmlElement *samples, bool relative=false, wxString tmp_path=wxT(""));
		void Clear(TLSample* tlSample);
		void ClearAll();
		TLColourManager *GetColourManager();
	private:
		wxString NormalizePath(wxString filename);
		TLSampleList m_sampleList;
		int m_MaxId;
		TLColourManager *m_colourMan;
};

#endif /*_TLSAMPLEMANAGER_H_*/
