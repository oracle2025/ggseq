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
//class DnDData;

class TLView
{
	public:
		TLView(TLData *TlData);
		~TLView();

		void SetVisibleFrame(long width, long height, long x=0, long y=0);
		void SetVisibleLength(gg_tl_dat Length);
		void SetPosition(gg_tl_dat Position);
		gg_tl_dat GetPosition();
		gg_tl_dat GetScrollBarRange();
		gg_tl_dat GetScrollBarThumbSize();

		void Draw(wxDC& dc_screen);

		void DeleteItem(TLItem *item, long trackNr);
		void AddItem(TLSample *sample, long position, long trackNr);
		void AddItem(wxString filename, long position, long trackNr);

		long GetTrackByY(long y);/*private*/
		void GetTracksSurroundedBy(int &trackNr1, int &trackNr2, long y1, long y2);
		
		void Select(long x,long y,long width,long height);
		void ClearSelection();

		TLItem *GetDragItem(long x, long y);
		void DoDrop(long x, long y, TLItem *item, long srcTrack, long x_offset, bool copy=false);

		void SetPlaybackPosition(long Position);
		long GetCaretPosition();
		long GetCaretPosition(gg_tl_dat Position);

		gg_tl_dat FromScreenXtoTL(long x);
		long FromTLtoScreenX(gg_tl_dat x);
		TLSample *GetSample(long position, long trackNr);
		long GetScreenSnapPosition(long position);
		
		bool IsSelectionAt(int x, int y, int& x_offset, int& y_offset, int& width, int& height);
		void DrawSelection(wxDC *dc);
		static void Draw3dRect(wxDC *dc, wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxColour colour);
		void EndSelectionDrag(int x, int y, bool copy, long x_offset);
		void SetSnapValue(long snapValue);
		long GetSnapValue();

		static wxColour GetDarkColour(wxColour colour);
		static wxColour GetLightColour(wxColour colour);
		
	private:
		void SnapItem(TLItem *item);/*kommt evtl. nach TLData*/
		long DrawTrack(wxDC& dc_screen, long yoffset, TLTrack* track);
		TLData *m_TlData;
		TLSelectionSet *m_selectionSet;
		gg_tl_dat m_LengthVisible;
		gg_tl_dat m_PositionVisible;
		float m_Faktor;
//		gg_tl_dat m_Length;
		long m_FrameX;
		long m_FrameY;
		long m_FrameWidth;
		long m_FrameHeight;
		long m_TrackDrawDist;
		wxIcon *m_gungirl;

//		long m_CaretPosition;
};

#endif /*_TLVIEW_H_*/
