/* TLTrack.h
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

#ifndef _TLTRACK_H_
#define _TLTRACK_H_

class TLItem;
class TLSample;
class TiXmlElement;

WX_DECLARE_LIST(TLItem, TLItemList);

class TLTrack
{
	public:
		TLTrack(int trackNr);
		~TLTrack();

		TLItemList::Node *GetFirst();
		TLItem *ItemAtPos(gg_tl_dat Position);
		TLItem *AddItem(TLSample *sample, gg_tl_dat position);
		void DeleteItem(TLItem *item);

		int GetHeight();
		gg_tl_dat GetLength();

		unsigned int FillBuffer(float* outBuffer, unsigned int count, gg_tl_dat position);
		void ResetOffsets();
		void Clear();
		void SortItems();
		void SetMute(bool mute=TRUE);
		void SetVolume(double vol);
		double GetVolume();
		bool IsMuted();
		void addXmlData(TiXmlElement *tracks);
	private:
		TLItemList m_itemList;
		int m_height;
		int m_trackNr;
		bool m_mute;
		double m_volume;
		gg_tl_dat m_length;
		TLItemList::Node *m_currentNode;
};

#endif /*_TLTRACK_H_*/
