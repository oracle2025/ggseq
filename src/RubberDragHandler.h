/* RubberDragHandler.h
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

#ifndef _RUBBER_DRAG_HANDLER_H_
#define _RUBBER_DRAG_HANDLER_H_

class RubberDragHandler : public MouseDragHandler
{
	public:
		RubberDragHandler( wxWindow* canvas, TLView *view, int xStart, int yStart, int x, int y );
		virtual void OnDrag( int x, int y );
		virtual void OnDrop( int x, int y, bool copy );
	private:
		wxWindow    *m_canvas;
		TLView      *m_view;
		wxRect       m_rubberFrame;
		bool         m_rubberFrameVisible;
	private:  /* - Helper Functions - */
		void NormalizeFrame();
		void HideFrame( wxDC& dc ); //TODO: in SampleDragHandler too
		void ShowFrame( wxDC& dc );
		void ToggleFrame( wxDC& dc );
};

#endif /* _RUBBER_DRAG_HANDLER_H_ */

