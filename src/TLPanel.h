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
class MiniPlayerInterface;
class Ruler; 
class UpdateListener;

class TLPanel : public wxPanel
{
	public:
		TLPanel(wxWindow* parent, wxScrollBar *scrollbar,
				wxWindowID id = -1,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL,
				const wxString& name = wxT("panel"));
		~TLPanel();

		void OnPaint(wxPaintEvent& event);
		void OnEraseBackground(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnMouseUp(wxMouseEvent& event);
		void OnMouseDown(wxMouseEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnDoubleClick(wxMouseEvent& event);
		void OnScroll(wxScrollEvent& event);

		void ResetScrollBar();
//		void EndPopupWindowDrag(int x, int y);
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
//		void PlaySample(wxString filename, long &length);
		void StopAll();
		void WavExport();
		bool UpdateCaret();
		void SetColours(wxString path);		
		void SetSnap();
		SoundManager *GetSoundManager();
		void SetMiniPlayer(MiniPlayerInterface *mp);
		void SetUpdateListener(UpdateListener *updateListener);
		wxString GetFilename();
	private:
		void DrawCaret(wxDC& dc);
		void SetRubberframePen(wxDC* dc);
		void StartRubberFrame(int x, int y);
		void UpdateRubberFrame(int x, int y);
		void EndRubberFrame(int x, int y);
		void StartSampleDrag(int x, int y, int srcTrackNr, TLItem* srcItem);
		void UpdateSampleDrag(int x, int y);
		void EndSampleDrag(int x, int y, bool copyOnDrag);
		void StartSelectionDrag(int x, int y, int width, int height);
		void UpdateSelectionDrag(int x, int y);
		void UpdateRulerTicks();
		void EndSelectionDrag(int x, int y, bool copyOnDrag);
		TLView *m_TlView;
		wxScrollBar *m_scrollBar;
		TLData *m_data;
		TLLoadSaveManager *m_loadSaveManager;
		SoundManager *m_soundManager;
		TLItem *m_DragItem;
		MiniPlayerInterface *m_miniPlayer;
#ifdef __WXMSW__ 
		wxGenericDragImage *m_dragImage;
#else
		wxDragImage *m_dragImage;
#endif
		TLMuteButton *m_buttons[8];
		TLTrackVolumeDial *m_dials[8];
		bool m_sampleDrag;
		bool m_rubberDrag;
		bool m_selectionDrag;

		int m_SampleDragSrcTrackNr;
		int m_SampleDragItemWidth;

		int m_DragX;
		int m_DragY;
		int x_offset;
		int y_offset;
		int m_oldXImgPos;
		int m_oldYImgPos;

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
		DECLARE_EVENT_TABLE()
};

#endif /*_TLPANEL_H_*/
