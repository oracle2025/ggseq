/* Listeners.h
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

#ifndef _LISTENERS_H_
#define _LISTENERS_H_
/*
class SnapChangeListener
{
	public:
		virtual void SnapChanged()=0;
};

class ZoomChangeListener
{
	public:
		virtual void ZoomChanged()=0;
};

class ScrollChangeListener
{
	public:
		virtual void ScrollChanged()=0;
};*/

class UndoRedoChangeListener
{
	public:
		virtual void UndoRedoChanged()=0;
};
/* EmitUndoRedoChange() */

#endif /* _LISTENERS_H_ */
