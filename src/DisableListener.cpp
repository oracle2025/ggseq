/* DisableListener.cpp
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

#include "DisableListener.h"

DoubleDisabler::DoubleDisabler(wxWindow* window1, wxWindow* window2)
{
	m_window1=window1;
	m_window2=window2;
}
void DoubleDisabler::DisableAll()
{
	m_window1->Enable(false);
	m_window2->Enable(false);
}
void DoubleDisabler::EnableAll()
{
	m_window1->Enable(true);
	m_window2->Enable(true);
}



