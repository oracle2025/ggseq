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
#include "MouseDragHandler.h"
#include "SelectionDragHandler.h"
#include "RubberDragHandler.h"
#include "EnvelopeDragHandler.h"  //TODO Alle Handler in MouseDragHandler.h?
#include "TrimmerDialog.h"

#define LEFT_OFFSET_TRACKS 52

enum
{
		ID_ScrollBar = 1,
		ID_PopupMenu,
		ID_PopupMenuEnvelope
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
	EVT_MENU(ID_PopupMenu, TLPanel::OnEdit)
	EVT_MENU(ID_PopupMenuEnvelope, TLPanel::OnToggleEnvelope)
END_EVENT_TABLE()

TLPanel::TLPanel(wxWindow* parent, BigScrollBar *scrollbar, Ruler *ruler, wxScrollBar *scrollbar2, wxWindowID id)
:wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
	m_view = 0;
	m_EditItem = 0;
	m_data = new TLData();
	m_data->SetPanel( this );
	m_view = new TLView(m_data);
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_data->AddTrack( -1 );
	m_view->SetPosition( 0 );
	m_view->UpdateDialsAndButtons();
	m_loadSaveManager = new TLLoadSaveManager( this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(),m_data); //FIXME
	new SoundManager(m_data);
	m_scrollBar = scrollbar;
	m_scrollBar2 = scrollbar2;
	m_ruler = ruler;
	m_ruler->SetSnap( (int)( m_view->GetSnapValue() / m_view->GetRealZoom() ) );
	ResetScrollBar();
	m_view->SetSize( GetSize().GetWidth(), GetSize().GetHeight() );
	m_CaretPosition = 0;
	m_CaretVisible  = true;
	//m_rubberDrag    = false;
	//m_frameVisible  = false;
	UpdateButtons();
	SetSizeHints( 100, 200 );
	m_ruler->SetListener( m_data );
  m_dragHandler = NULL;
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
	m_view->UpdateDialsAndButtons();
}
TLPanel::~TLPanel()
{
	while(m_data->GetTrackCount()>0) { // Muss hier gelöscht werden sonst segfault. Sollte TODO: Sollte aber "sauber" gelöst werden
		m_data->DeleteTrack(0);
	}
	delete m_view;
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
	m_view->Draw( dc );
	DrawCaret( dc );
}
void TLPanel::OnEraseBackground( wxEraseEvent& event )
{
}
void TLPanel::OnSize( wxSizeEvent& event )
{
	m_view->SetSize( GetSize().GetWidth(), GetSize().GetHeight() );
	ResetScrollBar();
}
void TLPanel::OnMouseMotion( wxMouseEvent& event )
{
	if ( event.m_shiftDown ) {
		m_view->SuspendSnap();
	} else {
		m_view->ResumeSnap();
	}
	if ( m_dragHandler ) {
		m_dragHandler->OnDrag( event.m_x, event.m_y );
		return;
	}
	if ( ! ( event.m_leftDown || event.m_rightDown ) )
		return;
	int c;
	int d;
	int xOffset;
	int yOffset;
	if ( m_view->IsSelectionAt( event.m_x, event.m_y, xOffset, yOffset, c, d ) ) {
		m_dragHandler = new SelectionDragHandler (
			this, m_view, c, d,
			event.m_x, event.m_y,
			xOffset, yOffset
			);
		return;
	}
	TLItem *item = m_view->GetDragItem( m_DragX, m_DragY );
	if ( !item ) {
		if ( event.m_leftDown ) {
			m_dragHandler = new RubberDragHandler (
				this, m_view, m_DragX, m_DragY,
				event.m_x, event.m_y
				);
		}
		return;
	}
	wxRect tmp_rect = m_view->GetItemBoundaries( item );
	wxRect *envelopeHandle = item->TouchingEnvelopeCtrl( event.m_x - tmp_rect.x,
			event.m_y - tmp_rect.y );
	if ( envelopeHandle && item->m_toggleEnvelope ) {
		m_dragHandler = new EnvelopeDragHandler(
			this, m_view, item, event.m_x, event.m_y,
			envelopeHandle
			);
		return;
	}
	m_dragHandler = new SampleDragHandler( this, item, m_view, event.m_x, event.m_y );
}
void TLPanel::OnMouseUp( wxMouseEvent& event )
{
	if ( event.m_shiftDown )
		m_view->SuspendSnap();
	else
		m_view->ResumeSnap();

/*	if ( m_rubberDrag ) {
		EndRubberFrame( event.m_x, event.m_y );
		return;
	}*/
  if ( m_dragHandler ) {
    m_dragHandler->OnDrop( event.m_x, event.m_y, event.ControlDown()/*event.RightUp()*/ );
    delete m_dragHandler;
    m_dragHandler = NULL;
    ResetScrollBar();
    Refresh();
    return;
  }
	if ( event.LeftUp() ) {
		m_view->ClearSelection();
		m_view->SelectTrack( event.m_y );
		Refresh();
		m_view->SetPlaybackPosition( event.m_x );
		wxClientDC dc( this );
		DrawCaret( dc );
		m_CaretPosition = m_view->GetCaretPosition();
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
	if ( event.RightDown() )
	{
		m_EditItem = m_view->GetDragItem( m_DragX, m_DragY );
		if (!m_EditItem)
			return;
		wxMenu popup;
		popup.Append( ID_PopupMenu, wxT("Edit Sample...") );
		popup.AppendCheckItem( ID_PopupMenuEnvelope, wxT("Envelope") );
		popup.Check( ID_PopupMenuEnvelope, m_EditItem->m_toggleEnvelope );
		PopupMenu( &popup, event.m_x, event.m_y );
	}
}
void TLPanel::OnDoubleClick( wxMouseEvent& event )
{
	m_view->ClearSelection();
	Refresh();
	int trackNr = m_view->GetTrackByY( event.m_y );
	if ( trackNr < 0 )
		return;
	TLSample *sample = m_view->GetSample( event.m_x, trackNr );
	if ( sample !=NULL ) {
		if ( g_ggseqProps.GetMiniPlayer() ) {
			g_ggseqProps.GetMiniPlayer()->SetSample( sample );
			g_ggseqProps.GetMiniPlayer()->Play();
		}
	}
}
void TLPanel::OnScroll2( wxScrollEvent& event )
{
	m_view->SetYScrollPosition( event.GetPosition() );
	this->Freeze();
	m_view->UpdateDialsAndButtons();
	Refresh();
	this->Thaw();
}
void TLPanel::OnScroll( wxScrollEvent& event )
{
	m_view->SetPosition( m_scrollBar->GetBigThumbPosition() );
	m_CaretPosition = m_view->GetCaretPosition();
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
		m_view->GetPosition(),
		m_view->GetScrollBarThumbSize(),
		m_view->GetScrollBarRange()
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
		m_view->SetYScrollPosition( m_scrollBar2->GetThumbPosition() );
		this->Freeze();
		m_view->UpdateDialsAndButtons();
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
	m_ruler->SetSnap( (long)( m_view->GetSnapValue() / m_view->GetRealZoom() ) );
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
	m_ruler->SetSnap( (long)( m_view->GetSnapValue() / m_view->GetRealZoom() ) );
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
	m_CaretPosition = m_view->GetCaretPosition();
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
		m_CaretPosition = m_view->GetCaretPosition();
		DrawCaret( dc );
		return false;
	}
	DrawCaret( dc );
	gg_tl_dat pos = g_ggseqProps.GetSoundManager()->GetPosition();
	gg_tl_dat pos2 = ( pos / 117600 ) * 31;
	if ( pos2 < m_scrollBar->GetBigThumbPosition() ) {
		m_scrollBar->SetBigThumbPosition( pos2 );
		m_view->SetPosition( m_scrollBar->GetBigThumbPosition() );
		UpdateRulerTicks();
		Refresh();
	} else if ( pos2 >= m_scrollBar->GetBigThumbPosition() + m_scrollBar->GetBigThumbSize() ) {
		//Hier wird gesprungen, sollte aber abhängig von der Fensterbreite passieren
		gg_tl_dat newPos = m_scrollBar->GetBigThumbPosition() + m_scrollBar->GetBigThumbSize();
		if ( newPos > m_scrollBar->GetBigRange() - m_scrollBar->GetBigThumbSize() )
			newPos = m_scrollBar->GetBigRange() - m_scrollBar->GetBigThumbSize();
		m_scrollBar->SetBigThumbPosition( newPos );
		m_view->SetPosition( newPos );
		UpdateRulerTicks();
		Refresh();
	}
	m_CaretPosition = m_view->GetCaretPosition( pos );
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
		-1, m_view->GetSnapValue() / 2,
		m_data->GetColourManager(), wxT("Preferences")
		);
	prefs.Centre();
	if ( prefs.ShowModal() == wxID_OK ) {
		m_view->SetSnapValue( prefs.GetFrameSnap() * 2 );
		prefs.SaveColours( m_data->GetColourManager() );
		wxConfigBase *conf = wxConfigBase::Get();
		conf->SetPath( wxT("/") );
		conf->Write( wxT("LoadLastProject"), prefs.GetLoadLastCheckbox()->GetValue() );
		Refresh();
	}
	m_ruler->SetSnap( (long)( m_view->GetSnapValue() / m_view->GetRealZoom() ) );
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
	int trackNr = m_view->GetTrackByY( y_ );
	if ( trackNr < 0 )
		return;
	m_view->AddItem( filename, x_, trackNr );
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
	m_ruler->SetPosition( m_view->GetPosition() );
}
void TLPanel::SetMasterVolume( float volume ) { m_data->SetMasterVolume( volume ); }

void TLPanel::Undo()
{
	m_view->Undo();
	m_view->UpdateDialsAndButtons();
	ResetScrollBar();
	Refresh();
}
void TLPanel::Redo()
{
	m_view->Redo();
	m_view->UpdateDialsAndButtons();
	ResetScrollBar();
	Refresh();
}
void TLPanel::AddTrack()
{
	m_view->m_docManager->SubmitCommand( new GgseqAddTrackCommand( m_data, -1, this ) );
	m_view->UpdateDialsAndButtons();
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
	m_view->m_docManager->SubmitCommand(
			new GgseqDeleteTrackCommand( m_data, track, this )
			);
	ResetScrollBar();
	Refresh();


}
void TLPanel::SetZoom( float zoom )
{
	m_view->SetZoom( zoom );
	ResetScrollBar();
	m_ruler->SetSnap( (long)( m_view->GetSnapValue() / m_view->GetRealZoom() ) );
	m_view->SetPosition( m_scrollBar->GetBigThumbPosition() );
}
void TLPanel::OnToggleEnvelope(wxMenuEvent& event)
{
	m_EditItem->m_toggleEnvelope = !m_EditItem->m_toggleEnvelope;
	m_EditItem = 0;
	Refresh();
}
void TLPanel::OnEdit(wxMenuEvent& event)
{
	//Open super funky Sample-Editor Window
	//m_EditItem;
	TrimmerDialog dlg( this->GetParent()->GetParent()->GetParent()->GetParent()->GetParent(), m_EditItem );
	dlg.Centre();
	if (dlg.ShowModal()==wxID_OK) {
		
	}

	m_EditItem = 0;
}
