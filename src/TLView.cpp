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

#include <iostream>

#include "stuff.h"
#include "TLTrack.h"
#include "TLView.h"
#include "TLData.h"
#include "TLItem.h"
#include "TLSample.h"
#include "TLSelectionSet.h"
#include "GgseqDocManager.h"
#include "gun_girl.xpm"
#include "TLMuteButton.h"
#include "dial.h"
#include "TLTrackVolumeDial.h"

#define SNAP_POSITION 117600
//OLD_SKOOL 110250
//FUNK 94500

#define LEFT_OFFSET_TRACKS 52
#define TOP_OFFSET_TRACKS 3 //22

float MAX(float a, float b, float c)
{
	if (a>=b && a>=c)
		return a;
	if (b>=a && b>=c)
		return b;
	return c;
}
float MIN(float a, float b, float c)
{
	if (a<=b && a<=c)
		return a;
	if (b<=a && b<=c)
		return b;
	return c;
}
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = MIN( r, g, b );
	max = MAX( r, g, b );
	*v = max; // v
	delta = max - min;
	if( max != 0 )
		*s = delta / max; // s
	else {
		// r = g = b = 0 // s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max ) {
		*h = ( g - b ) / delta; // between yellow & magenta
	} else if( g == max ) {
		*h = 2 + ( b - r ) / delta; // between cyan & yellow
	} else {
		*h = 4 + ( r - g ) / delta; // between magenta & cyan
	}
	*h *= 60; // degrees
	if( *h < 0 )
		*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60; // sector 0 to 5
	i = (int)floor( h );
	f = h - i; // factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default: // case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

TLView::TLView(TLData *TlData)
{
	m_TlData=TlData;
	m_TrackDrawDist=5;
	m_selectionSet=new TLSelectionSet();
	wxConfigBase *conf=wxConfigBase::Get();
	conf->SetPath(wxT("/"));
	m_TlData->SetSnapValue(conf->Read(wxT("SnapPosition"), SNAP_POSITION));
	m_gungirl=new wxIcon(gun_girl_xpm);	
	m_SnapSuspended=false;
	m_docManager = new GgseqDocManager( m_TlData );
	m_TlData->SetDocManager(m_docManager);
	m_YscrollPosition=0;
}
void TLView::SuspendSnap() { m_SnapSuspended=true; }
void TLView::ResumeSnap() { m_SnapSuspended=false; }
TLView::~TLView()
{
	wxConfigBase *conf=wxConfigBase::Get();
	conf->Write(wxT("SnapPosition"),m_TlData->GetSnapValue()/*m_SnapPosition*/);
	delete m_selectionSet;
	delete m_TlData;
	delete m_gungirl;
	delete m_docManager;
}
void TLView::SetVisibleFrame(long width, long height, long x, long y)
{
	m_FrameX=x;
	m_FrameY=y;
	m_FrameWidth=width;
	m_FrameHeight=height;
	m_Faktor=(float)m_FrameWidth/(float)m_LengthVisible;
	SetVisibleLength(m_FrameWidth);
}
void TLView::SetVisibleLength(gg_tl_dat Length)
{
	m_LengthVisible=(Length*117600)/31; //TODO: Nicht Hardcoden
	m_Faktor=(float)m_FrameWidth/(float)m_LengthVisible;
}
void TLView::SetPosition(gg_tl_dat Position)
{
	m_PositionVisible=(Position*117600)/31; //TODO: Nicht Hardcoden
}
gg_tl_dat TLView::GetPosition()
{
	return (m_PositionVisible*31)/117600; //TODO: Nicht Hardcoden
}

gg_tl_dat TLView::GetScrollBarRange()
{
	return (m_TlData->GetLength()/117600)*31; //TODO: Nicht Hardcoden
}
gg_tl_dat TLView::GetScrollBarThumbSize()
{
	return (m_LengthVisible/117600)*31; //TODO: Nicht Hardcoden
}
void TLView::Draw(wxDC& dc/*_screen*/)
{
//	wxMemoryDC dc;
	long yoffset;
//	dc.SelectObject(wxBitmap(m_FrameWidth,m_FrameHeight));
	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
//	dc.DrawRectangle(0,0,m_FrameWidth,m_FrameHeight);

//-.-
	dc.SetBrush(*wxGREY_BRUSH);
	wxBrush brush1=dc.GetBrush();
	brush1.SetColour(wxColour(166,166,166));
	dc.SetBrush(brush1);
/*	wxPen pen1=dc.GetPen();
	pen1.SetColour(wxColour(100,100,100));
	dc.SetPen(pen1);*/
	dc.SetPen(wxPen(wxColour(100,100,100),1,wxSOLID));
	yoffset=TOP_OFFSET_TRACKS;//m_FrameY;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *track = node->GetData();
		if (track->GetSelected()) {
			wxBrush brush1=dc.GetBrush();
			brush1.SetColour(wxColour(199,199,188));
			dc.SetBrush(brush1);
		}
		dc.DrawRectangle(m_FrameX,yoffset-m_YscrollPosition,m_FrameWidth,track->GetHeight());
		yoffset+=track->GetHeight()+m_TrackDrawDist;
		if (track->GetSelected()) {
			wxBrush brush1=dc.GetBrush();
			brush1.SetColour(wxColour(166,166,166));
			dc.SetBrush(brush1);
		}

	}

//-.-
	//dc.DrawIcon(*m_gungirl,m_FrameWidth+m_FrameX-m_gungirl->GetWidth(),m_FrameHeight+TOP_OFFSET_TRACKS-m_gungirl->GetHeight());

	yoffset=TOP_OFFSET_TRACKS;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		yoffset = DrawTrack(dc, yoffset, current);
	}
	/*if (m_Length>m_PositionVisible && m_Length<m_PositionVisible+m_LengthVisible)
		dc.DrawLine((long)((m_Length-m_PositionVisible)*m_Faktor),0,(long)((m_Length-m_PositionVisible)*m_Faktor),m_FrameHeight);*/
//	dc_screen.Blit(m_FrameX, m_FrameY,m_FrameWidth,m_FrameHeight , &dc, 0,0);
}
void TLView::UpdateDialsAndButtons()
{
	int i=0;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->m_muteButton->SetSize(5,TOP_OFFSET_TRACKS+i*30-m_YscrollPosition,25,25 );
		current->m_volumeDial->SetSize(31,TOP_OFFSET_TRACKS+i*30-m_YscrollPosition,25,25 );
		i++;
	}

}
long TLView::DrawTrack(wxDC& dc, long yoffset, TLTrack* track)
{
	gg_tl_dat start,end;
/*	dc.SetBrush(*wxGREY_BRUSH);
	wxBrush brush1=dc.GetBrush();
	brush1.SetColour(wxColour(166,166,166));
	dc.SetBrush(brush1);
	pen1.SetColour(wxColour(100,100,100));
	dc.SetPen(pen1);
	dc.DrawRectangle(m_FrameX,yoffset,m_FrameWidth,track->GetHeight());*/

	wxPen pen1=dc.GetPen();
	pen1.SetColour(*wxBLACK);
	dc.SetPen(pen1);
	dc.SetBrush(*wxRED_BRUSH);
	for ( TLItemList::Node *node = track->GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData();
		if ((current->GetPosition() >= m_PositionVisible &&
		  current->GetPosition() <= m_PositionVisible+m_LengthVisible) ||
		  (current->GetPosition()+current->GetLength() >= m_PositionVisible &&
		  current->GetPosition()+current->GetLength()<= m_PositionVisible+m_LengthVisible) || 
		  (current->GetPosition() <= m_PositionVisible && current->GetPosition()+current->GetLength() >= m_PositionVisible+m_LengthVisible)) {
			start=current->GetPosition()-m_PositionVisible;
			if (start<0)
				start=0;
			end=current->GetPosition()+current->GetLength()-m_PositionVisible;
			if (end > m_LengthVisible)
				end=m_LengthVisible;
			wxBrush b1=dc.GetBrush();
			b1.SetColour(current->GetSample()->GetColour());
			dc.SetBrush(b1);
		//	if (current->IsSelected()) {
		//		pen1=dc.GetPen();
		//		pen1.SetColour(*wxWHITE);
		//		pen1.SetStyle(wxSHORT_DASH);
		//		pen1.SetWidth(3);
		//		dc.SetPen(pen1);
		//	}
//			dc.DrawRectangle((long)(start*m_Faktor)+m_FrameX,yoffset,(long)((end-start)*m_Faktor),track->GetHeight());
			dc.SetPen(*wxBLACK_PEN);
			dc.SetBrush(*wxBLACK_BRUSH);
			Draw3dRect(&dc,(long)(start*m_Faktor)+m_FrameX,yoffset-m_YscrollPosition,(long)((end-start)*m_Faktor),track->GetHeight(),current->GetSample()->GetColour());
			if (current->IsSelected()) {
				pen1=dc.GetPen();
				pen1.SetColour(*wxWHITE);
				pen1.SetStyle(wxSHORT_DASH);
				pen1.SetWidth(3);
				dc.SetPen(pen1);
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.DrawRectangle((long)(start*m_Faktor)+m_FrameX,yoffset-m_YscrollPosition,(long)((end-start)*m_Faktor),track->GetHeight());
			}

			dc.SetPen(*wxBLACK_PEN);
			dc.SetClippingRegion((long)(start*m_Faktor)+m_FrameX,yoffset-m_YscrollPosition,(long)((end-start)*m_Faktor),track->GetHeight());
			dc.SetFont(*wxSMALL_FONT);
			wxFileName fn(current->GetSample()->GetFilename());
//#ifndef __WXMSW__ 
			if ( (long)(end*m_Faktor)+m_FrameX>(long)(start*m_Faktor)+1+m_FrameX ) //TODO Evil Hack ;)
				dc.DrawText(fn.GetName(),(long)((current->GetPosition()-m_PositionVisible)/*start*/ *m_Faktor)+1+m_FrameX,yoffset+1-m_YscrollPosition);
				
//#endif
			dc.DestroyClippingRegion();
		}
	}
	return yoffset+track->GetHeight()+m_TrackDrawDist;
}
void TLView::Draw3dRect(wxDC *dc, wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxColour colour)
{
	wxBrush b1=dc->GetBrush();
	b1.SetColour(colour);
	dc->SetBrush(b1);
	dc->DrawRectangle(x,y,width,height);
/*	float H, S, V;
	RGBtoHSV(((float)colour.Red())/256.0,((float)colour.Green())/256.0,((float)colour.Blue())/256.0,&H,&S,&V);
	S-=0.2;
	if (S<0)
		S=0;
	float R,G,B;
	HSVtoRGB(&R,&G,&B,H,S,V);
	wxColour c1((unsigned char)(R*255),(unsigned char)(G*255),(unsigned char)(B*255));*/
	wxPen pen1=dc->GetPen();
	pen1.SetColour(GetLightColour(colour));
	dc->SetPen(pen1);

//	dc->SetPen(*wxGREEN_PEN);
	dc->DrawLine(x,y,x+width-1,y);
	dc->DrawLine(x,y,x,y+height-1);


/*	RGBtoHSV(((float)colour.Red())/256.0,((float)colour.Green())/256.0,((float)colour.Blue())/256.0,&H,&S,&V);
	V-=0.2;
	if (V<0)
		V=0;
	HSVtoRGB(&R,&G,&B,H,S,V);
	c1 = wxColour((unsigned char)(R*255),(unsigned char)(G*255),(unsigned char)(B*255));*/
	pen1=dc->GetPen();
	pen1.SetColour(GetDarkColour(colour));
	dc->SetPen(pen1);

	dc->DrawLine(x,y+height-1,x+width-1,y+height-1);
	dc->DrawLine(x+width-1,y,x+width-1,y+height);
}
wxColour TLView::GetDarkColour(wxColour colour)
{
	float H, S, V;
	float R, G, B;
	R=((float)colour.Red())/256.0;
	G=((float)colour.Green())/256.0;
	B=((float)colour.Blue())/256.0;
	RGBtoHSV(R,G,B,&H,&S,&V);
	V-=0.2;
	if (V<0)
		V=0;
	HSVtoRGB(&R,&G,&B,H,S,V);
	wxColour result((unsigned char)(R*255),(unsigned char)(G*255),(unsigned char)(B*255));
	return result;
}
wxColour TLView::GetLightColour(wxColour colour)
{
	float H, S, V;
	RGBtoHSV(((float)colour.Red())/256.0,((float)colour.Green())/256.0,((float)colour.Blue())/256.0,&H,&S,&V);
	S-=0.3;
	if (S<0)
		S=0;
	float R,G,B;
	HSVtoRGB(&R,&G,&B,H,S,V);
	wxColour result((unsigned char)(R*255),(unsigned char)(G*255),(unsigned char)(B*255));
	return result;
}
void TLView::AddItem(TLSample *sample, long position, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
//	m_TlData->AddItem(sample,GetSnap(FromScreenXtoTL(position)),trackNr);
	m_docManager->SubmitCommand( new GgseqAddItemCommand( m_TlData, sample->GetFilename(), GetSnap(FromScreenXtoTL(position)), trackNr, sample ));
}

void TLView::AddItem(wxString filename, long position, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
	m_docManager->SubmitCommand( new GgseqAddItemCommand( m_TlData, filename,
		GetSnap(FromScreenXtoTL(position)),trackNr ) );
/*	TLItem *item=m_TlData->AddItem(filename,FromScreenXtoTL(position),trackNr);
	if (!item)
		return;
	SnapItem(item);*/
}

void TLView::DeleteItem(TLItem *item, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
//	m_TlData->DeleteItem(item,trackNr);
	m_docManager->SubmitCommand( new GgseqDeleteItemCommand( m_TlData, item ) );
}

gg_tl_dat TLView::FromScreenXtoTL(long x)
{
	
	gg_tl_dat f = m_PositionVisible+( ((gg_tl_dat)(x-m_FrameX)) * ( (gg_tl_dat)m_LengthVisible / (gg_tl_dat)m_FrameWidth ) );
	return f;
}
long TLView::FromTLtoScreenX(gg_tl_dat x)
{
	double a;
	double b;
	b=(double)(x-m_PositionVisible)*(double)m_FrameWidth;
	a=(b/m_LengthVisible)+m_FrameX;
	return (long)a;
}
long TLView::GetTrackByY(long y)
{
	long y_offset = TOP_OFFSET_TRACKS-m_YscrollPosition;
	long cnt = 0;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		if (y>y_offset && y<current->GetHeight()+y_offset)
			return cnt;
		cnt++;
		y_offset+=current->GetHeight()+m_TrackDrawDist;
	}
	return -1;
}
TLItem *TLView::GetDragItem(long x, long y)
{
	if (m_TlData->IsBlocked())
		return NULL;
	long track=GetTrackByY(y);
	if (track<0)
		return NULL;
	return m_TlData->ItemAtPos(FromScreenXtoTL(x),track);
}
void TLView::DoDrop(long x, long y, TLItem *item, long srcTrack, long x_offset, bool copy)
{
	if (m_TlData->IsBlocked())
		return;
	long track=GetTrackByY(y);
	if (x+x_offset<m_FrameX || x+x_offset>m_FrameX+m_FrameWidth)
		track=-1;

	/*Delete Item*/
	if (track<0 && !copy) {
		DeleteItem(item,srcTrack);
		return;
	} else if (track<0)
		return;
	if (copy) {
		AddItem(item->GetSample(),x,track);
	} else {
		m_docManager->SubmitCommand( new GgseqMoveItemCommand( m_TlData, item, GetSnap(FromScreenXtoTL(x)), track ) );
	}
}
gg_tl_dat TLView::GetSnap( gg_tl_dat x )
{
	if ( m_SnapSuspended )
		return x;
	gg_tl_dat pos=x%(gg_tl_dat)m_TlData->GetSnapValue();
	if (pos) {
		if (pos> (gg_tl_dat)m_TlData->GetSnapValue()/2)
			x=x-pos+ (gg_tl_dat)m_TlData->GetSnapValue();
		else
			x=x-pos;
	}
	return x;
}

void TLView::SetPlaybackPosition(long Position)
{
	if (m_TlData->IsBlocked())
		return;
	m_TlData->SetPlaybackPosition(FromScreenXtoTL(Position));
}
TLSample *TLView::GetSample(long position, long trackNr)
{
	TLItem *item = m_TlData->ItemAtPos(FromScreenXtoTL(position), trackNr);
	if (!item)
		return NULL;
	return item->GetSample();
}

long TLView::GetScreenSnapPosition(long position)
{	
	gg_tl_dat pos1=FromScreenXtoTL(position);
	pos1=GetSnap(pos1);
	return FromTLtoScreenX(pos1);
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
	int i=0;
	int firstTrackNr;
	int scndTrackNr;
	GetTracksSurroundedBy(firstTrackNr, scndTrackNr, y,y+height );
	/*FromScreenXtoTL*/
	gg_tl_dat TL_X1=FromScreenXtoTL(x);
	gg_tl_dat TL_X2=FromScreenXtoTL(x+width);

	for ( TLTrackList::Node *trackNode = m_TlData->GetFirst(); trackNode; trackNode = trackNode->GetNext() ) {
		if (i>=firstTrackNr && i<=scndTrackNr) {
			TLTrack *track = trackNode->GetData();
			for ( TLItemList::Node *node = track->GetFirst(); node; node = node->GetNext() ) {
				TLItem *current = node->GetData();
				if (current->GetPosition()<=TL_X2&&current->GetEndPosition()>=TL_X1) {
					m_selectionSet->AddSample(current->GetSample(),current);
				}
			
			}
		}
		i++;
	}
	m_selectionSet->CalcBounds();
}

void TLView::ClearSelection()
{
	m_selectionSet->Clear(m_TlData);
}

void TLView::GetTracksSurroundedBy(int &trackNr1, int &trackNr2, long y1, long y2)
{
	trackNr1=-1;
	trackNr2=-1;
	long y_offset = TOP_OFFSET_TRACKS-m_YscrollPosition;
	int cnt = 0;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		if (trackNr1<0)
			if (y_offset+current->GetHeight()>y1)
				trackNr1=cnt;
		if (y_offset/*+current->GetHeight()*/<y2)
			trackNr2=cnt;
		cnt++;
		y_offset+=current->GetHeight()+m_TrackDrawDist;
	}
}
bool TLView::IsSelectionAt(int x, int y, int& x_offset, int& y_offset, int& width, int& height)
{
	if (m_TlData->IsBlocked())
		return false;
	if (!m_selectionSet->IsActive())
		return false;
	gg_tl_dat TL_x=FromScreenXtoTL(x);
	int track=GetTrackByY(y);

	width=FromTLtoScreenX(m_selectionSet->GetX2())-FromTLtoScreenX(m_selectionSet->GetX1());
	height=(m_selectionSet->GetTrack2()-m_selectionSet->GetTrack1()+1)*30-5;
	if (track>=m_selectionSet->GetTrack1() && track<=m_selectionSet->GetTrack2() && TL_x>=m_selectionSet->GetX1() && TL_x<=m_selectionSet->GetX2()) {
		x_offset = x-FromTLtoScreenX(m_selectionSet->GetX1());
		y_offset = y-(m_selectionSet->GetTrack1()*30+5);
		return true;
	}
	return false;
}
void TLView::DrawSelection(wxDC *dc)
{
	for ( TLSelItemList::Node *node = m_selectionSet->GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData()->GetItem();
		dc->DrawRectangle((int)((current->GetPosition()-m_selectionSet->GetX1())*m_Faktor),
		  (current->GetTrack()-m_selectionSet->GetTrack1())*30,
		  (int)(current->GetLength()*m_Faktor),
		  25);
	}
}
void TLView::EndSelectionDrag(int x, int y, bool copy, long x_offset)
{
	gg_tl_dat TL_x=FromScreenXtoTL(x);/*TODO: festellen wann: Add, Move, Delete*/
	TL_x=GetSnap(TL_x);

	int track=GetTrackByY(y);
	if (x+x_offset<m_FrameX || x+x_offset>m_FrameX+m_FrameWidth)
		track=-1;
	TLSelectionSet *SelSet;
	//Add: track>=0 and copy==true
	//Move: track>=0 and copy==false
	//Delete: track<0 and copy==false
	if ( track>=0 && copy ) { /*Add*/
		m_docManager->SubmitCommand( new GgseqAddItemsCommand( m_TlData, m_selectionSet, &SelSet, TL_x, track ));
		delete m_selectionSet;
		m_selectionSet=SelSet;
	}
	if ( track>=0 && !copy ) {
		m_docManager->SubmitCommand( new GgseqMoveItemsCommand( m_TlData, m_selectionSet, &SelSet, TL_x, track ));
		delete m_selectionSet;
		m_selectionSet=SelSet;
	}
	if ( track<0 and !copy ) {
		m_docManager->SubmitCommand( new GgseqDeleteItemsCommand( m_TlData, m_selectionSet ));
	}
}


void TLView::SetSnapValue(long snapValue)
{
	m_TlData->SetSnapValue(snapValue);
}
long TLView::GetSnapValue()
{
	return m_TlData->GetSnapValue();
}
void TLView::Undo()
{
	if (m_docManager->CanUndo()) {
		ClearSelection();
		m_docManager->Undo();
	}
}
void TLView::Redo()
{
	if (m_docManager->CanRedo()) {
		ClearSelection();
		m_docManager->Redo();
	}
}
void TLView::SelectTrack(long y)
{
	int track=GetTrackByY(y);
	m_TlData->SelectTrack(track);
}
