/* EnvelopeDragHandler.cpp
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
#include "TLPanel.h"
#include "TLItem.h"
#include "TLSample.h"
#include "MouseDragHandler.h"
#include "EnvelopeDragHandler.h"


EnvelopeDragHandler::EnvelopeDragHandler( wxWindow* canvas, TLView *view,
	TLItem *item, int x, int y, wxRect *envelopeHandle )
{
	m_canvas = canvas;
	m_view = view;
	m_item = item;
	m_envelopeHandle = envelopeHandle;
	m_itemBoundaries = m_view->GetItemBoundaries( item );

	m_canvas->CaptureMouse(); //TODO Put me in Base Class
	
	m_xOffset = x - ( m_itemBoundaries.x + m_envelopeHandle->x );
	m_yOffset = y - ( m_itemBoundaries.y + m_envelopeHandle->y );
}
void EnvelopeDragHandler::OnDrag( int x, int y )
{
	wxClientDC dc( m_canvas );
	wxPoint handlePos = wxPoint( x - m_xOffset, y - m_yOffset );
	handlePos = FitInside( handlePos );
	m_envelopeHandle->x = handlePos.x - m_itemBoundaries.x;
	m_envelopeHandle->y = handlePos.y - m_itemBoundaries.y;
	FixEnvelopeCtrls();

	Draw( dc );
}
void EnvelopeDragHandler::OnDrop( int x, int y, bool copy )
{
	m_item->GuiEnvToDataEnv();
	m_canvas->ReleaseMouse();
	//TODO Calculate Real Envelope Data
}
wxPoint EnvelopeDragHandler::FitInside( wxPoint handlePos )
{
	if ( handlePos.x < m_itemBoundaries.x )
		handlePos.x = m_itemBoundaries.x;
	else if ( handlePos.x + 7 > m_itemBoundaries.GetRight() )
		handlePos.x = m_itemBoundaries.GetRight() - 7;
	if ( handlePos.y < m_itemBoundaries.y )
		handlePos.y = m_itemBoundaries.y;
	else if ( handlePos.y + 7 > m_itemBoundaries.GetBottom() )
		handlePos.y = m_itemBoundaries.GetBottom() - 7;
	return handlePos;
}
void EnvelopeDragHandler::FixEnvelopeCtrls()
{
	if ( m_envelopeHandle == (&(m_item->m_leftFadeIn)) ) {
		m_envelopeHandle->x = 0;
	}
	if ( m_envelopeHandle == (&(m_item->m_rightFadeOut)) ) {
		m_envelopeHandle->x = m_itemBoundaries.width-7;
	}
	if ( m_envelopeHandle == (&(m_item->m_rightFadeIn)) ) {
		m_item->m_leftFadeOut.y = m_envelopeHandle->y;
		if (m_item->m_leftFadeOut.x < m_item->m_rightFadeIn.x ) {
			m_item->m_leftFadeOut.x = m_item->m_rightFadeIn.x;
		}
	}
	if ( m_envelopeHandle == (&(m_item->m_leftFadeOut)) ) {
		m_item->m_rightFadeIn.y = m_envelopeHandle->y;
		if (m_item->m_leftFadeOut.x < m_item->m_rightFadeIn.x ) {
			m_item->m_rightFadeIn.x = m_item->m_leftFadeOut.x;
		}
	}
}
void EnvelopeDragHandler::Draw( wxDC &dc )
{

	m_view->Draw3dRect(
		&dc, m_itemBoundaries.x, m_itemBoundaries.y,
		m_itemBoundaries.width, m_itemBoundaries.height,
		m_item->GetSample()->GetColour()
		);
	
	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( *wxWHITE_BRUSH );
//	dc.DrawRectangle( m_itemBoundaries.x + m_item->m_x_test, m_itemBoundaries.y + m_item->m_y_test, 10, 10);
	m_item->DrawEnvelope( dc, m_itemBoundaries.x, m_itemBoundaries.y );
}
