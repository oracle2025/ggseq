/* MouseDragHandler.h
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
#ifndef _MOUSE_DRAG_HANDLER_H_
#define _MOUSE_DRAG_HANDLER_H_

class TLItem;
class wxDragImage;


class MouseDragHandler
{
	public:
    virtual ~MouseDragHandler(){};
		virtual void OnDrag( int x, int y ) = 0;
		virtual void OnDrop( int x, int y, bool copy ) = 0;
};

class SampleDragHandler : public MouseDragHandler
{
  public:
    SampleDragHandler( wxWindow* canvas, TLItem *item, TLView *view, int x, int y );
    ~SampleDragHandler();
		virtual void OnDrag( int x, int y );
		virtual void OnDrop( int x, int y, bool copy );
  private:
    wxDragImage *GenerateDragImg( TLItem *item, int width );
    void         ToggleFrame( wxDC& dc );
    void         ShowFrame( wxDC& dc );
    void         HideFrame( wxDC& dc );
    wxWindow    *m_canvas;
    TLItem      *m_item;
    wxDragImage *m_dragImage;
    wxRect       m_dragImagePos;
    wxRect       m_dragFramePos;
    TLView      *m_view;
    int          m_xOffset;
    int          m_yOffset;
    bool         m_frameVisible;
};


#endif /* _MOUSE_DRAG_HANDLER_H_ */

