/* EnvelopeDragHandler.h
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

#ifndef _ENVELOPE_DRAG_HANDLER_H_
#define _ENVELOPE_DRAG_HANDLER_H_

class EnvelopeDragHandler : public MouseDragHandler
{
	public:
		EnvelopeDragHandler (
			wxWindow* canvas, TLView *view,
			TLItem *item, int x, int y, wxRect *envelopeHandle 
			);
		virtual void OnDrag( int x, int y );
		virtual void OnDrop( int x, int y, bool copy );
	private:
		wxWindow    *m_canvas;
		TLView      *m_view;
		TLItem      *m_item;
		int          m_xOffset;
		int          m_yOffset;
		wxRect       m_itemBoundaries;
		wxRect      *m_envelopeHandle;
	private: /* Helper Functions */
		wxPoint      FitInside( wxPoint handlePos );
		void         Draw( wxDC &dc );
		void         FixEnvelopeCtrls();
};

#endif /* _ENVELOPE_DRAG_HANDLER_H_ */
