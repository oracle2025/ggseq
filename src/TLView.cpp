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

#include "TLTrack.h"
#include "TLView.h"
#include "TLData.h"
#include "TLItem.h"
#include "TLSample.h"
#include "TLSelectionSet.h"

#define SNAP_POSITION 117600
//OLD_SKOOL 110250
//FUNK 94500

TLView::TLView(TLData *TlData)
{
	m_TlData=TlData;
	m_TrackDrawDist=5;
	m_selectionSet=new TLSelectionSet();
	wxConfig config(wxT("ggseq"));
	m_SnapPosition=config.Read(wxT("SnapPosition"), SNAP_POSITION);
	
}
TLView::~TLView()
{
	wxConfig config(wxT("ggseq"));
	config.Write(wxT("SnapPosition"),m_SnapPosition);
	delete m_selectionSet;
	delete m_TlData;
}
void TLView::SetVisibleFrame(long width, long height, long x, long y)
{
	m_FrameX=x;
	m_FrameY=y;
	m_FrameWidth=width;
	m_FrameHeight=height;
	m_Faktor=(float)m_FrameWidth/(float)m_LengthVisible;
	m_Length=2000000;
	SetVisibleLength(m_FrameWidth);/*--*/
}
void TLView::SetVisibleLength(long Length)
{
	m_LengthVisible=(Length*117600)/31;
	m_Faktor=(float)m_FrameWidth/(float)m_LengthVisible;
}
void TLView::SetPosition(long Position)
{
	m_PositionVisible=(Position*117600)/31;
}
long TLView::GetScrollBarRange()
{
	return (m_TlData->GetLength()/117600)*31;//m_Length;
}
long TLView::GetScrollBarThumbSize()
{
	return (m_LengthVisible/117600)*31;
}
void TLView::Draw(wxDC& dc/*_screen*/)
{
//	wxMemoryDC dc;
	long yoffset;
//	dc.SelectObject(wxBitmap(m_FrameWidth,m_FrameHeight));
	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
//	dc.DrawRectangle(0,0,m_FrameWidth,m_FrameHeight);
	yoffset=m_FrameY;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		yoffset = DrawTrack(dc, yoffset, current);
	}
	/*if (m_Length>m_PositionVisible && m_Length<m_PositionVisible+m_LengthVisible)
		dc.DrawLine((long)((m_Length-m_PositionVisible)*m_Faktor),0,(long)((m_Length-m_PositionVisible)*m_Faktor),m_FrameHeight);*/
//	dc_screen.Blit(m_FrameX, m_FrameY,m_FrameWidth,m_FrameHeight , &dc, 0,0);
}
long TLView::DrawTrack(wxDC& dc, long yoffset, TLTrack* track)
{
	long start,end;
	dc.SetBrush(*wxGREY_BRUSH);
	
	wxBrush brush1=dc.GetBrush();
	brush1.SetColour(wxColour(166,166,166));
	dc.SetBrush(brush1);
	wxPen pen1=dc.GetPen();
	pen1.SetColour(wxColour(100,100,100));
	dc.SetPen(pen1);
	dc.DrawRectangle(m_FrameX,yoffset,m_FrameWidth,track->GetHeight());
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
			if (current->IsSelected()) {
				pen1=dc.GetPen();
				pen1.SetColour(*wxWHITE);
				pen1.SetStyle(wxSHORT_DASH);
				pen1.SetWidth(3);
				dc.SetPen(pen1);
			}
			dc.DrawRectangle((long)(start*m_Faktor)+m_FrameX,yoffset,(long)((end-start)*m_Faktor),track->GetHeight());
			dc.SetPen(*wxBLACK_PEN);
			dc.SetClippingRegion((long)(start*m_Faktor)+m_FrameX,yoffset,(long)((end-start)*m_Faktor),track->GetHeight());
			dc.SetFont(*wxSMALL_FONT);
			wxFileName fn(current->GetSample()->GetFilename());
#ifndef __WXMSW__ 
			dc.DrawText(fn.GetName(),(long)(start*m_Faktor)+1+m_FrameX,yoffset+1);
#endif
			dc.DestroyClippingRegion();
		}
	}
	return yoffset+track->GetHeight()+m_TrackDrawDist;
}
void TLView::AddItem(TLSample *sample, long position, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
	TLItem *item=m_TlData->AddItem(sample,FromScreenXtoTL(position),trackNr);
	if (!item)
		return;
	SnapItem(item);
}

void TLView::AddItem(wxString filename, long position, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
	TLItem *item=m_TlData->AddItem(filename,FromScreenXtoTL(position),trackNr);
	if (!item)
		return;
	SnapItem(item);
}

void TLView::DeleteItem(TLItem *item, long trackNr)
{
	if (m_TlData->IsBlocked())
		return;
	m_TlData->DeleteItem(item,trackNr);
}

long TLView::FromScreenXtoTL(long x)
{

	long f = m_PositionVisible+(((long)(x-m_FrameX))*((long)m_LengthVisible/m_FrameWidth));
	return f;
}
long TLView::FromTLtoScreenX(long x)
{
	double a;
	double b;
	b=(double)(x-m_PositionVisible)*(double)m_FrameWidth;
	a=(b/m_LengthVisible)+m_FrameX;
	return (long)a;
}
long TLView::GetTrackByY(long y)
{
	long y_offset = m_FrameY;
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
void TLView::DoDrop(long x, long y, TLItem *item, long srcTrack, bool copy)
{
	if (m_TlData->IsBlocked())
		return;
	long track=GetTrackByY(y);

	/*Delete Item*/
	if (track<0 && !copy) {
		DeleteItem(item,srcTrack);
		return;
	} else if (track<0)
		return;
	if (copy) {
		AddItem(item->GetSample(),x,track);
	} else {
		if (srcTrack==track) { /*Move in Track*/
			m_TlData->SetItemPosition(item,FromScreenXtoTL(x));
			SnapItem(item);
		} else { /*Move out of Track*/
			AddItem(item->GetSample(),x,track);
			DeleteItem(item,srcTrack);
		}
	}
}
void TLView::SnapItem(TLItem *item)
{
	if (m_TlData->IsBlocked())
		return;
	long pos=item->GetPosition()%m_SnapPosition;
	if (pos) {
		if (pos>m_SnapPosition/2)
			m_TlData->SetItemPosition(item,item->GetPosition()-pos+m_SnapPosition);
		else
			m_TlData->SetItemPosition(item,item->GetPosition()-pos);
	}
}

void TLView::SetPlaybackPosition(long Position)
{
	if (m_TlData->IsBlocked())
		return;
	m_TlData->SetPlaybackPosition(FromScreenXtoTL(Position));
}
wxString TLView::GetSampleFilename(long position, long trackNr)
{
	puts("spass1");
	TLItem *item = m_TlData->ItemAtPos(FromScreenXtoTL(position), trackNr);
	if (!item)
		return wxT("");;
	TLSample *sample = item->GetSample();
	puts("spass2");
	return sample->GetFilename();
	
}

long TLView::GetScreenSnapPosition(long position)
{	
	long pos1=FromScreenXtoTL(position);
	long pos=pos1%m_SnapPosition;
	if (pos) {
		if (pos>m_SnapPosition/2)
			pos1=pos1-pos+m_SnapPosition;
		else
			pos1=pos1-pos;
	}
	return FromTLtoScreenX(pos1);
}

long TLView::GetCaretPosition()
{
	return FromTLtoScreenX(m_TlData->GetPlaybackPosition()/*m_CaretPosition*/);	
}

long TLView::GetCaretPosition(long Position)
{
	return FromTLtoScreenX(Position);	
}
void TLView::Select(long x,long y,long width,long height)
{
	ClearSelection();
	int i=0;
	int firstTrackNr;
	int scndTrackNr;
	GetTracksSurroundedBy(firstTrackNr, scndTrackNr, y,y+height );
	/*FromScreenXtoTL*/
	long TL_X1=FromScreenXtoTL(x);
	long TL_X2=FromScreenXtoTL(x+width);

//	m_selectionSet->SetBounds(firstTrackNr, scndTrackNr,TL_X1,TL_X2);

	for ( TLTrackList::Node *trackNode = m_TlData->GetFirst(); trackNode; trackNode = trackNode->GetNext() ) {
		if (i>=firstTrackNr && i<=scndTrackNr) {
			TLTrack *track = trackNode->GetData();
			for ( TLItemList::Node *node = track->GetFirst(); node; node = node->GetNext() ) {
				TLItem *current = node->GetData();
				if (current->GetPosition()>=TL_X1&&current->GetEndPosition()<=TL_X2) {
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
	long y_offset = m_FrameY;
	int cnt = 0;
	for ( TLTrackList::Node *node = m_TlData->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		if (trackNr1<0)
			if (y_offset>y1)
				trackNr1=cnt;
		if (y_offset+current->GetHeight()<y2)
			trackNr2=cnt;
		cnt++;
		y_offset+=current->GetHeight()+m_TrackDrawDist;
	}
}
/*bool TLView::HasSelection()
{
	m_selectionSet->IsActive()
}*/
bool TLView::IsSelectionAt(int x, int y, int& x_offset, int& y_offset, int& width, int& height)
{
	if (m_TlData->IsBlocked())
		return false;
	if (!m_selectionSet->IsActive())
		return false;
	long TL_x=FromScreenXtoTL(x);
	int track=GetTrackByY(y);

//	x_offset=FromTLtoScreenX(m_selectionSet->GetX1());
//	y_offset=m_selectionSet->GetTrack1()*30+5;

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
void TLView::EndSelectionDrag(int x, int y, bool copy)
{
	long TL_x=FromScreenXtoTL(x);
	long pos=TL_x%m_SnapPosition;
	if (pos) {
		if (pos>m_SnapPosition/2)
			TL_x=TL_x-pos+m_SnapPosition;
		else
			TL_x=TL_x-pos;
	}

	int track=GetTrackByY(y);
	if (track>=0)
		m_selectionSet->AddTo(m_TlData,TL_x,track);
	if (!copy)
		m_selectionSet->DeleteFrom(m_TlData);
}

