/* RubberDragHandler.cpp
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
#include "TLView.h"
#include "MouseDragHandler.h"
#include "RubberDragHandler.h"

void SetRubberFramePen( wxDC& dc ); // Is in MouseDragHandler.cpp

RubberDragHandler::RubberDragHandler( wxWindow* canvas, TLView *view, int xStart, int yStart, int x, int y )
{
	m_canvas             = canvas;
	m_view               = view;
	m_rubberFrameVisible = false;
	m_rubberFrame = wxRect( xStart, yStart, x - xStart, y - yStart );
	m_canvas->CaptureMouse(); //FIXME Wirklich nötig?
	m_view->ClearSelection();
	m_canvas->Refresh();
}
void RubberDragHandler::OnDrag( int x, int y )
{
	wxClientDC dc( m_canvas );
	HideFrame( dc );
	m_rubberFrame.width = x - m_rubberFrame.x;
	m_rubberFrame.height = y - m_rubberFrame.y;
	ShowFrame( dc );
}
void RubberDragHandler::OnDrop( int x, int y, bool copy )
{
	m_canvas->ReleaseMouse();
	m_rubberFrame.width = x - m_rubberFrame.x;
	m_rubberFrame.height = y - m_rubberFrame.y;
	wxClientDC dc( m_canvas );
	//HideFrame( dc );
	NormalizeFrame();
	m_view->Select(
		m_rubberFrame.x, m_rubberFrame.y,
		m_rubberFrame.width, m_rubberFrame.height
		);
	m_canvas->Refresh();
}
void RubberDragHandler::NormalizeFrame()
{
	if ( m_rubberFrame.width < 0 ) {
		m_rubberFrame.width =- m_rubberFrame.width;
		m_rubberFrame.x = m_rubberFrame.x - m_rubberFrame.width;
	}
	if ( m_rubberFrame.height < 0 ) {
		m_rubberFrame.height =- m_rubberFrame.height;
		m_rubberFrame.y = m_rubberFrame.y - m_rubberFrame.height;
	}

}
void RubberDragHandler::ToggleFrame( wxDC& dc )
{
  SetRubberFramePen( dc );
  dc.DrawRectangle( m_rubberFrame.x, m_rubberFrame.y,
      m_rubberFrame.width, m_rubberFrame.height );
}
void RubberDragHandler::ShowFrame( wxDC& dc )
{
  ToggleFrame( dc );
  m_rubberFrameVisible = true;
}
void RubberDragHandler::HideFrame( wxDC& dc )
{
  if ( m_rubberFrameVisible )
    ToggleFrame( dc );
  m_rubberFrameVisible = false;
}

