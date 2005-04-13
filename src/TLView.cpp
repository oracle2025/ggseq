/* TLView.cpp
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

#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/config.h>
#include <wx/tglbtn.h>

#include <iostream>

#include "stuff.h"
#include "TLItem.h"
#include "TLTrack.h"
#include "TLView.h"
#include "TLData.h"
#include "TLSample.h"
#include "TLSelectionSet.h"
#include "GgseqDocManager.h"
#include "gun_girl.xpm"
#include "drop_down_mini.xpm"
#include "TLMuteButton.h"
#include "dial.h"
#include "TLTrackVolumeDial.h"
#include "colour_helper.h"

#define SNAP_POSITION 117600

#define LEFT_OFFSET_TRACKS 52
#define TOP_OFFSET_TRACKS 3 //22

#define VIEW_LEFT_BORDER  57
#define VIEW_RIGHT_BORDER  5
#define VIEW_TOP_BORDER    3

//wxIcon *dropDownIcon;
wxIcon dropDownIcon(drop_down_mini_xpm);

TLView::TLView(TLData *TlData)
{
	g_ggseqProps.SetView( this );
	m_TlData          = TlData;
	m_TrackDrawDist   = 5;
	m_YscrollPosition = 0;
	m_SnapSuspended   = false;
	m_zoom            = 1.0;
	m_selectionSet    = new TLSelectionSet();
	m_gungirl         = new wxIcon( gun_girl_xpm );
	m_drop_down_mini  = new wxIcon( drop_down_mini_xpm );
	m_docManager      = new GgseqDocManager( m_TlData );
	m_PositionVisible = 0;
	m_width           = 200;
	m_height          = 200;
	m_TlData->SetDocManager( m_docManager );
	{
		wxConfigBase *conf = wxConfigBase::Get();
		conf->SetPath( wxT("/") );
		m_TlData->SetSnapValue( conf->Read( wxT("SnapPosition"), SNAP_POSITION ) );
	}
//	dropDownIcon = new wxIcon(drop_down_mini_xpm);
}
TLView::~TLView()
{
	{
		wxConfigBase *conf = wxConfigBase::Get();
		conf->Write( wxT("SnapPosition"), m_TlData->GetSnapValue() );
	}
	delete m_selectionSet;
	delete m_TlData;
	delete m_gungirl;
	delete m_drop_down_mini;
	delete m_docManager;
//	delete dropDownIcon;
}


void TLView::SuspendSnap() { m_SnapSuspended = true; }
void TLView::ResumeSnap() { m_SnapSuspended = false; }
void TLView::SetZoom( float zoom ) { m_zoom = zoom; }
void TLView::SetSnapValue(long snapValue)
	{ m_TlData->SetSnapValue( snapValue ); }
long TLView::GetSnapValue()
	{ return m_TlData->GetSnapValue(); }
float TLView::GetRealZoom()
	{ return HARD_ZOOM/m_zoom; }
void TLView::SetSize( long width, long height )
	{ m_width = width; m_height = height; }
gg_tl_dat TLView::VisibleLength()
{
	return (gg_tl_dat)( ( m_width - VIEW_LEFT_BORDER - VIEW_RIGHT_BORDER ) * GetRealZoom() );
}
void TLView::SetPosition(gg_tl_dat Position)
	{ m_PositionVisible = (gg_tl_dat) ( Position * GetRealZoom() ); }
gg_tl_dat TLView::GetPosition()
	{ return (gg_tl_dat) ( m_PositionVisible / GetRealZoom() ); }
gg_tl_dat TLView::GetScrollBarRange()
	{ return (gg_tl_dat) ( m_TlData->GetLength() / GetRealZoom() ); }
gg_tl_dat TLView::GetScrollBarThumbSize()
	{ return m_width - VIEW_LEFT_BORDER - VIEW_RIGHT_BORDER; }

void TLView::SetBrushColour( wxDC& dc, wxColour colour )
{
	wxBrush brush1 = dc.GetBrush();
	brush1.SetColour( colour );
	dc.SetBrush( brush1 );
}
void TLView::Draw( wxDC& dc )
{
	long yoffset;
	dc.SetBrush( *wxGREY_BRUSH );
	SetBrushColour( dc, wxColour( 166, 166, 166 ) );
	dc.SetPen( wxPen( wxColour( 100, 100, 100), 1, wxSOLID ) );
	yoffset = TOP_OFFSET_TRACKS;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *track = node->GetData();
		if (track->GetSelected()) {
			SetBrushColour( dc, wxColour( 199, 199, 188 ) );
		}
		dc.DrawRectangle( VIEW_LEFT_BORDER,
		                  yoffset - m_YscrollPosition,
		                  m_width - VIEW_LEFT_BORDER - VIEW_RIGHT_BORDER,
		                  track->GetHeight() );
		yoffset += track->GetHeight() + m_TrackDrawDist;
		if (track->GetSelected()) {
			SetBrushColour( dc, wxColour( 166, 166, 166 ) );
		}
	}
	yoffset = TOP_OFFSET_TRACKS;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		yoffset = DrawTrack( dc, yoffset, current );
	}
}
void TLView::UpdateDialsAndButtons()
{
	int i = TOP_OFFSET_TRACKS - m_YscrollPosition ;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->m_muteButton->SetSize( 5, i, 25, 25 );
		current->m_volumeDial->SetSize( 31, i, 25, 25 );
		i += 30;
	}
}
void TLView::DrawItem(wxDC& dc, TLItem* item, long left, long delta_left, long top, long width, long height)
{
/* - Item itself - */
	Draw3dRect( &dc, left, top, width, height, item->GetSample()->GetColour() );
/* - Selection - */
	if ( item->IsSelected() ) {
		dc.SetPen( wxPen( *wxWHITE, 3, wxSHORT_DASH ) );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle( left, top, width, height );
	}
/* - Label (Filename) - */
	if ( width > 1 ) {
		dc.SetPen( *wxBLACK_PEN );
		dc.SetClippingRegion( left, top, width, height );
		dc.SetFont( *wxSMALL_FONT );
		wxFileName fn( item->GetSample()->GetFilename() );
		long v3 = left + delta_left;
			dc.DrawText( fn.GetName(), v3, top + 1 );
/* - Extras ;) -*/ 
		dc.SetPen( *wxBLACK_PEN );
		dc.SetBrush( *wxWHITE_BRUSH );
		if ( item->m_toggleEnvelope ) {
			item->DrawEnvelope( dc, left + delta_left, top );
		}
		dc.DrawIcon(dropDownIcon, left+delta_left+(item->GetLen()/GetRealZoom())-7,top+height-7);
		dc.DestroyClippingRegion();
	}
//	dc.DrawRectangle( left + item->m_x_test, top+item->m_y_test, 10, 10);
}

bool TLView::ItemVisible( TLItem* item )
{
	gg_tl_dat i_left = item->GetPosition();
	gg_tl_dat i_right = item->GetEndPosition();
	gg_tl_dat vis_right = m_PositionVisible + VisibleLength();
	gg_tl_dat vis_left = m_PositionVisible;
	if ( (i_left  >= vis_left && i_left  <= vis_right) ||
	     (i_right >= vis_left && i_right <= vis_right) || 
	     (i_left  <= vis_left && i_right >= vis_right) )
		return true;
	return false;
}
/*
bool TLView::ItemPosOnScreen(TLItem* item)
{
}
*/
long TLView::DrawTrack(wxDC& dc, long yoffset, TLTrack* track)
{
	gg_tl_dat start, end, delta_start;
	for ( TLItemList::Node *node = track->GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData();
		if ( !ItemVisible( current ) )
			continue;
		start = current->GetPosition() - m_PositionVisible;
		delta_start = 0;
		if ( start < 0 ) {
			delta_start = start;
			start = 0;
		}
		end = current->GetEndPosition() - m_PositionVisible;
		if ( end > VisibleLength() )
			end = VisibleLength();
		long left       = (long) ( start /  GetRealZoom() ) + VIEW_LEFT_BORDER;
		long delta_left = (long) ( delta_start / GetRealZoom() );
		long top        = yoffset - m_YscrollPosition;
		long width      = (long) ( ( end - start ) /  GetRealZoom() );
		long height     = track->GetHeight();
		DrawItem( dc, current, left, delta_left, top, width, height );
	}
	return yoffset + track->GetHeight() + m_TrackDrawDist;
}
void TLView::Draw3dRect(wxDC *dc, wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxColour colour)
{
/*	wxBrush b1=dc->GetBrush();
	b1.SetColour(colour);*/
	dc->SetBrush(wxBrush(colour, wxSOLID));
	dc->SetPen(*wxTRANSPARENT_PEN);
	dc->DrawRectangle(x,y,width,height);
/*	wxPen pen1=dc->GetPen();
	pen1.SetColour(GetLightColour(colour));*/
	dc->SetPen(wxPen(GetLightColour(colour), 1, wxSOLID ));
	dc->DrawLine(x,y,x+width-1,y);
	dc->DrawLine(x,y,x,y+height-1);
/*	pen1=dc->GetPen();
	pen1.SetColour(GetDarkColour(colour));*/
	dc->SetPen(wxPen(GetDarkColour(colour), 1, wxSOLID ));
	dc->DrawLine(x,y+height-1,x+width-1,y+height-1);
	dc->DrawLine(x+width-1,y,x+width-1,y+height);
}
void TLView::AddItem( TLSample *sample, long position, long trackNr )
{
	if (m_TlData->IsBlocked())
		return;
	m_docManager->SubmitCommand(
		new GgseqAddItemCommand(
			m_TlData, sample->GetFilename(),
			GetSnap( FromScreenXtoTL( position ) ),
			trackNr, sample
			)
		);
}

void TLView::AddItem(wxString filename, long position, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
	m_docManager->SubmitCommand(
		new GgseqAddItemCommand(
			m_TlData, filename,
			GetSnap( FromScreenXtoTL( position ) ),
			trackNr
			)
		);
}

void TLView::DeleteItem(TLItem *item )
{
	if (m_TlData->IsBlocked())
		return;
	m_docManager->SubmitCommand( new GgseqDeleteItemCommand( m_TlData, item ) );
}

gg_tl_dat TLView::FromScreenXtoTL(long x)
{
	gg_tl_dat result = (gg_tl_dat)( ( x - VIEW_LEFT_BORDER ) * GetRealZoom() ) + m_PositionVisible;
	return result;
}
long TLView::FromTLtoScreenX(gg_tl_dat x)
{
	double result = (double)(x-m_PositionVisible) / GetRealZoom() + VIEW_LEFT_BORDER;
	return (long) result;
}
long TLView::GetTrackByY(long y)
{
	long y_offset = TOP_OFFSET_TRACKS-m_YscrollPosition;
	long cnt = 0;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		if ( y > y_offset && y < current->GetHeight() + y_offset )
			return cnt;
		cnt++;
		y_offset += current->GetHeight() + m_TrackDrawDist;
	}
	return -1;
}
wxRect TLView::GetItemBoundaries(TLItem *item)
{
  	static int x, y, width, height;
	x      = FromTLtoScreenX( item->GetPosition() );
  	y      = item->GetTrack() * 30 + TOP_OFFSET_TRACKS - GetYScrollPosition();
	width  = (int)( item->GetLen() / GetRealZoom() );
	height = 25;
	return wxRect( x, y, width, height );
}
TLItem *TLView::GetDragItem( long x, long y )
{
	if (m_TlData->IsBlocked())
		return NULL;
	long track = GetTrackByY(y);
	if ( track < 0 )
		return NULL;
	return m_TlData->ItemAtPos( FromScreenXtoTL( x ), track );
}
void TLView::DoDrop( long x, long y, TLItem *item, long x_offset, bool copy )
{
	if ( m_TlData->IsBlocked() )
		return;
	long track = GetTrackByY( y );
	if ( x + x_offset < VIEW_LEFT_BORDER || x + x_offset > m_width - VIEW_RIGHT_BORDER )
		track = -1;
	/*Delete Item*/
	if ( track < 0 && !copy ) {
		DeleteItem( item );
		return;
	} else if ( track < 0 )
		return;
	if ( copy ) {
		AddItem( item->GetSample(), x, track );
	} else {
		m_docManager->SubmitCommand(
			new GgseqMoveItemCommand(
				m_TlData, item,
				GetSnap( FromScreenXtoTL( x ) ),
				track
				)
			);
	}
}
gg_tl_dat TLView::GetSnap( gg_tl_dat x )
{
	if ( m_SnapSuspended )
		return x;
	gg_tl_dat pos = x % (gg_tl_dat)m_TlData->GetSnapValue();
	if ( pos ) {
		if ( pos > (gg_tl_dat)m_TlData->GetSnapValue() / 2 )
			x = x - pos + (gg_tl_dat)m_TlData->GetSnapValue();
		else
			x = x - pos;
	}
	//TODO: Snap To Sample implementieren
	return x;
}

void TLView::SetPlaybackPosition( long Position )
{
	if ( m_TlData->IsBlocked() )
		return;
	m_TlData->SetPlaybackPosition( FromScreenXtoTL( Position ) );
}
TLSample *TLView::GetSample( long position, long trackNr )
{
	TLItem *item = m_TlData->ItemAtPos( FromScreenXtoTL( position ), trackNr );
	if ( !item )
		return NULL;
	return item->GetSample();
}

long TLView::GetScreenSnapPosition( long position )
{	
	gg_tl_dat pos1 = FromScreenXtoTL( position );
	pos1 = GetSnap( pos1 );
	return FromTLtoScreenX( pos1 );
}

long TLView::GetCaretPosition()
{
	return FromTLtoScreenX( m_TlData->GetPlaybackPosition() );	
}

long TLView::GetCaretPosition( gg_tl_dat Position )
{
	return FromTLtoScreenX( Position );	
}
void TLView::Select( long x, long y, long width, long height )
{
	ClearSelection();
	int i = 0;
	int firstTrackNr;
	int scndTrackNr;
	GetTracksSurroundedBy( firstTrackNr, scndTrackNr, y, y + height );
	gg_tl_dat TL_X1 = FromScreenXtoTL( x );
	gg_tl_dat TL_X2 = FromScreenXtoTL( x + width );
	for ( TLTrackList::Node *trackNode = m_TlData->GetFirst(); trackNode; trackNode = trackNode->GetNext() ) {
		if ( !( i >= firstTrackNr && i <= scndTrackNr ) ) {
			i++;
			continue;
		}
		TLTrack *track = trackNode->GetData();
		for ( TLItemList::Node *node = track->GetFirst(); node; node = node->GetNext() ) {
			TLItem *current = node->GetData();
			if ( current->GetPosition()    <= TL_X2 &&
			     current->GetEndPosition() >= TL_X1 ) {
				m_selectionSet->AddSample( current->GetSample(), current );
			}
		}
		i++;
	}
	m_selectionSet->CalcBounds();
}

void TLView::ClearSelection()
{
	m_selectionSet->Clear( m_TlData );
}

void TLView::GetTracksSurroundedBy( int &trackNr1, int &trackNr2, long y1, long y2 )
{
	trackNr1 = -1;
	trackNr2 = -1;
	long y_offset = TOP_OFFSET_TRACKS - m_YscrollPosition;
	int cnt = 0;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		if ( trackNr1 < 0 )
			if ( y_offset + current->GetHeight() > y1 )
				trackNr1 = cnt;
		if ( y_offset < y2 )
			trackNr2 = cnt;
		cnt++;
		y_offset += current->GetHeight() + m_TrackDrawDist;
	}
}
bool TLView::IsSelectionAt( int x, int y, int& x_offset, int& y_offset, int& width, int& height )
{
	if ( m_TlData->IsBlocked() )
		return false;
	if ( !m_selectionSet->IsActive() )
		return false;
	gg_tl_dat TL_x = FromScreenXtoTL( x );
	int track = GetTrackByY( y );

	width = FromTLtoScreenX( m_selectionSet->GetX2() )
		- FromTLtoScreenX( m_selectionSet->GetX1() );
	height = ( m_selectionSet->GetTrack2() - m_selectionSet->GetTrack1() + 1 ) * 30 - 5;
	if ( track >= m_selectionSet->GetTrack1() &&
	     track <= m_selectionSet->GetTrack2() &&
	     TL_x  >= m_selectionSet->GetX1() &&
	     TL_x  <= m_selectionSet->GetX2()) {
		x_offset = x - FromTLtoScreenX( m_selectionSet->GetX1() );
		y_offset = y - ( m_selectionSet->GetTrack1() * 30 + 5 );
		return true;
	}
	return false;
}
void TLView::DrawSelection( wxDC *dc )
{
	for ( TLSelItemList::Node *node = m_selectionSet->GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData()->GetItem();
		dc->DrawRectangle( (int) ( ( current->GetPosition() - m_selectionSet->GetX1() ) /  GetRealZoom() ),
		  ( current->GetTrack() - m_selectionSet->GetTrack1() ) * 30,
		  (int)( current->GetLen() /  GetRealZoom() ), 25);
	}
}
void TLView::EndSelectionDrag(int x, int y, bool copy, long x_offset)
{
	gg_tl_dat TL_x = FromScreenXtoTL(x);
	TL_x = GetSnap( TL_x );
	int track = GetTrackByY(y);
	if ( x + x_offset < VIEW_LEFT_BORDER || x + x_offset > m_width - VIEW_RIGHT_BORDER )
		track = -1;
	TLSelectionSet *SelSet;
	if ( track >= 0 && copy ) {
		m_docManager->SubmitCommand(
			new GgseqAddItemsCommand(
				m_TlData, m_selectionSet,
				&SelSet, TL_x, track
				)
			);
		delete m_selectionSet;
		m_selectionSet = SelSet;
	}
	if ( track >= 0 && !copy ) {
		m_docManager->SubmitCommand(
			new GgseqMoveItemsCommand(
				m_TlData, m_selectionSet,
				&SelSet, TL_x, track
				)
			);
		delete m_selectionSet;
		m_selectionSet = SelSet;
	}
	if ( track < 0 and !copy ) {
		m_docManager->SubmitCommand(
			new GgseqDeleteItemsCommand( m_TlData, m_selectionSet )
			);
	}
}


void TLView::Undo()
{
	if ( m_docManager->CanUndo() ) {
		ClearSelection();
		m_docManager->Undo();
	}
}
void TLView::Redo()
{
	if ( m_docManager->CanRedo() ) {
		ClearSelection();
		m_docManager->Redo();
	}
}
void TLView::SelectTrack( long y )
{
	int track = GetTrackByY( y );
	m_TlData->SelectTrack( track );
}
