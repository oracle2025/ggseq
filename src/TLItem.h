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
} EnvelopePoint;
/*typedef struct {
	wxRect m_leftFadeIn;
	wxRect m_rightFadeIn;
	wxRect m_leftFadeOut;
	wxRect m_rightFadeOut;
} itemUiStuff;*/
#include <soundtouch/SoundTouch.h>
class TLSample;
class GetItemTrackListener;
class TLItem // Should not contain UI-relevant Data. ( Envelope Rects? )
{
	public:
	// constructors and destructors
		TLItem( TLSample *sample,
			gg_tl_dat position,
			long referenceId,
			GetItemTrackListener* trackListener
		);
		~TLItem();
	// method declarations
		unsigned int FillBuffer(float* outBuffer, gg_tl_dat pos, unsigned int count, bool mute, double volume);
		int GetTrack();
		TLSample *GetSample();
		long GetReference();
		void SetReference( long referenceId ) { m_referenceId = referenceId; }
		gg_tl_dat GetLength();
		gg_tl_dat GetPosition();
		gg_tl_dat GetEndPosition();
		void SetPosition(gg_tl_dat position);
		void Select();
		void UnSelect();
		bool IsSelected();
		void DataEnvToGuiEnv();
		void Stretch( float amount, gg_tl_dat trimStart = -1, gg_tl_dat trimEnd = -1 );
		//void SetEnvelope( EnvelopePoint *envelope );
		void DrawEnvelope( wxDC &dc, int xOffset, int yOffset, wxRect *customFades = 0 );
		wxRect *TouchingEnvelopeCtrl( int x, int y );
		void GuiEnvToDataEnv();
	// member variable declarations
		gg_tl_dat m_position; /*TODO: über friend Klassen*/
		bool m_toggleEnvelope;
		float *m_stretchedBuffer;
		gg_tl_dat m_stretchedLen;
		wxRect m_fades[4];
		EnvelopePoint m_realEnvelope[4];
	private:
	// method declarations
		float GetEnvelopValue( int position );
	// member variable declarations
		long m_referenceId;
		TLSample *m_sample;
		bool m_selected;
		GetItemTrackListener* m_trackListener;
/*		float m_timestretch;
		gg_tl_dat m_trimStart;
		gg_tl_dat m_trimEnd;*/
};
#endif /*_TLITEM_H_*/
