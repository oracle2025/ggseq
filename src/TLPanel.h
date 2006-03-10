/* TLPanel.h
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

#ifndef _TLPANEL_H_
#define _TLPANEL_H_

class DnDData;
class TLLoadSaveManager;
class SoundManager;
class TLData;
#ifdef __WXMSW__ 
class wxGenericDragImage;
#else
class wxDragImage;
#endif
class TLView;
class TLItem;
class TLMuteButton;
class TLTrackVolumeDial;
class Ruler; 
class UpdateListener;
class BigScrollBar;
class TLTrack;
class MouseDragHandler;
class wxScrollBar;
class UndoRedoChangeListener;

class TLPanel : public wxPanel
{
	public:
		TLPanel(wxWindow* parent, BigScrollBar *scrollbar,
			Ruler *ruler, wxScrollBar *scrollbar2, wxWindowID id = -1 );
		~TLPanel();
		void OnPaint(wxPaintEvent& event);
		void OnEraseBackground(wxEraseEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnMouseUp(wxMouseEvent& event);
		void OnMouseDown(wxMouseEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnDoubleClick(wxMouseEvent& event);
		void OnScroll(wxScrollEvent& event);
		void OnScroll2(wxScrollEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnToggleEnvelope(wxCommandEvent& event);
		void ResetScrollBar();
		void DropFileAt(int x, int y, wxString filename);
		bool New();
		bool Load();
		bool Load(wxString& filename);
		bool SaveAs();
		bool Save();
		bool SaveOnExit();
		void Rewind();
		void Play();
		void Stop();
		void StopAll();
		void ExportPackage();
		void ImportPackage( wxString package, wxString contents );
		void WavExport();
		bool UpdateCaret();
		void SetPrefs( const wxString &path );
		void SetZoom( float zoom );
		void SetUpdateListener(UpdateListener *updateListener);
		void SetUndoRedoChangeListener(UndoRedoChangeListener *urChangeListener);
		wxString GetFilename();
		void SetMasterVolume(float volume);
		void Undo();
		void Redo();
		bool CanUndo();
		bool CanRedo();
		void AddTrack();
		void DeleteTrack();

		void AddControls(TLTrack *track);
		void DeleteControls(TLTrack *track);
		TLData *m_data;
	private:
		void DrawCaret(wxDC& dc);
		void UpdateRulerTicks();
		BigScrollBar *m_scrollBar;
		wxScrollBar *m_scrollBar2;
		TLView *m_view;
		TLLoadSaveManager *m_loadSaveManager;
		TLItem *m_EditItem;
#ifdef __WXMSW__ 
		wxGenericDragImage *m_dragImage;
#else
		wxDragImage *m_dragImage;
#endif


		int m_DragX;
		int m_DragY;
		void UpdateButtons();
		Ruler *m_ruler;
		int m_CaretPosition;
		bool m_CaretVisible;
		MouseDragHandler *m_dragHandler;
		DECLARE_EVENT_TABLE()
};

#endif /*_TLPANEL_H_*/
