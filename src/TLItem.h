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
class TLItem
{
	public:
		TLItem(TLSample *sample, int trackNr,int position/* = 0*/);
		~TLItem();
		int FillBuffer(float* outBuffer, int pos, int count, bool mute, double volume);
		int GetLength();
		const int GetPosition();
		int GetTrack();
		int GetEndPosition();
		void SetPosition(int position);
		void ResetOffset();
		int m_position;
		TLSample *GetSample();
		void Select();
		void UnSelect();
		bool IsSelected();
	private:
		int m_playingOffset;
		TLSample *m_sample;
		bool m_selected;
		int m_trackNr;
};

#endif /*_TLITEM_H_*/
