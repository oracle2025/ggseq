/* BigScrollBar.cpp
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

#include "stuff.h"
#include "BigScrollBar.h"
#define SCROLLBAR_LENGTH 100000

BigScrollBar::BigScrollBar(wxWindow* parent, wxWindowID id) : wxScrollBar(parent, id)
{ m_BigRange=SCROLLBAR_LENGTH; }

void BigScrollBar::SetBigScrollBar(gg_tl_dat position, gg_tl_dat thumbSize, gg_tl_dat range)
{
	m_BigRange = range;
	int sbPosition = (int) ( (position*SCROLLBAR_LENGTH)/m_BigRange );
	int sbThumbSize = (int) ( (thumbSize*SCROLLBAR_LENGTH)/m_BigRange );
	SetScrollbar(sbPosition, sbThumbSize, SCROLLBAR_LENGTH, sbThumbSize);
}

void BigScrollBar::SetBigThumbPosition(gg_tl_dat position)
{
	int sbPosition = (int) ( (position*SCROLLBAR_LENGTH)/m_BigRange );
	SetThumbPosition(sbPosition);
}
gg_tl_dat BigScrollBar::GetBigThumbPosition()
{
	gg_tl_dat result = (((gg_tl_dat)GetThumbPosition())*m_BigRange)/SCROLLBAR_LENGTH;
	return result;
}
gg_tl_dat BigScrollBar::GetBigThumbSize()
{
	gg_tl_dat result = (((gg_tl_dat)GetThumbSize())*m_BigRange)/SCROLLBAR_LENGTH;
	return result;
}
gg_tl_dat BigScrollBar::GetBigRange()
{ return m_BigRange; }

