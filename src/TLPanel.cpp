/* TLPanel.cpp
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
#ifdef __WXMSW__ 
#include <wx/generic/dragimgg.h>
#else
#include <wx/dragimag.h>
#endif

#include <wx/listctrl.h>
#include <wx/config.h>
#include <wx/tglbtn.h>
#include <wx/filename.h>
#include <iostream>

#include "TLView.h"
#include "TLPanel.h"
#include "TLItem.h"
#include "TLSample.h"
#include "TLMiniFiler2.h"
#include "TLLoadSaveManager.h"
#include "SoundManager.h"
#include "TLData.h"
#include "TLSelectColourDialog.h"
#include "TLSetSnapDialog.h"
#include "TLMuteButton.h"
#include "mute_off.xpm"
#include "dial.h"
#include "TLTrackVolumeDial.h"
#include "TLTrack.h"
#include "MiniPlayer.h"
#include "Ruler.h"

#define LEFT_OFFSET_TRACKS 52
#define TOP_OFFSET_TRACKS 22

enum
{
		ID_ScrollBar = 1,
};

BEGIN_EVENT_TABLE(TLPanel, wxPanel)
	EVT_PAINT(TLPanel::OnPaint)
//	EVT_LEFT_UP(TLPanel::OnMouseLeftUp)
	EVT_LEFT_UP(TLPanel::OnMouseUp)
	EVT_RIGHT_UP(TLPanel::OnMouseUp)
	EVT_RIGHT_DOWN(TLPanel::OnMouseDown)
	EVT_LEFT_DOWN(TLPanel::OnMouseDown)
	EVT_LEFT_DCLICK(TLPanel::OnDoubleClick)
	EVT_MOTION(TLPanel::OnMouseMotion)
	EVT_SIZE(TLPanel::OnSize)
	EVT_COMMAND_SCROLL_THUMBTRACK(ID_ScrollBar,TLPanel::OnScroll)
#ifndef __WXMSW__
	EVT_ERASE_BACKGROUND(TLPanel::OnEraseBackground)
#endif
END_EVENT_TABLE()

TLPanel::TLPanel(wxWindow* parent, wxScrollBar *scrollbar, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:wxPanel(parent, id, pos, size, style/*|wxSUNKEN_BORDER*/ , name)
{

	m_data = new TLData();
	m_data->AddTrack();
	m_data->AddTrack();
	m_data->AddTrack();
	m_data->AddTrack();
	m_data->AddTrack();
	m_data->AddTrack();
	m_data->AddTrack();
	m_data->AddTrack();

	m_TlView = new TLView(m_data);
	m_TlView->SetVisibleFrame(100,100);
	m_TlView->SetVisibleLength(1420000);
	m_TlView->SetPosition(0);

	m_loadSaveManager = new TLLoadSaveManager(this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(),m_data);
	m_soundManager = new SoundManager(m_data);


	m_scrollBar = scrollbar;//new wxScrollBar(this,ID_ScrollBar,wxPoint(0,0),wxSize(GetSize().GetWidth(),0));	

	int i=0;
	for ( TLTrackList::Node *node = m_data->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
#ifdef __WXMSW__
		/*TLMuteButton *button =*/m_buttons[i] = new TLMuteButton( this, -1,current,wxT("M"), wxPoint(5,TOP_OFFSET_TRACKS+i*30),wxSize(25,25));
#else
		/*TLMuteButton *button =*/m_buttons[i] = new TLMuteButton( this, -1,current,mute_off_xpm, wxPoint(5,TOP_OFFSET_TRACKS+i*30),wxSize(25,25));
#endif
		
		m_dials[i] = new TLTrackVolumeDial(this, -1, current,wxPoint(31,TOP_OFFSET_TRACKS+i*30),wxSize(25,25));
		i++;
	}
	m_ruler = new Ruler(this,-1,wxPoint(LEFT_OFFSET_TRACKS,2),wxSize(GetSize().GetWidth()-5-LEFT_OFFSET_TRACKS,15),wxRAISED_BORDER);
	m_ruler->SetSnap((m_TlView->GetSnapValue()/*m_SnapPosition*/*31)/117600);
	
	
//	m_data->GetTrackCount

//	m_scrollBar->SetSize(0,GetSize().GetHeight()-20,GetSize().GetWidth(),20);
	m_scrollBar->SetScrollbar(0,m_TlView->GetScrollBarThumbSize() ,m_TlView->GetScrollBarRange() , m_TlView->GetScrollBarThumbSize());
	m_TlView->SetVisibleFrame(GetSize().GetWidth(),GetSize().GetHeight()/*-m_scrollBar->GetSize().GetHeight()*/);
	m_CaretPosition=0;
	m_CaretVisible=true;//false;
	m_sampleDrag=false;
	m_rubberDrag=false;
	m_selectionDrag=false;
	m_frameVisible=false;
	UpdateButtons();
	m_miniPlayer=NULL;
	SetSizeHints(100, 200);
}
TLPanel::~TLPanel()
{
	delete m_TlView;
	delete m_soundManager;
	delete m_loadSaveManager;
}
void TLPanel::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	m_TlView->Draw(dc);
//	m_CaretPosition = m_TlView->GetCaretPosition();
	DrawCaret(dc);
/*	if (m_DragFrameVisible) {
		SetRubberframePen(&dc);
		dc.DrawRectangle(m_DragFrameLastX,m_DragFrameLastY,m_SampleDragItemWidth,25);
	}*/
}
void TLPanel::OnEraseBackground(wxPaintEvent& event)
{
}
void TLPanel::OnSize(wxSizeEvent& event)
{
#ifdef __WXMSW__
	m_TlView->SetVisibleFrame(GetSize().GetWidth()-11-LEFT_OFFSET_TRACKS,GetSize().GetHeight()-22-TOP_OFFSET_TRACKS,5+LEFT_OFFSET_TRACKS,TOP_OFFSET_TRACKS);
//	m_scrollBar->SetSize(0,GetSize().GetHeight()-22,GetSize().GetWidth()-6,16);
#else
	m_TlView->SetVisibleFrame(GetSize().GetWidth()-10-LEFT_OFFSET_TRACKS,GetSize().GetHeight()/*-m_scrollBar->GetSize().GetHeight()*/-TOP_OFFSET_TRACKS,5+LEFT_OFFSET_TRACKS,TOP_OFFSET_TRACKS);
//	m_scrollBar->SetSize(0,GetSize().GetHeight()-m_scrollBar->GetSize().GetHeight()-2,GetSize().GetWidth()-4,20);
#endif
	m_ruler->SetSize(5+LEFT_OFFSET_TRACKS,2,GetSize().GetWidth()-10-LEFT_OFFSET_TRACKS,15);

	ResetScrollBar();
}
void TLPanel::OnMouseMotion(wxMouseEvent& event)
{
	if (m_sampleDrag) {
		UpdateSampleDrag(event.m_x,event.m_y);
		return;
	}
	if (m_rubberDrag) {
		UpdateRubberFrame(event.m_x, event.m_y);
		return;
	}
	if (m_selectionDrag) {
		UpdateSelectionDrag(event.m_x, event.m_y);
		return;
	}
	if (event.m_leftDown||event.m_rightDown) {
		int c;
		int d;
		if (m_TlView->IsSelectionAt(event.m_x,event.m_y,x_offset,y_offset,c,d)) {
			StartSelectionDrag(event.m_x,event.m_y,c,d);
			return;
		}
		int srcTrackNr = m_TlView->GetTrackByY(m_DragY);
		TLItem *srcItem = m_TlView->GetDragItem(m_DragX,m_DragY);
		if (!srcItem) {
			if (event.m_leftDown)
				StartRubberFrame(event.m_x,event.m_y);
			return;
		}

		StartSampleDrag(event.m_x,event.m_y, srcTrackNr, srcItem);
	}
}
void TLPanel::OnMouseUp(wxMouseEvent& event)
{
	if (m_rubberDrag) {
		EndRubberFrame(event.m_x, event.m_y);
		return;
	}
	if (m_sampleDrag) {
		EndSampleDrag(event.m_x, event.m_y,event.RightUp());
		return;
	}
	if (m_selectionDrag) {
		EndSelectionDrag(event.m_x, event.m_y,event.RightUp());
		return;
	}
	if (event.LeftUp()) {
		m_TlView->ClearSelection();
#ifdef __WXMSW__
		Refresh(true);
#else
		Refresh(false);
#endif
		m_TlView->SetPlaybackPosition(event.m_x);
		wxClientDC dc(this);
		DrawCaret(dc);
		m_CaretPosition = m_TlView->GetCaretPosition();
		DrawCaret(dc);
	}
}
void TLPanel::OnMouseDown(wxMouseEvent& event)
{
	m_DragX=event.m_x;
	m_DragY=event.m_y;
	if(m_miniPlayer)
		m_miniPlayer->Stop();
}
void TLPanel::OnDoubleClick(wxMouseEvent& event)
{
	m_TlView->ClearSelection();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
	int trackNr = m_TlView->GetTrackByY(event.m_y);
	if (trackNr<0)
		return;
//	wxString tmp = m_TlView->GetSampleFilename(event.m_x, trackNr);
	TLSample *sample = m_TlView->GetSample(event.m_x, trackNr);
	if (sample !=NULL) {
		if (m_miniPlayer) {
			m_miniPlayer->SetSample(sample);
			m_miniPlayer->Play();
		}
	}
//	if (tmp!=wxT("")) {
//		if (m_miniPlayer) {
//			m_miniPlayer->SetFilename(tmp);
//			m_miniPlayer->Play();
//		}
//	}
}
void TLPanel::StartRubberFrame(int x, int y)
{
	CaptureMouse();
	m_TlView->ClearSelection();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
	m_rubberDrag=true;
//	wxClientDC dc(this);
//	SetRubberframePen(&dc);
	m_rubberFrame=wxRect(m_DragX,m_DragY,x-m_DragX,y-m_DragY);
//	ShowFrame(m_rubberFrame,&dc);
}
void TLPanel::UpdateRubberFrame(int x, int y)
{
	wxClientDC dc(this);
	SetRubberframePen(&dc);
	HideFrame(m_rubberFrame,&dc);
	m_rubberFrame.width=x-m_rubberFrame.x;
	m_rubberFrame.height=y-m_rubberFrame.y;
	ShowFrame(m_rubberFrame,&dc);
	return;
}
void TLPanel::EndRubberFrame(int x, int y)
{
	ReleaseMouse();
	m_rubberDrag=false;
	wxClientDC dc(this);
	SetRubberframePen(&dc);
	HideFrame(m_rubberFrame,&dc);
	if (m_rubberFrame.width<0) {
		m_rubberFrame.width=-m_rubberFrame.width;
		m_rubberFrame.x=m_rubberFrame.x-m_rubberFrame.width;
	}
	if (m_rubberFrame.height<0) {
		m_rubberFrame.height=-m_rubberFrame.height;
		m_rubberFrame.y=m_rubberFrame.y-m_rubberFrame.height;
	}

	m_TlView->Select(m_rubberFrame.x,m_rubberFrame.y,m_rubberFrame.width,m_rubberFrame.height);
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
	return;
}
void TLPanel::SetRubberframePen(wxDC* dc)
{
	dc->SetLogicalFunction(wxINVERT);
	dc->SetBrush(*wxTRANSPARENT_BRUSH);
	wxPen pen1=*wxBLACK_PEN;
	pen1.SetStyle(wxSHORT_DASH);
	dc->SetPen(pen1);
}
void TLPanel::StartSampleDrag(int x, int y, int srcTrackNr, TLItem* srcItem)
{
	m_TlView->ClearSelection();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
	m_SampleDragSrcTrackNr = srcTrackNr;
	m_DragItem = srcItem;

	x_offset = m_DragX-m_TlView->FromTLtoScreenX(m_DragItem->GetPosition());
	y_offset = m_DragY-m_SampleDragSrcTrackNr*30-/*5*/TOP_OFFSET_TRACKS;

	m_SampleDragItemWidth = m_DragItem->GetSample()->GetLength()/3793;
	wxBitmap bmp1(m_SampleDragItemWidth,25);
	wxMemoryDC dc;
	dc.SelectObject(bmp1);
	m_DragItem->GetSample()->Draw(dc);
#ifdef __WXMSW__ 
	m_dragImage = new wxGenericDragImage(bmp1);
#else
	m_dragImage = new wxDragImage(bmp1);
#endif
	m_dragImage->BeginDrag(wxPoint(x_offset,y_offset),this);
	m_dragImage->Move(wxPoint(x,y));
	m_oldXImgPos=x-x_offset;
	m_oldYImgPos=y-y_offset;
	m_dragImage->Show();
	m_sampleDrag=true;
}
void TLPanel::UpdateSampleDrag(int x, int y)
{
	wxClientDC dc(this);
	SetRubberframePen(&dc);
	int new_x=x-x_offset;
	int new_y=y-y_offset;
	
	wxRegion region(0,0,GetSize().GetWidth(),GetSize().GetHeight());
	region.Subtract(wxRect(m_oldXImgPos,m_oldYImgPos,m_SampleDragItemWidth,25));
	m_oldXImgPos=x-x_offset;
	m_oldYImgPos=y-y_offset;
	dc.SetClippingRegion(region);
	HideFrame(m_sampleFrame,&dc);
	dc.DestroyClippingRegion();

	m_dragImage->Move(wxPoint(x,y));
	int trackNr = m_TlView->GetTrackByY(y);
	if (trackNr<0) {
		return;
	}
	region=wxRegion(0,0,GetSize().GetWidth(),GetSize().GetHeight());
	region.Subtract(wxRect(new_x,new_y,m_SampleDragItemWidth,25));
	dc.SetClippingRegion(region);
	new_x=m_TlView->GetScreenSnapPosition(new_x);
	new_y=trackNr*30+/*5*/TOP_OFFSET_TRACKS;
	m_sampleFrame=wxRect(new_x,new_y,m_SampleDragItemWidth,25);
	ShowFrame(m_sampleFrame,&dc);
	dc.DestroyClippingRegion();
	return;
}
void TLPanel::EndSampleDrag(int x, int y, bool copyOnDrag)
{
	m_sampleDrag=false;
	m_dragImage->Hide();
	m_dragImage->EndDrag();
	delete m_dragImage;
	wxClientDC dc(this);
	SetRubberframePen(&dc);
	HideFrame(m_sampleFrame,&dc);
//	if (x-x_offset<0 || y<0 || x-x_offset>GetSize().GetWidth() || y>GetSize().GetHeight())
//		return;
	m_TlView->DoDrop(x-x_offset, y, m_DragItem, m_SampleDragSrcTrackNr,x_offset,copyOnDrag);
	ResetScrollBar();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
}
void TLPanel::StartSelectionDrag(int x, int y, int width, int height)
{
	int x_ = x-x_offset;
	int y_ = y;
//	CaptureMouse();
//	wxClientDC dc(this);
//	SetRubberframePen(&dc);
	m_selectionDrag=true;
	wxBitmap bitmap(width,height);
	wxMemoryDC dc;
	dc.SelectObject(bitmap);
	wxPen pen(*wxBLACK_PEN);
	pen.SetStyle(wxLONG_DASH);
	dc.SetPen(pen);
	dc.SetBrush(*wxGREEN_BRUSH);
	dc.DrawRectangle(0,0,width,height);
	wxBrush ab;
	ab.SetStyle(wxTRANSPARENT);
	dc.SetBrush(ab);
	pen.SetColour(*wxWHITE);
	pen.SetWidth(3);
	pen.SetStyle(wxSOLID);
	dc.SetPen(pen);
	m_TlView->DrawSelection(&dc);
	pen.SetWidth(1);
	pen.SetColour(*wxBLACK);
	pen.SetStyle(wxLONG_DASH);
	dc.SetPen(pen);
	m_TlView->DrawSelection(&dc);

	wxMask *mask = new wxMask(bitmap,*wxGREEN);
	bitmap.SetMask(mask);

#ifdef __WXMSW__ 
	m_dragImage = new wxGenericDragImage(bitmap);
#else
	m_dragImage = new wxDragImage(bitmap);
#endif
	
	m_dragImage->BeginDrag(wxPoint(0,0),this);
	m_selectionFrame=wxRect(x_,y_,width,height);
	int trackNr = m_TlView->GetTrackByY(y_);
	if (trackNr>=0) {
		m_selectionFrame.y=trackNr*30+TOP_OFFSET_TRACKS;
	} else {
		return;
	}
	m_selectionFrame.x=m_TlView->GetScreenSnapPosition(x_);

	m_dragImage->Move(wxPoint(m_selectionFrame.x,m_selectionFrame.y));
	m_dragImage->Show();
//	ShowFrame(m_selectionFrame,&dc);
}
void TLPanel::UpdateSelectionDrag(int x, int y)
{
	int x_ = x-x_offset;
	int y_ = y;
/*	wxClientDC dc(this);
	SetRubberframePen(&dc);
	HideFrame(m_selectionFrame,&dc);*/
	int trackNr = m_TlView->GetTrackByY(y_);
	if (trackNr<0) {
		m_dragImage->Hide();
		return;
	}
	m_dragImage->Show();
	m_selectionFrame.y=trackNr*30+TOP_OFFSET_TRACKS;
	m_selectionFrame.x=m_TlView->GetScreenSnapPosition(x_);
	m_dragImage->Move(wxPoint(m_selectionFrame.x,m_selectionFrame.y));
//	ShowFrame(m_selectionFrame,&dc);
}
void TLPanel::EndSelectionDrag(int x, int y, bool copyOnDrag)
{
	int x_ = x-x_offset;
	int y_ = y;
//	ReleaseMouse();
	m_selectionDrag=false;
	m_dragImage->Hide();
	m_dragImage->EndDrag();
	delete m_dragImage;

//	if (m_selectionFrame.x<0 || y<0 || m_selectionFrame.x>GetSize().GetWidth() || y>GetSize().GetHeight())
//		return;

	m_TlView->EndSelectionDrag(x_, y_, copyOnDrag, x_offset);
/*	wxClientDC dc(this);
	SetRubberframePen(&dc);
	HideFrame(m_selectionFrame,&dc);*/
	ResetScrollBar();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
}
void TLPanel::ShowFrame(wxRect& rect, wxDC* dc)
{
	dc->DrawRectangle(rect.x,rect.y,rect.width,rect.height);
	m_frameVisible = true;
}
void TLPanel::HideFrame(wxRect& rect, wxDC* dc)
{
	if (m_frameVisible) {
		dc->DrawRectangle(rect.x,rect.y,rect.width,rect.height);
	}
	m_frameVisible = false;
}
void TLPanel::OnScroll(wxScrollEvent& event)
{
	m_TlView->SetPosition(event.GetPosition());
	m_CaretPosition = m_TlView->GetCaretPosition();
	UpdateRulerTicks();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif
}
void TLPanel::DrawCaret(wxDC& dc)
{
	if (m_CaretPosition < 5+LEFT_OFFSET_TRACKS) 
		return;
	dc.SetLogicalFunction(wxINVERT);
	dc.SetPen(*wxTRANSPARENT_PEN);
#ifdef __WXMSW__
	dc.DrawRectangle(m_CaretPosition,0,1,GetSize().GetHeight());
#else
	dc.DrawRectangle(m_CaretPosition,0,1,GetSize().GetHeight()/*-m_scrollBar->GetSize().GetHeight()*/);
#endif
}

void TLPanel::ResetScrollBar()
{
	m_scrollBar->SetScrollbar(m_scrollBar->GetThumbPosition(),m_TlView->GetScrollBarThumbSize() ,m_TlView->GetScrollBarRange() , m_TlView->GetScrollBarThumbSize());
}

/*void TLPanel::HideFrame(int x, int y, wxDC* dc)
{
	SetRubberframePen(dc);
	wxRegion region(0,0,GetSize().GetWidth(),GetSize().GetHeight());
	region.Subtract(wxRect(x,y,m_SampleDragItemWidth,25));
	dc->SetClippingRegion(region);

	if (m_DragFrameVisible)
		dc->DrawRectangle(m_DragFrameLastX,m_DragFrameLastY,m_SampleDragItemWidth,25);

	dc->DestroyClippingRegion();
}
void TLPanel::ShowFrame(int x, int y, wxDC* dc)
{
	int trackNr = m_TlView->GetTrackByY(y+y_offset);
	if (trackNr<0) {
		m_DragFrameVisible=false;
		return;
	}
	wxRegion region(0,0,GetSize().GetWidth(),GetSize().GetHeight());
	region.Subtract(wxRect(x,y,m_SampleDragItemWidth,25));
	dc->SetClippingRegion(region);

	m_DragFrameLastX=m_TlView->GetScreenSnapPosition(x);
	m_DragFrameLastY=trackNr*30+5;
	dc->DrawRectangle(m_DragFrameLastX,m_DragFrameLastY,m_SampleDragItemWidth,25);
	m_DragFrameVisible=true;
	dc->DestroyClippingRegion();
}*/
bool TLPanel::New()
{
	m_soundManager->Stop();
	m_loadSaveManager->New();
	Refresh();
	ResetScrollBar();
	return true;
}
bool TLPanel::Load()
{
	m_soundManager->Stop();
	m_loadSaveManager->Load();
	Refresh();
	ResetScrollBar();
	UpdateButtons();
	m_ruler->SetSnap((m_TlView->GetSnapValue()*31)/117600);
	m_ruler->Refresh();
	return true;
}
bool TLPanel::SaveAs()
{
	return m_loadSaveManager->SaveAs();
}
bool TLPanel::Save()
{
	return m_loadSaveManager->Save();
}
bool TLPanel::SaveOnExit()
{
	return m_loadSaveManager->SaveOnExit();
}
void TLPanel::Rewind()
{
//	m_TlView->SetPlaybackPosition(0);
	m_data->SetPlaybackPosition(0);
	wxClientDC dc(this);
	DrawCaret(dc);
	m_CaretPosition = m_TlView->GetCaretPosition();
	DrawCaret(dc);
}
void TLPanel::Play()
{
	m_miniPlayer->Stop();
	m_soundManager->Play();
}
void TLPanel::Stop()
{
	m_soundManager->Stop();
	UpdateCaret();
}
/*void TLPanel::PlaySample(wxString filename, long &length)
{
	m_soundManager->Play(filename,length);
}*/
SoundManager *TLPanel::GetSoundManager()
{
	return m_soundManager;
}
void TLPanel::SetMiniPlayer(MiniPlayerInterface *mp)
{
	m_miniPlayer=mp;
}
void TLPanel::StopAll()
{
	m_soundManager->Stop();
}
void TLPanel::WavExport()
{
	m_soundManager->Stop();
	wxConfig config(wxT("ggseq"));
	wxString lastFolder = config.Read(wxT("LastExportFolder"), wxT(""));
	
	wxString filename = wxFileSelector(wxT("Save WAV-File as"),lastFolder,wxT(""),wxT("wav"),wxT("WAV files (*.wav)|*.wav"),wxSAVE,this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent());
	if ( !filename.empty() ) {
		if (wxFileExists(filename)) {
			wxMessageDialog msg_dlg(m_parent,wxT("File exists!\nOverride?"), wxT("Override File?"), wxYES_NO |wxICON_QUESTION );
			if (msg_dlg.ShowModal()==wxID_NO)
				return;
		}
		wxString dir;
		wxFileName::SplitPath(filename, &dir, NULL, NULL);
		config.Write(wxT("LastExportFolder"),dir);
		m_data->SaveWAV(filename);
	}

/*	wxFileDialog dlg1(this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(), wxT("Save WAV-File as"),lastFolder,wxT(""),wxT("WAV files (*.wav)|*.wav"),wxSAVE);
	if (dlg1.ShowModal()==wxID_OK) {
		wxString filename = dlg1.GetDirectory() +wxT("/")+ dlg1.GetFilename();
		if (wxFileExists(filename)) {
			wxMessageDialog msg_dlg(m_parent,wxT("File exists!\nOverride?"), wxT("Override File?"), wxYES_NO |wxICON_QUESTION );
			if (msg_dlg.ShowModal()==wxID_NO)
				return;
		}
		config.Write(wxT("LastExportFolder"),dlg1.GetDirectory());
		m_data->SaveWAV(filename);
	}
*/
}
bool TLPanel::UpdateCaret()
{
	wxClientDC dc(this);
	if (m_soundManager->Done()) {
		m_soundManager->Stop();
		DrawCaret(dc);
		m_CaretPosition = m_TlView->GetCaretPosition();
		DrawCaret(dc);
		return false;
	}
//	SetCaretPosition(m_soundManager->GetPlaybackPosition());
	DrawCaret(dc);
	int pos=m_soundManager->GetPosition();
	int pos2=(pos/117600)*31;
	if (pos2<m_scrollBar->GetThumbPosition()) {
		m_scrollBar->SetThumbPosition(pos2);
		m_TlView->SetPosition(m_scrollBar->GetThumbPosition());
#ifdef __WXMSW__
		Refresh(true);
#else
		Refresh(false);
#endif
	} else if (pos2>=m_scrollBar->GetThumbPosition()+m_scrollBar->GetThumbSize()) {
		int newPos = m_scrollBar->GetThumbPosition()+m_scrollBar->GetThumbSize();
		if (newPos>m_scrollBar->GetRange())
			newPos=m_scrollBar->GetRange();
		m_scrollBar->SetThumbPosition(newPos);
		m_TlView->SetPosition(m_scrollBar->GetThumbPosition());
#ifdef __WXMSW__
		Refresh(true);
#else
		Refresh(false);
#endif
	}
	m_CaretPosition = m_TlView->GetCaretPosition(pos);

/*	int ScreenWidth=GetSize().GetWidth();
	int DataLength=m_scrollBar->GetThumbSize();
	int ScrollPosition=m_scrollBar->GetThumbPosition();
	int PositionOnScreen = ((Position-ScrollPosition)*ScreenWidth)/DataLength;
	if (PositionOnScreen>ScreenWidth) {
		if (Position<m_scrollBar->GetRange())
			m_scrollBar->SetThumbPosition(Position);
		else
			m_scrollBar->SetThumbPosition(m_scrollBar->GetRange());
		m_TlView->SetPosition(m_scrollBar->GetThumbPosition());
		Refresh(false);
		ScrollPosition=m_scrollBar->GetThumbPosition();
		PositionOnScreen = ((Position-ScrollPosition)*ScreenWidth)/DataLength;
	}
	wxClientDC dc(this);
	if (m_CaretVisible) {
		DrawCaret(dc);
		m_CaretPosition=PositionOnScreen;
		DrawCaret(dc);
	}*/
	
	DrawCaret(dc);
	return true;
}

wxString TLPanel::GetFilename()
{
	if (m_data->GetFilename().IsEmpty())
		return wxT("Unnamed");
	return m_data->GetFilename();
}
void TLPanel::SetColours(wxString path)
{
	TLSelectColourDialog dlg(this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(),-1,m_data->GetColourManager(),wxT("Setup Colours for Folders"));
	dlg.SetPath(path);
	dlg.SetSize(wxSize(500,400));
	dlg.Centre();
	if (dlg.ShowModal()==wxID_OK) {
		dlg.SetColours(m_data->GetColourManager());
#ifdef __WXMSW__
		Refresh(true);
#else
		Refresh(false);
#endif
	}
}
void TLPanel::SetSnap()
{
	TLSetSnapDialog dlg(this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(),-1,m_TlView->GetSnapValue()/2,wxT("Set Sample Snapping Position"));
	dlg.Centre();
	if (dlg.ShowModal()==wxID_OK) {
		m_TlView->SetSnapValue(dlg.m_SnapPosition*2);
	}
	m_ruler->SetSnap((m_TlView->GetSnapValue()*31)/117600);
	m_ruler->Refresh();
}

void TLPanel::DropFileAt(int x, int y, wxString filename)
{
	int x_=x;
	int y_=y;
	ScreenToClient(&x_,&y_);
	if (x_<0 || y_<0 || x_>GetSize().GetWidth() || y_>GetSize().GetHeight())
		return;
	int trackNr = m_TlView->GetTrackByY(y_);
	if (trackNr<0)
		return;
	m_TlView->AddItem(filename,x_,trackNr);
	ResetScrollBar();
#ifdef __WXMSW__
	Refresh(true);
#else
	Refresh(false);
#endif

}

void TLPanel::UpdateButtons()
{
	int i=0;
	for ( TLTrackList::Node *node = m_data->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		m_buttons[i]->SetMute(current->IsMuted());
		m_dials[i]->SetValue((int)(current->GetVolume()*100));
		i++;
	}

}
void TLPanel::SetUpdateListener(UpdateListener *updateListener)
{
	m_data->SetUpdateListener(updateListener);
}
void TLPanel::UpdateRulerTicks()
{
//	m_TlView->m_SnapPosition;
/*	long ab=(m_TlView->m_SnapPosition*31)/117600;
	long cd=m_TlView->GetPosition();
	long index_c=cd/ab;
	wxString out;
	out << index_c;
//	puts(out.mb_str());
//	m_ruler
	wxClientDC dc(m_ruler);
	dc.DrawText(out,2,-2);*/
//	m_ruler->SetSnap((m_TlView->m_SnapPosition*31)/117600);
	m_ruler->SetPosition(m_TlView->GetPosition());
//	m_ruler->Refresh();
}
