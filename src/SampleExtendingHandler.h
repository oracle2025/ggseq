/* SampleExtendingHandler.h
 *
 *  Copyright (C) 2006 Richard Spindler <richard.spindler AT gmail.com>
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
#ifndef _SAMPLE_EXTENDING_HANDLER_
#define _SAMPLE_EXTENDING_HANDLER_

#include "MouseDragHandler.h"


class SampleExtendingHandler : public MouseDragHandler
{
	public:
		SampleExtendingHandler( wxWindow* canvas, TLItem *item, TLView *view, int x, int y );
		~SampleExtendingHandler();
		virtual void OnDrag( int x, int y );
		virtual void OnDrop( int x, int y, bool copy );
	private:
		TLView      *m_view;
		TLItem      *m_item;
		wxWindow    *m_canvas;
		int m_x;
		int m_track_y;
		long m_item_len;
		long m_item_end;
};


#endif /* _SAMPLE_EXTENDING_HANDLER_ */
