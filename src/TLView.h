/* TLView.h
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

#ifndef _TLVIEW_H_
#define _TLVIEW_H_

class TLData;
class TLItem;
class TLSample;
class TLTrack;
class TLSelectionSet;
class GgseqDocManager;

class TLView
{
	public:
		TLView(TLData *TlData);
		~TLView();
/* - Get Set - */
		void SetSize( long width, long height );
		void SetZoom( float zoom );
		void SetPosition(gg_tl_dat Position);
		gg_tl_dat GetPosition();
		void SetSnapValue(long snapValue);
		long GetSnapValue();
		void SuspendSnap();
		void ResumeSnap();
		void SetYScrollPosition( long position ){m_YscrollPosition=position;};
		long GetYScrollPosition() {return m_YscrollPosition;};
/* - Get Set End - */
		gg_tl_dat GetScrollBarRange();
		gg_tl_dat GetScrollBarThumbSize();
/* - Draw related Functions - */
		void Draw(wxDC& dc_screen);
		void DrawSelection(wxDC *dc);
		static void Draw3dRect(wxDC *dc, wxCoord x,
			wxCoord y, wxCoord width, wxCoord height, wxColour colour);

		void DeleteItem(TLItem *item/*, long trackNr*/);
		void AddItem(TLSample *sample, long position, long trackNr);
		void AddItem(wxString filename, long position, long trackNr);

		long GetTrackByY(long y);/*private*/
		void GetTracksSurroundedBy(int &trackNr1, int &trackNr2, long y1, long y2);
		
		void Select(long x,long y,long width,long height);
		void ClearSelection();

		TLItem *GetDragItem(long x, long y);
		void DoDrop( long x, long y, TLItem *item,
		long x_offset, bool copy=false);

		void SetPlaybackPosition(long Position);
		long GetCaretPosition();
		long GetCaretPosition(gg_tl_dat Position);

		gg_tl_dat FromScreenXtoTL(long x);
		long FromTLtoScreenX(gg_tl_dat x);
		TLSample *GetSample(long position, long trackNr);
		long GetScreenSnapPosition(long position);
		bool IsSelectionAt(int x, int y, int& x_offset,
			int& y_offset, int& width, int& height);
		void EndSelectionDrag(int x, int y, bool copy, long x_offset);
		void Undo();
		void Redo();
		void SelectTrack(long y);
		void UpdateDialsAndButtons();
		GgseqDocManager *m_docManager;
		float GetRealZoom();
		wxRect GetItemBoundaries(TLItem *item);
		//Redraw Item => Zeichnet nur das Item.
	private:
		gg_tl_dat VisibleLength();
		gg_tl_dat GetSnap(gg_tl_dat x);
		long DrawTrack(wxDC& dc_screen, long yoffset, TLTrack* track);
		void DrawItem(wxDC& dc, TLItem* item, long left, long delta_left, long top, long width, long height);
		TLData *m_TlData;
		TLSelectionSet *m_selectionSet;
		gg_tl_dat m_PositionVisible; // TODO: muss weg
		long m_YscrollPosition;
		bool m_SnapSuspended;
		long m_TrackDrawDist;

		long m_width;
		long m_height;
		float m_zoom;
		wxIcon *m_drop_down_mini;
		wxIcon *m_gungirl;
		/* - Helper Functions - */
		void SetBrushColour( wxDC& dc, wxColour colour );
		bool ItemVisible( TLItem* item );
};

#endif /*_TLVIEW_H_*/
