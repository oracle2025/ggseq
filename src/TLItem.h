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

class TLSample;
class GetItemTrackListener;
class TLItem
{
	public:
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
		gg_tl_dat m_position; /*TODO: über friend Klassen*/
		TLSample *GetSample();
		void Select();
		void UnSelect();
		bool IsSelected();
		long GetReference();
	private:
		long m_referenceId;
		TLSample *m_sample;
		bool m_selected;
		//int m_trackNr;
		GetItemTrackListener* m_trackListener;
};

#endif /*_TLITEM_H_*/
