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

class TLMuteButton;
class TLTrackVolumeDial;
class GetTrackNrListener;
class GetItemTrackListener
{
	public:
		virtual ~GetItemTrackListener(){};
		virtual int GetTrackNr()=0;
};

class TLPanel;

class TLTrack : public GetItemTrackListener
{
	public:
		TLTrack( GetTrackNrListener *trackListener, TLPanel* panel );
		~TLTrack();

		TLItemList::Node *GetFirst();
		TLItem *ItemAtPos(gg_tl_dat Position);
		TLItem *AddItem(TLSample *sample, gg_tl_dat position, long referenceId );
		int GetTrackNr();
		void DeleteItem(TLItem *item/*, long referenceId*/);

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
		bool GetSelected() {return m_selected;};
		void SetSelected(bool selected) {m_selected = selected;};
		TLMuteButton *m_muteButton;
		TLTrackVolumeDial *m_volumeDial;
	private:
		TLPanel *m_panel;
		GetTrackNrListener *m_trackListener;
		TLItemList *m_itemList;// = TLItemList(wxKEY_INTEGER);
		int m_height;
		//int m_trackNr;
		bool m_mute;
		bool m_selected;
		double m_volume;
		gg_tl_dat m_length;
		TLItemList::Node *m_currentNode;
		//long m_referenceId;
};

#endif /*_TLTRACK_H_*/
