/* TLSample.h
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

#ifndef _TLSAMPLE_H_
#define _TLSAMPLE_H_

class wxString;
class TLColourManager;
class UpdateListener;

class TLSample
{
	public:
		TLSample(const wxString &filename, int id,TLColourManager *colourMan, UpdateListener* updateListener=NULL);
		~TLSample();
		float *GetBuffer();
		gg_tl_dat GetLength();
		bool IsValid();
		void Ref();
		void UnRef();
		int GetRefCount();
		int GetId();
		wxString GetFilename();
		wxIcon GetIcon();
		void Draw(wxDC& dc_screen);
		wxColour GetColour();
		
		gg_tl_dat m_infoFrames;
		long m_infoSampleRate;
		long m_infoChannels;
	private:
		int m_id; /*Für die Zuordnung im XML-File*/
		int m_refCount;/*Ums zu löschen sobald es keine Referenzen mehr gibt.*/
		gg_tl_dat m_bufferLength;
		float *m_buffer;
		wxString m_filename;
		bool m_valid;
		TLColourManager *m_colourMan;
		wxColour m_colour;
};

#endif /*_TLSAMPLE_H_*/
