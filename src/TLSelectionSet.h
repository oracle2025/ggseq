/* TLSelectionSet.h
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

#ifndef _TLSELECTIONSET_H_
#define _TLSELECTIONSET_H_

class TLSelectionItem;
class TLData;
WX_DECLARE_LIST(TLSelectionItem, TLSelItemList);

class TLSelectionItem
{
	public:
		TLSelectionItem(TLSample *sample, TLItem *item);
		~TLSelectionItem();
		TLSample *GetSample();
		TLItem *GetItem();
		long GetPosition();
		int GetTrack();
	private:
		TLSample *m_sample;
		TLItem *m_item;
};


class TLSelectionSet
{
	public:
		TLSelectionSet();
		~TLSelectionSet();
		void AddSample(TLSample *sample, TLItem *item/*, int position, int trackNr*/);
		void Clear(TLData* data);
		void DeleteFrom(TLData* data);
		void AddTo(TLData* data, int offsetPosition, int track); /*Für TLView*/
		TLSelItemList::Node *GetFirst();
		bool IsActive();
		void CalcBounds();
		int GetTrack1();
		int GetTrack2();
		long GetX1();
		long GetX2();
	private:
		TLSelItemList m_itemList;
		int m_trackNr1;
		int m_trackNr2;
		long m_x1;
		long m_x2;
/*		int m_trackOffset;
		long m_offset*/
};


#endif /* _TLSELECTIONSET_H_ */
