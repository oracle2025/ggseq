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
typedef struct {
	float leftFadeLevel;  // = 1.0;
	float leftFadePos;    // = 0.5;
	float middleLevel;    // = 1.0;
	float rightFadeLevel; // = 1.0;
	float rightFadePos;   // = 0.5;
} NativeEnvData;
#define RECT_LENGTH 7
#define TRACK_HEIGHT 25
void GetRectsFromEnv( const NativeEnvData& e, gg_tl_dat SampleLength, wxRect *result );
void GetEnvFromRects( NativeEnvData& e, const wxRect* rect, gg_tl_dat SampleLength );
void GetSampleEnvelope( EnvelopePoint *result, const NativeEnvData& e, gg_tl_dat SampleLength );

#include <soundtouch/SoundTouch.h>

class TLSample;
class GetItemTrackListener;

typedef struct { // Store everything important for UndoRedo of an Item.
	gg_tl_dat position; // empty in TLItem
	bool toggleEnvelope;
	long referenceId;
	NativeEnvData nativeEnvData;
	float timestretch;
	gg_tl_dat leftTrim;
	gg_tl_dat rightTrim;
	wxString filename; // empty in TLItem
	int trackId;       // empty in TLItem
} ItemEssentials;


class TLItem // Should not contain UI-relevant Data. ( Envelope Rects? )
{
	public:
	// constructors and destructors
		TLItem( TLSample *sample,
			gg_tl_dat position,
			long referenceId,
			GetItemTrackListener* trackListener
		);
		TLItem( TLSample *sample,
			const ItemEssentials& e,
			GetItemTrackListener* trackListener );
		~TLItem();
	// method declarations
		float    *GetBuffer();
		gg_tl_dat GetBufferLen();
		float    *GetStretchedBuffer();
		void      Draw( wxDC& dc, float zoom );
		void      DrawEnvelope( wxDC &dc,
				int xOffset, int yOffset,
				bool updateEnv = true,
				wxRect *customFades = 0,
				float zoom = HARD_ZOOM );
		wxRect   *GetEnvelopeHandle( int x, int y, float zoom = HARD_ZOOM );
		void      DragEnvelopeHandle( wxRect *handle );
		void      GetEnvelopeData( NativeEnvData &env );
		void      GetNewEnvelopeData( NativeEnvData &env, 
				wxRect *handle );
		void      SetEnvelopeData( const NativeEnvData &env );

		int       GetTrack();
		long      GetReference();
		void      SetReference( long referenceId ) { m_referenceId = referenceId; }
		gg_tl_dat GetLen();
		gg_tl_dat GetPosition();
		gg_tl_dat GetEndPosition();
		void      SetPosition(gg_tl_dat position);
		void      Select();
		void      UnSelect();
		bool      IsSelected();
		float    *GetTrimNStretchBuffer();
		gg_tl_dat GetLeftTrim();
		gg_tl_dat GetRightTrim();
		float     GetTimestretch();
		void      SetTrimNStretch( gg_tl_dat leftTrim,
				gg_tl_dat rightTrim, float timestretch );
	

		unsigned int FillBuffer(float* outBuffer,
				gg_tl_dat pos, unsigned int count,
				bool mute, double volume);

		TLSample *GetSample();
		void GetEssentials( ItemEssentials &e );
		
		bool m_toggleEnvelope;
		gg_tl_dat m_position;
	private:
	// method declarations
		float GetEnvelopValue( int position );
		void GuiEnvToDataEnv();
	// member variable declarations
		ItemEssentials m_essentials;
		long m_referenceId;
		TLSample *m_sample;
		bool m_selected;
		GetItemTrackListener* m_trackListener;
		wxRect m_guiEnvData[4];
		EnvelopePoint m_sampleEnvData[4];
		NativeEnvData m_nativeEnvData;
		float m_timestretch;
		gg_tl_dat m_leftTrim;
		gg_tl_dat m_rightTrim;
		float *m_stretchedBuffer;
		gg_tl_dat m_stretchedLen;
};
#endif /*_TLITEM_H_*/
