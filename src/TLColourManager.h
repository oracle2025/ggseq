/* TLColourManager.h
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

#ifndef _TLCOLORMANAGER_H_
#define _TLCOLORMANAGER_H_

class TLDirColour;

WX_DECLARE_LIST(TLDirColour, TLDirColourList);
class TLDirColour
{
	public:
		TLDirColour(wxString dir, wxColour colour);
		wxString m_directory;
		wxColour m_colour;
};

class TLColourManager
{
	public:
		TLColourManager();
		~TLColourManager();
		wxColour GetColour(wxString dirname);
		void SetColour(wxString dirname, wxColour colour);
		TLDirColourList::Node *GetColours();
		void Clear();
	private:
		TLDirColourList m_colourList;
};

#endif /*_TLCOLORMANAGER_H_*/
