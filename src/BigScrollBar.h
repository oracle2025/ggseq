/* BigScrollBar.h
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
 
#ifndef _BIG_SCROLL_BAR_H_
#define _BIG_SCROLL_BAR_H_

class BigScrollBar : public wxScrollBar
{
	public:
		BigScrollBar(wxWindow* parent, wxWindowID id);
		void SetBigScrollBar(gg_tl_dat position, gg_tl_dat thumbSize, gg_tl_dat range);
		void SetBigThumbPosition(gg_tl_dat position);
		gg_tl_dat GetBigThumbPosition();
		gg_tl_dat GetBigThumbSize();
		gg_tl_dat GetBigRange();
	private:
		gg_tl_dat m_BigRange;
};

#endif /* _BIG_SCROLL_BAR_H_ */

