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
#include <wx/dcbuffer.h>
#include <wx/dir.h>
//#include <iostream>

#include "tinyxml.h"
#include "stuff.h"
#include "TLView.h"
#include "TLPanel.h"
#include "TLItem.h"
#include "TLSample.h"
#include "TLMiniFiler2.h"
#include "TLLoadSaveManager.h"
#include "SoundManager.h"
#include "TLData.h"
#include "TLMuteButton.h"
#include "mute_off.xpm"
#include "dial.h"
#include "TLTrackVolumeDial.h"
#include "TLTrack.h"
#include "MiniPlayer.h"
#include "Ruler.h"
#include "BigScrollBar.h"
#include "GgseqDocManager.h"
#include "PreferencesDialog.h"

#define LEFT_OFFSET_TRACKS 52
#define TOP_OFFSET_TRACKS 3

enum
{
		ID_ScrollBar = 1,
};

BEGIN_EVENT_TABLE(TLPanel, wxPanel)
 	EVT_PAINT(TLPanel::OnPaint)
	EVT_LEFT_UP(TLPanel::OnMouseUp)
	EVT_RIGHT_UP(TLPanel::OnMouseUp)
	EVT_RIGHT_DOWN(TLPanel::OnMouseDown)
	EVT_LEFT_DOWN(TLPanel::OnMouseDown)
	EVT_LEFT_DCLICK(TLPanel::OnDoubleClick)
	EVT_MOTION(TLPanel::OnMouseMotion)
	EVT_SIZE(TLPanel::OnSize)
	EVT_COMMAND_SCROLL_THUMBTRACK(ID_ScrollBar,TLPanel::OnScroll)
	EVT_ERASE_BACKGROUND(TLPanel::OnEraseBackground)
END_EVENT_TABLE()

TLPanel::TLPanel(wxWindow* parent, BigScrollBar *scrollbar, Ruler *ruler, wxScrollBar *scrollbar2, wxWindowID id)
:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
	m_TlView = 0;
	m_data = new TLData();
	m_data->SetPanel( this );
	m_TlView = new TLView(m_data);
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_TlView->SetPosition( 0 );
	m_TlView->UpdateDialsAndButtons();
	m_loadSaveManager = new TLLoadSaveManager( this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(),m_data); //FIXME
	new SoundManager(m_data);
	m_scrollBar = scrollbar;
	m_scrollBar2 = scrollbar2;
	m_ruler = ruler;
	m_ruler->SetSnap( (int)( m_TlView->GetSnapValue() / m_TlView->GetRealZoom() ) );
	ResetScrollBar();
	m_TlView->SetSize( GetSize().GetWidth(), GetSize().GetHeight() );
	m_CaretPosition = 0;
	m_CaretVisible  = true;
	m_sampleDrag    = false;
	m_rubberDrag    = false;
	m_selectionDrag = false;
	m_frameVisible  = false;
	UpdateButtons();
	SetSizeHints( 100, 200 );
	m_ruler->SetListener( m_data );
}
void TLPanel::AddControls(TLTrack *track)
{
#ifdef __WXMSW__
	track->m_muteButton = new TLMuteButton( this, -1, track, wxT("M"), wxDefaultPosition, wxSize(25,25) );
#else
	track->m_muteButton = new TLMuteButton( this, -1, track, mute_off_xpm, wxDefaultPosition, wxSize(25,25) );
#endif
	track->m_volumeDial = new TLTrackVolumeDial(this, -1, track, wxDefaultPosition, wxSize(25,25) );
}
void TLPanel::DeleteControls(TLTrack *track)
{
	track->m_muteButton->Destroy();
	track->m_volumeDial->Destroy();
	m_TlView->UpdateDialsAndButtons();
}
TLPanel::~TLPanel()
{
	while(m_data->GetTrackCount()>0) { // Muss hier gelöscht werden sonst segfault. Sollte TODO: Sollte aber "sauber" gelöst werden
		m_data->DeleteTrack(0);
	}
	delete m_TlView;
	delete g_ggseqProps.GetSoundManager();
	g_ggseqProps.SetSoundManager(0);
	delete m_loadSaveManager;
}
void TLPanel::OnPaint( wxPaintEvent& event )
{
	int width, height;
	GetClientSize( &width, &height );
	wxBufferedPaintDC dc( this );
	dc.SetBrush( wxBrush( GetBackgroundColour(), wxSOLID ) );
	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle( 0, 0, width, height );
	m_TlView->Draw( dc );
	DrawCaret( dc );
}
void TLPanel::OnEraseBackground( wxEraseEvent& event )
{
}
void TLPanel::OnSize( wxSizeEvent& event )
{
	m_TlView->SetSize( GetSize().GetWidth(), GetSize().GetHeight() );
	ResetScrollBar();
}
void TLPanel::OnMouseMotion( wxMouseEvent& event )
{
	if ( event.m_shiftDown )
		m_TlView->SuspendSnap();
	else
		m_TlView->ResumeSnap();
	if ( m_sampleDrag ) {
		UpdateSampleDrag( event.m_x, event.m_y );
		return;
	}
	if ( m_rubberDrag ) {
		UpdateRubberFrame( event.m_x, event.m_y );
		return;
	}
	if ( m_selectionDrag ) {
		UpdateSelectionDrag( event.m_x, event.m_y );
		return;
	}
	if ( event.m_leftDown || event.m_rightDown ) {
		int c;
		int d;
		if ( m_TlView->IsSelectionAt( event.m_x, event.m_y, x_offset, y_offset, c, d ) ) {
			StartSelectionDrag( event.m_x, event.m_y, c, d );
			return;
		}
		int srcTrackNr = m_TlView->GetTrackByY( m_DragY );
		TLItem *srcItem = m_TlView->GetDragItem( m_DragX, m_DragY );
		if ( !srcItem ) {
			if ( event.m_leftDown )
				StartRubberFrame( event.m_x, event.m_y );
			return;
		} else {
			/*item->top item->bottom item->left item->right*/
			//      m_TlView->HandleInternal( srcItem, m_DragX, m_DragX );
			//      m_TlView->GetItemBoundaries
			// srcTrackNr;
			// srcItem;
		}/*Testen ob quadrat unter Mauszeiger*/
		StartSampleDrag( event.m_x, event.m_y, srcTrackNr, srcItem);
	}
}
/*void TLPanel::GetItemRelativeXY(&x, &y)
{
  
}
*/
void TLPanel::OnMouseUp( wxMouseEvent& event )
{
	if ( event.m_shiftDown )
		m_TlView->SuspendSnap();
	else
		m_TlView->ResumeSnap();

	if ( m_rubberDrag ) {
		EndRubberFrame( event.m_x, event.m_y );
		return;
	}
	if ( m_sampleDrag ) {
		EndSampleDrag( event.m_x, event.m_y, event.RightUp() );
		return;
	}
	if ( m_selectionDrag ) {
		EndSelectionDrag( event.m_x, event.m_y, event.RightUp() );
		return;
	}
	if ( event.LeftUp() ) {
		m_TlView->ClearSelection();
		m_TlView->SelectTrack( event.m_y );
		Refresh();
		m_TlView->SetPlaybackPosition( event.m_x );
		wxClientDC dc( this );
		DrawCaret( dc );
		m_CaretPosition = m_TlView->GetCaretPosition();
		DrawCaret( dc );
	}
}
void TLPanel::OnMouseDown( wxMouseEvent& event )
{
	//TODO: Prüfen ob Cursor im kleinen menü-Eck ist, und ggfls. POP-Up öffnen
	m_DragX = event.m_x;
	m_DragY = event.m_y;
	if( g_ggseqProps.GetMiniPlayer() )
		g_ggseqProps.GetMiniPlayer()->Stop();
}
void TLPanel::OnDoubleClick( wxMouseEvent& event )
{
	m_TlView->ClearSelection();
	Refresh();
	int trackNr = m_TlView->GetTrackByY( event.m_y );
	if ( trackNr < 0 )
		return;
	TLSample *sample = m_TlView->GetSample( event.m_x, trackNr );
	if ( sample !=NULL ) {
		if ( g_ggseqProps.GetMiniPlayer() ) {
			g_ggseqProps.GetMiniPlayer()->SetSample( sample );
			g_ggseqProps.GetMiniPlayer()->Play();
		}
	}
}
void TLPanel::StartRubberFrame( int x, int y )
{
	CaptureMouse();
	m_TlView->ClearSelection();
	Refresh();
	m_rubberDrag = true;
	m_rubberFrame = wxRect( m_DragX, m_DragY, x - m_DragX, y - m_DragY );
}
void TLPanel::UpdateRubberFrame( int x, int y )
{
	wxClientDC dc( this );
	SetRubberframePen( &dc );
	HideFrame( m_rubberFrame, &dc );
	m_rubberFrame.width = x - m_rubberFrame.x;
	m_rubberFrame.height = y - m_rubberFrame.y;
	ShowFrame( m_rubberFrame, &dc );
	return;
}
void TLPanel::EndRubberFrame( int x, int y )
{
	ReleaseMouse();
	m_rubberDrag = false;
	wxClientDC dc( this );
	SetRubberframePen( &dc );
	HideFrame( m_rubberFrame, &dc );
	if ( m_rubberFrame.width < 0 ) {
		m_rubberFrame.width =- m_rubberFrame.width;
		m_rubberFrame.x = m_rubberFrame.x - m_rubberFrame.width;
	}
	if ( m_rubberFrame.height < 0 ) {
		m_rubberFrame.height =- m_rubberFrame.height;
		m_rubberFrame.y = m_rubberFrame.y - m_rubberFrame.height;
	}

	m_TlView->Select(
		m_rubberFrame.x, m_rubberFrame.y,
		m_rubberFrame.width, m_rubberFrame.height
		);
	Refresh();
	return;
}
void TLPanel::SetRubberframePen( wxDC* dc )
{
	dc->SetLogicalFunction( wxINVERT );
	dc->SetBrush( *wxTRANSPARENT_BRUSH );
	dc->SetPen( wxPen( *wxBLACK, 1, wxSHORT_DASH ) );
}
void TLPanel::StartSampleDrag( int x, int y, int srcTrackNr, TLItem* srcItem )
{
	m_TlView->ClearSelection();
	Refresh();
	Update();
	m_SampleDragSrcTrackNr = srcTrackNr;
	m_DragItem = srcItem;

	x_offset = m_DragX - m_TlView->FromTLtoScreenX( m_DragItem->GetPosition() );
	y_offset = m_DragY - m_SampleDragSrcTrackNr * 30 - TOP_OFFSET_TRACKS + m_TlView->GetYScrollPosition();

	m_SampleDragItemWidth = (int)( m_DragItem->GetLength() / m_TlView->GetRealZoom() );
	wxBitmap bmp1( m_SampleDragItemWidth, 25 );
	wxMemoryDC dc;
	dc.SelectObject( bmp1 );
	m_DragItem->GetSample()->Draw( dc, m_TlView->GetRealZoom() );
#ifdef __WXMSW__ 
	m_dragImage = new wxGenericDragImage( bmp1 );
#else
	m_dragImage = new wxDragImage( bmp1 );
#endif
	m_dragImage->BeginDrag( wxPoint( x_offset, y_offset ), this );
	m_dragImage->Move( wxPoint( x, y ) );
	m_oldXImgPos = x - x_offset;
	m_oldYImgPos = y - y_offset;
	m_dragImage->Show();
	m_sampleDrag = true;
}
void TLPanel::UpdateSampleDrag( int x, int y )
	/* This whole Region Stuff is neccessary to display the dashed
	 * Frame below the dragged Sample*/
{
	wxClientDC dc( this );
	SetRubberframePen( &dc );
	int new_x = x - x_offset;
	int new_y = y - y_offset;
	wxRegion region( 0, 0, GetSize().GetWidth(), GetSize().GetHeight() );
	region.Subtract( wxRect( m_oldXImgPos, m_oldYImgPos, m_SampleDragItemWidth, 25 ) );
	m_oldXImgPos = x - x_offset;
	m_oldYImgPos = y - y_offset;
	dc.SetClippingRegion( region );
	HideFrame( m_sampleFrame, &dc );
	dc.DestroyClippingRegion();
	m_dragImage->Move( wxPoint( x, y ) );
	int trackNr = m_TlView->GetTrackByY( y );
	if ( trackNr < 0 ) {
		return;
	}
	region = wxRegion( 0, 0, GetSize().GetWidth(), GetSize().GetHeight() );
	region.Subtract( wxRect( new_x, new_y, m_SampleDragItemWidth, 25 ) ); //TODO
	dc.SetClippingRegion( region );
	new_x = m_TlView->GetScreenSnapPosition( new_x );
	new_y = trackNr * 30 + TOP_OFFSET_TRACKS - m_TlView->GetYScrollPosition(); //TODO
	m_sampleFrame = wxRect( new_x, new_y, m_SampleDragItemWidth, 25 ); //TODO
	ShowFrame( m_sampleFrame, &dc );
	dc.DestroyClippingRegion();
	return;
}
void TLPanel::EndSampleDrag( int x, int y, bool copyOnDrag )
{
	m_sampleDrag = false;
	m_dragImage->Hide();
	m_dragImage->EndDrag();
	delete m_dragImage;
	wxClientDC dc( this );
	SetRubberframePen( &dc );
	HideFrame( m_sampleFrame, &dc );
	m_TlView->DoDrop( x - x_offset, y, m_DragItem, m_SampleDragSrcTrackNr, x_offset, copyOnDrag );
	ResetScrollBar();
	Refresh();
}
void TLPanel::StartSelectionDrag( int x, int y, int width, int height )
{
	int x_ = x-x_offset;
	int y_ = y;
	m_selectionDrag = true;
	wxBitmap bitmap( width, height );
	wxMemoryDC *dc = new wxMemoryDC();
	dc->SelectObject( bitmap );
	wxPen pen( *wxBLACK, 1, wxLONG_DASH );
	dc->SetPen( pen );
	dc->SetBrush( *wxGREEN_BRUSH );
	dc->DrawRectangle( 0, 0, width, height );
	wxBrush ab;
	ab.SetStyle( wxTRANSPARENT );
	dc->SetBrush( ab );
	pen.SetColour( *wxWHITE );
	pen.SetWidth( 3 );
	pen.SetStyle( wxSOLID );
	dc->SetPen( pen );
	m_TlView->DrawSelection( dc );
	pen.SetWidth( 1 );
	pen.SetColour( *wxBLACK );
	pen.SetStyle( wxLONG_DASH );
	dc->SetPen( pen );
	m_TlView->DrawSelection( dc );
	delete dc;
	wxMask *mask = new wxMask( bitmap, *wxGREEN );
	bitmap.SetMask( mask );
#ifdef __WXMSW__ 
	m_dragImage = new wxGenericDragImage( bitmap );
#else
	m_dragImage = new wxDragImage( bitmap );
#endif
	m_dragImage->BeginDrag( wxPoint( 0, 0 ), this );
	m_selectionFrame = wxRect( x_, y_, width, height );
	int trackNr = m_TlView->GetTrackByY( y_ );
	if ( trackNr >= 0 ) {
		m_selectionFrame.y = trackNr * 30 + TOP_OFFSET_TRACKS - m_TlView->GetYScrollPosition(); //TODO
	} else {
		return;
	}
	m_selectionFrame.x = m_TlView->GetScreenSnapPosition( x_ );
	m_dragImage->Move( wxPoint( m_selectionFrame.x, m_selectionFrame.y ) );
	m_dragImage->Show();
}
void TLPanel::UpdateSelectionDrag( int x, int y )
{
	int x_ = x-x_offset;
	int y_ = y;
	int trackNr = m_TlView->GetTrackByY( y_ );
	if ( trackNr < 0 ) {
		m_dragImage->Hide();
		return;
	}
	m_dragImage->Show();
	m_selectionFrame.y = trackNr * 30 + TOP_OFFSET_TRACKS - m_TlView->GetYScrollPosition(); //TODO
	m_selectionFrame.x = m_TlView->GetScreenSnapPosition( x_ );
	m_dragImage->Move( wxPoint( m_selectionFrame.x, m_selectionFrame.y ) );
}
void TLPanel::EndSelectionDrag( int x, int y, bool copyOnDrag )
{
	int x_ = x - x_offset;
	int y_ = y;
	m_selectionDrag = false;
	m_dragImage->Hide();
	m_dragImage->EndDrag();
	delete m_dragImage;
	m_TlView->EndSelectionDrag( x_, y_, copyOnDrag, x_offset );
	ResetScrollBar();
	Refresh();
}
void TLPanel::ShowFrame( wxRect& rect, wxDC* dc )
{
	dc->DrawRectangle( rect.x, rect.y, rect.width, rect.height);
	m_frameVisible = true;
}
void TLPanel::HideFrame( wxRect& rect, wxDC* dc )
{
	if ( m_frameVisible ) {
		dc->DrawRectangle( rect.x, rect.y, rect.width, rect.height );
	}
	m_frameVisible = false;
}
void TLPanel::OnScroll2( wxScrollEvent& event )
{
	m_TlView->SetYScrollPosition( event.GetPosition() );
	this->Freeze();
	m_TlView->UpdateDialsAndButtons();
	Refresh();
	this->Thaw();
}
void TLPanel::OnScroll( wxScrollEvent& event )
{
	m_TlView->SetPosition( m_scrollBar->GetBigThumbPosition() );
	m_CaretPosition = m_TlView->GetCaretPosition();
	UpdateRulerTicks();
	Refresh();
}
void TLPanel::DrawCaret(wxDC& dc)
{
	if ( m_CaretPosition < 5 + LEFT_OFFSET_TRACKS )
		return;
	dc.SetLogicalFunction( wxINVERT );
	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle( m_CaretPosition, 0, 1, GetSize().GetHeight() );
}

void TLPanel::ResetScrollBar()
{
	m_scrollBar->SetBigScrollBar(
		m_TlView->GetPosition(),
		m_TlView->GetScrollBarThumbSize(),
		m_TlView->GetScrollBarRange()
		);
	int width, height;
	GetSize( &width, &height );
	m_scrollBar2->SetScrollbar(
		m_scrollBar2->GetThumbPosition(),
		height, TOP_OFFSET_TRACKS + 30 * m_data->GetTrackCount(),
		height
		);
	if ( m_scrollBar2->GetThumbPosition() >
	     m_scrollBar2->GetRange() - m_scrollBar2->GetThumbSize() ) {
		m_scrollBar2->SetThumbPosition(
			m_scrollBar2->GetRange()-m_scrollBar2->GetThumbSize()
			);
		if ( m_scrollBar2->GetThumbSize() > m_scrollBar2->GetRange() ) {
			m_scrollBar2->SetThumbPosition( 0 );
		}
		m_TlView->SetYScrollPosition( m_scrollBar2->GetThumbPosition() );
		this->Freeze();
		m_TlView->UpdateDialsAndButtons();
		Refresh();
		this->Thaw();
	}

}
bool TLPanel::New()
{
	g_ggseqProps.GetSoundManager()->Stop();
	m_loadSaveManager->New();
	Refresh();
	ResetScrollBar();
	return true;
}
bool TLPanel::Load()
{
	g_ggseqProps.GetSoundManager()->Stop();
	m_loadSaveManager->Load();
	Refresh();
	ResetScrollBar();
	UpdateCaret();
	UpdateButtons();
	m_ruler->SetSnap( (long)( m_TlView->GetSnapValue() / m_TlView->GetRealZoom() ) );
	m_ruler->Refresh();
	return true;
}
bool TLPanel::Load( wxString& filename )
{
	g_ggseqProps.GetSoundManager()->Stop();
	if ( m_data->UnsavedChanges() ) {
		wxMessageDialog msg_dlg(
			m_parent,wxT("Save Changes?"),
			wxT("Unsaved Changes"),
			wxYES_NO | wxCANCEL|wxICON_QUESTION
			);
		int result = msg_dlg.ShowModal();
		switch ( result ) {
			case wxID_CANCEL:
				return FALSE;
			case wxID_YES:
				if ( !m_loadSaveManager->Save() )
					return FALSE;
				break;
		}
	}
	m_data->Load( filename );
	Refresh();
	ResetScrollBar();
	UpdateButtons();
	m_ruler->SetSnap( (long)( m_TlView->GetSnapValue() / m_TlView->GetRealZoom() ) );
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
	m_data->SetPlaybackPosition( 0 );
	wxClientDC dc( this );
	DrawCaret( dc );
	m_CaretPosition = m_TlView->GetCaretPosition();
	DrawCaret( dc );
}
void TLPanel::Play()
{
	g_ggseqProps.GetMiniPlayer()->Stop();
	g_ggseqProps.GetSoundManager()->Play();
}
void TLPanel::Stop()
{
	g_ggseqProps.GetSoundManager()->Stop();
	UpdateCaret();
}
void TLPanel::StopAll()
{
	g_ggseqProps.GetSoundManager()->Stop();
}
void TLPanel::ImportPackage( wxString package, wxString contents )
{
	wxFileName fn( package );
	wxString cont_path = contents + wxFileName::GetPathSeparator() + fn.GetName();
	wxMkdir( cont_path );
	wxSetWorkingDirectory( cont_path );
	wxExecute( wxString( wxT("unzip ") ) + package, wxEXEC_SYNC );
	wxArrayString files;
	wxDir::GetAllFiles( cont_path, &files, wxT("*.ggseq") );
	wxString ggseq_file = files[0];
	TiXmlDocument doc( ggseq_file.mb_str() );
	doc.LoadFile();
	TiXmlElement *element = doc.RootElement();
	TiXmlNode *node = element->FirstChild( "samples" );
	node = node->FirstChild( "sample" );
	element = node->ToElement();
	for ( ; element; ) {
		node = element->FirstChild();
		TiXmlText *text = node->ToText();
		wxString txt( text->Value(), *wxConvCurrent );
		txt = cont_path + wxFileName::GetPathSeparator() + txt;
		text->SetValue( txt.mb_str() );
		element=element->NextSiblingElement( "sample" );
	}
	doc.SaveFile();
	Load( ggseq_file );
}
void TLPanel::ExportPackage()
{
	wxString filename = wxFileSelector(
				wxT("Export Package as"),
				wxT(""), wxT(""), wxT("ggz"),
				wxT("Gungirl Package files (*.ggz)|*.ggz"),
				wxSAVE,
				this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent()
				);
	if ( !filename.empty() ) {
		if ( wxFileExists( filename ) ) {
			wxMessageDialog msg_dlg(
					m_parent,
					wxT("File exists!\nOverride?"),
					wxT("Override File?"),
					wxYES_NO |wxICON_QUESTION
					);
			if ( msg_dlg.ShowModal() == wxID_NO )
				return;
		}
		m_data->ExportPackage( filename );
	}



}
void TLPanel::WavExport()
{
	g_ggseqProps.GetSoundManager()->Stop();
	wxConfigBase *conf = wxConfigBase::Get();
	wxString lastFolder = conf->Read( wxT("LastExportFolder"), wxT("") );
	
	wxString filename = wxFileSelector(
				wxT("Save WAV-File as"),
				lastFolder,wxT(""),wxT("wav"),
				wxT("WAV files (*.wav)|*.wav"),
				wxSAVE,
				this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent()
				);
	if ( !filename.empty() ) {
		if ( wxFileExists(filename) ) {
			wxMessageDialog msg_dlg(
					m_parent,wxT("File exists!\nOverride?"),
					wxT("Override File?"),
					wxYES_NO |wxICON_QUESTION
					);
			if ( msg_dlg.ShowModal() == wxID_NO )
				return;
		}
		wxString dir;
		wxFileName::SplitPath( filename, &dir, NULL, NULL );
		conf->Write( wxT("LastExportFolder"), dir );
		m_data->SaveWAV( filename );
	}

}
bool TLPanel::UpdateCaret()
{
	wxClientDC dc( this );
	if ( g_ggseqProps.GetSoundManager()->Done() ) {
		g_ggseqProps.GetSoundManager()->Stop();
		DrawCaret( dc );
		m_CaretPosition = m_TlView->GetCaretPosition();
		DrawCaret( dc );
		return false;
	}
	DrawCaret( dc );
	gg_tl_dat pos = g_ggseqProps.GetSoundManager()->GetPosition();
	gg_tl_dat pos2 = ( pos / 117600 ) * 31;
	if ( pos2 < m_scrollBar->GetBigThumbPosition() ) {
		m_scrollBar->SetBigThumbPosition( pos2 );
		m_TlView->SetPosition( m_scrollBar->GetBigThumbPosition() );
		UpdateRulerTicks();
		Refresh();
	} else if ( pos2 >= m_scrollBar->GetBigThumbPosition() + m_scrollBar->GetBigThumbSize() ) {
		//Hier wird gesprungen, sollte aber abhängig von der Fensterbreite passieren
		gg_tl_dat newPos = m_scrollBar->GetBigThumbPosition() + m_scrollBar->GetBigThumbSize();
		if ( newPos > m_scrollBar->GetBigRange() - m_scrollBar->GetBigThumbSize() )
			newPos = m_scrollBar->GetBigRange() - m_scrollBar->GetBigThumbSize();
		m_scrollBar->SetBigThumbPosition( newPos );
		m_TlView->SetPosition( newPos );
		UpdateRulerTicks();
		Refresh();
	}
	m_CaretPosition = m_TlView->GetCaretPosition( pos );
	DrawCaret( dc );
	return true;
}

wxString TLPanel::GetFilename()
{
	if ( m_data->GetFilename().IsEmpty() )
		return wxT("Unnamed");
	return m_data->GetFilename();
}
void TLPanel::SetPrefs()
{
	PreferencesDialog prefs(
		this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(),
		-1, m_TlView->GetSnapValue() / 2,
		m_data->GetColourManager(), wxT("Preferences")
		);
	prefs.Centre();
	if ( prefs.ShowModal() == wxID_OK ) {
		m_TlView->SetSnapValue( prefs.GetFrameSnap() * 2 );
		prefs.SaveColours( m_data->GetColourManager() );
		wxConfigBase *conf = wxConfigBase::Get();
		conf->SetPath( wxT("/") );
		conf->Write( wxT("LoadLastProject"), prefs.GetLoadLastCheckbox()->GetValue() );
		Refresh();
	}
	m_ruler->SetSnap( (long)( m_TlView->GetSnapValue() / m_TlView->GetRealZoom() ) );
	m_ruler->Refresh();
	m_scrollBar->Enable(); /*TODO: Evil Hack*/
}

void TLPanel::DropFileAt( int x, int y, wxString filename )
{
	int x_ = x;
	int y_ = y;
	ScreenToClient( &x_, &y_ );
	if (x_ < 0 || y_ < 0 || x_ > GetSize().GetWidth() || y_ > GetSize().GetHeight() )
		return;
	int trackNr = m_TlView->GetTrackByY( y_ );
	if ( trackNr < 0 )
		return;
	m_TlView->AddItem( filename, x_, trackNr );
	ResetScrollBar();
	Refresh();
}

void TLPanel::UpdateButtons()
{
	for ( TLTrackList::Node *node = m_data->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->m_muteButton->SetMute( current->IsMuted() );
		current->m_volumeDial->SetValue( (int)( current->GetVolume() * 100 ) );
	}
}
void TLPanel::SetUpdateListener( UpdateListener *updateListener )
{
	m_data->SetUpdateListener( updateListener );
}
void TLPanel::UpdateRulerTicks()
{
	m_ruler->SetPosition( m_TlView->GetPosition() );
}
void TLPanel::SetMasterVolume( float volume ) { m_data->SetMasterVolume( volume ); }

void TLPanel::Undo()
{
	m_TlView->Undo();
	m_TlView->UpdateDialsAndButtons();
	ResetScrollBar();
	Refresh();
}
void TLPanel::Redo()
{
	m_TlView->Redo();
	m_TlView->UpdateDialsAndButtons();
	ResetScrollBar();
	Refresh();
}
void TLPanel::AddTrack()
{
	m_TlView->m_docManager->SubmitCommand( new GgseqAddTrackCommand( m_data, -1, this ) );
	m_TlView->UpdateDialsAndButtons();
	ResetScrollBar();
	Refresh();
}
void TLPanel::DeleteTrack()
{
	TLTrack *track=0;
	for ( TLTrackList::Node *node = m_data->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		if (current->GetSelected())
			track=current;
	}
	if (track==0)
		return;
	m_TlView->m_docManager->SubmitCommand(
			new GgseqDeleteTrackCommand( m_data, track, this )
			);
	ResetScrollBar();
	Refresh();


}
void TLPanel::SetZoom( float zoom )
{
	m_TlView->SetZoom( zoom );
	ResetScrollBar();
	m_ruler->SetSnap( (long)( m_TlView->GetSnapValue() / m_TlView->GetRealZoom() ) );
	m_TlView->SetPosition( m_scrollBar->GetBigThumbPosition() );
}
