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
#include "GgseqDocManager.h"


EnvelopeDragHandler::EnvelopeDragHandler( wxWindow* canvas, TLView *view, TLData *data,
	TLItem *item, int x, int y, wxRect *envelopeHandle )
{//TODO Get all Envelope Data, Generate new Envelope Data, and submit everything back
	/*m_envelopeHandle = 0;
	for ( int i = 0; i < 4; i++ ) {
		m_fades[i] = item->m_guiEnvData[i];
		if ( envelopeHandle == (&(item->m_guiEnvData[i])) ) {
			m_envelopeHandle = &m_fades[i];
		}
	}
	if (!m_envelopeHandle) {
		wxLogError(wxT("Error "));
		return;
	}*/
	m_envelopeHandle = envelopeHandle;
	wxASSERT(m_envelopeHandle)
	m_canvas = canvas;
	m_data = data;
	m_view = view;
	m_item = item;
	//m_envelopeHandle = envelopeHandle;
	m_itemBoundaries = m_view->GetItemBoundaries( item );
	//Start Position for Drag (for Undo Redo)
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
/*void EnvelopeDragHandler::GuiEnvToDataEnv( wxRect* fades, EnvelopePoint* realEnv )
{
	realEnv[0].x = 0;
	realEnv[0].y = float(18 - fades[0].y) / 18.0; //Trackhöhe;
	realEnv[1].x = int(fades[1].x * ( 117600.0 / 31.0 ) );
	realEnv[1].y = float(18 - fades[1].y) / 18.0;
	realEnv[2].x = int(fades[2].x * ( 117600.0 / 31.0 ) );
	realEnv[2].y = float(18 - fades[2].y)  / 18.0;
	realEnv[3].x = m_item->GetLength();//int(m_rightFadeOut.x * ( 117600.0 / 31.0 ) );
	realEnv[3].y = float(18 - fades[3].y)  / 18.0;
}*/
void EnvelopeDragHandler::OnDrop( int x, int y, bool copy )
{
	NativeEnvData nativeEnvelope;
	m_item->GetNewEnvelopeData( nativeEnvelope, m_envelopeHandle );
//	GetEnvFromRects( nativeEnvelope, m_fades, m_item->GetLength());
	m_canvas->ReleaseMouse();
	g_ggseqProps.GetDocManager()->SubmitCommand(
			new GgseqEnvelopeItemCommand(
				m_data, m_item,
				nativeEnvelope
				)
			);

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
void EnvelopeDragHandler::FixEnvelopeCtrls()//TODO: Clean up the Mess
{
	m_item->DragEnvelopeHandle( m_envelopeHandle );
/*	if ( m_envelopeHandle == (&(m_fades[0])) ) {
		m_envelopeHandle->x = 0;
	}
	if ( m_envelopeHandle == (&(m_fades[3])) ) {
		m_envelopeHandle->x = m_itemBoundaries.width-7;
	}
	if ( m_envelopeHandle == (&(m_fades[1])) ) {
		m_fades[2].y = m_envelopeHandle->y;
		if (m_fades[2].x < m_fades[1].x ) {
			m_fades[2].x = m_fades[1].x;
		}
	}
	if ( m_envelopeHandle == (&(m_fades[2])) ) {
		m_fades[1].y = m_envelopeHandle->y;
		if (m_fades[2].x < m_fades[1].x ) {
			m_fades[1].x = m_fades[2].x;
		}
	}*/
}
void EnvelopeDragHandler::Draw( wxDC &dc )
{
	dc.SetDeviceOrigin( m_itemBoundaries.x, m_itemBoundaries.y );
	m_item->Draw( dc, g_ggseqProps.GetZoom() );
	dc.SetDeviceOrigin( 0, 0 );
	
/*	m_view->Draw3dRect(
		&dc, m_itemBoundaries.x, m_itemBoundaries.y,
		m_itemBoundaries.width, m_itemBoundaries.height,
		m_item->GetSample()->GetColour()
		);
	
	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( *wxWHITE_BRUSH );*/
	m_item->DrawEnvelope( dc, m_itemBoundaries.x, m_itemBoundaries.y, false );
}
