/* DisableListener.h
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
#ifndef _DISABLELISTENER_H_
#define _DISABLELISTENER_H_

class DisableListener
{
	public:
		virtual void DisableAll()=0;
		virtual void EnableAll()=0;
};

class DoubleDisabler: public DisableListener
{
	public:
		DoubleDisabler(wxWindow* window1, wxWindow* window2);
		void DisableAll();
		void EnableAll();
	private:
		wxWindow* m_window1;
		wxWindow* m_window2;
};

#endif /* _DISABLELISTENER_H_ */
