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
		void SetPrefs();
		void SetZoom( float zoom );
		void SetUpdateListener(UpdateListener *updateListener);
		wxString GetFilename();
		void SetMasterVolume(float volume);
		void Undo();
		void Redo();
		void AddTrack();
		void DeleteTrack();

		void AddControls(TLTrack *track);
		void DeleteControls(TLTrack *track);
	private:
		void DrawCaret(wxDC& dc);
		void SetRubberframePen(wxDC* dc);
		void StartRubberFrame(int x, int y);
		void UpdateRubberFrame(int x, int y);
		void EndRubberFrame(int x, int y);
		void StartSelectionDrag(int x, int y, int width, int height);
		void UpdateSelectionDrag(int x, int y);
		void UpdateRulerTicks();
		void EndSelectionDrag(int x, int y, bool copyOnDrag);
		TLView *m_TlView;
		BigScrollBar *m_scrollBar;
		wxScrollBar *m_scrollBar2;
		TLData *m_data;
		TLLoadSaveManager *m_loadSaveManager;
		TLItem *m_DragItem;
#ifdef __WXMSW__ 
		wxGenericDragImage *m_dragImage;
#else
		wxDragImage *m_dragImage;
#endif
		bool m_sampleDrag;
		bool m_rubberDrag;
		bool m_selectionDrag;

		int m_SampleDragItemWidth;

		int m_DragX;
		int m_DragY;
		int x_offset;
		int y_offset;
		wxRect m_oldImgPos;

		wxRect m_selectionFrame;
		wxRect m_sampleFrame;
		wxRect m_rubberFrame;
		bool m_frameVisible;
		void ShowFrame(wxRect& rect, wxDC* dc);
		void HideFrame(wxRect& rect, wxDC* dc);
		void UpdateButtons();
		Ruler *m_ruler;
		int m_CaretPosition;
		bool m_CaretVisible;
    MouseDragHandler *m_dragHandler;
		DECLARE_EVENT_TABLE()
};

#endif /*_TLPANEL_H_*/
