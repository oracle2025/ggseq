/* WaveEditor.h
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

#ifndef _WAVE_EDITOR_H_
#define _WAVE_EDITOR_H_

#ifdef __GNUG__
    #pragma interface "WaveEditor.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


class TLItem;

class WaveEditor : public wxPanel
{
	public:
		// constructors and destructors
		WaveEditor( wxWindow* parent, TLItem *item, wxWindowID id = -1 );
		virtual ~WaveEditor();
		// method declarations
	private:
		// member variable declarations
		TLItem *m_item;
	private:
		// handler declarations
		void OnPaint( wxPaintEvent& event );
		void OnEraseBackground( wxEraseEvent& event );
	private:
		DECLARE_EVENT_TABLE()
};

#endif /* _WAVE_EDITOR_H_ */

