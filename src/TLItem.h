/* TLItem.h
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

#ifndef _TLITEM_H_
#define _TLITEM_H_

typedef struct {
	int x;    // Position in Samples
	float y;  // 0.0 bis 1.0
} Env;
#include <soundtouch/SoundTouch.h>
class TLSample;
class GetItemTrackListener;
class TLItem
{
	public:
/*		TLItem( TLItem *item, gg_tl_dat position,
			long referenceId,
			GetItemTrackListener* trackListener
		);*/
		TLItem( TLSample *sample,
		//	int trackNr,
			gg_tl_dat position,
			long referenceId,
			GetItemTrackListener* trackListener
		);
		~TLItem();
		unsigned int FillBuffer(float* outBuffer, gg_tl_dat pos, unsigned int count, bool mute, double volume);
		gg_tl_dat GetLength();
		gg_tl_dat GetPosition();
		gg_tl_dat GetEndPosition();
		int GetTrack();
		void SetPosition(gg_tl_dat position);
		gg_tl_dat m_position; /*TODO: �ber friend Klassen*/
		TLSample *GetSample();
		void Select();
		void UnSelect();
		bool IsSelected();
		long GetReference();
//    bool HandleInternal(gg_tl_dat x, float y);
//		long m_x_test;
//		long m_y_test;
		wxRect m_leftFadeIn;
		wxRect m_rightFadeIn;
		wxRect m_leftFadeOut;
		wxRect m_rightFadeOut;
		void DrawEnvelope( wxDC &dc, int xOffset, int yOffset );
		wxRect *TouchingEnvelopeCtrl( int x, int y );
		void GuiEnvToDataEnv();
		void Stretch( float amount );
//		void Trim( gg_tl_dat start, gg_tl_dat end );
		bool m_toggleEnvelope;
		float *m_stretchedBuffer;
		gg_tl_dat m_stretchedLen;
	private:
		float GetEnvelopValue( int position );
		Env m_realEnvelope[4];
		long m_referenceId;
		TLSample *m_sample;
		bool m_selected;
		//int m_trackNr;
		GetItemTrackListener* m_trackListener;
		SoundTouch m_SoundTouch;
};
/*
class TLStoreItem : public TLItem
{
	public:
		TLSample *GetSample();
	private:
		TLSample *m_sample;
};
*/
#endif /*_TLITEM_H_*/
