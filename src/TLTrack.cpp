/* TLTrack.cpp
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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "TLTrack.h"
#include "TLItem.h"
#include "TLSample.h"

/*XML*/
#include "tinyxml.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TLItemList);

TLTrack::TLTrack(int trackNr)
{
	m_height=25;
	m_pos=0;
	m_trackNr=trackNr;
	m_itemList.DeleteContents(true);
	m_volume=1.0;
	m_mute=false;
}
TLTrack::~TLTrack()
{
	m_itemList.Clear();
}

TLItemList::Node *TLTrack::GetFirst()
{
	return m_itemList.GetFirst();
}
TLItem *TLTrack::ItemAtPos(int Position)
{
	for ( TLItemList::Node *node = m_itemList.GetLast(); node; node = node->GetPrevious() ) {
		TLItem *current = node->GetData();
		if ((current->GetPosition()<Position)&&(current->GetPosition()+current->GetLength()>Position))
			return current;
	}
	return (TLItem*)NULL;
}
void TLTrack::DeleteItem(TLItem *item)
{
	/*if (*/m_itemList.DeleteObject(item);/*)
		delete item;*/
}
TLItem *TLTrack::AddItem(TLSample *sample, int position)
{
	TLItem *tmp = new TLItem(sample,m_trackNr,position);
	m_itemList.Append(tmp);
	return tmp;
}

static int TlListCompare(const TLItem **arg1, const TLItem **arg2);

void TLTrack::SortItems()
{
	m_itemList.Sort(TlListCompare);
}

static int TlListCompare(const TLItem **arg1, const TLItem **arg2)
{
//	return ( (*arg1)->m_position > (*arg2)->m_position );
	return ((*arg1)->m_position > (*arg2)->m_position) ? 1 :
	 	(((*arg1)->m_position == (*arg2)->m_position) ? 0 : -1);
}
int TLTrack::GetHeight()
{
	return m_height;
}
void TLTrack::ResetOffsets(int Position)
{
	for ( TLItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData();
		current->ResetOffset();
	}
	m_currentNode = m_itemList.GetFirst();
	m_pos=Position;
//	m_done=false;
}
unsigned int TLTrack::FillBuffer(float* outBuffer, unsigned int count)
{
	unsigned int inc;
	unsigned int written=0;
	unsigned int emptyItems=0;
	float* incBuffer=outBuffer;
	while(written<count && m_currentNode) {
		inc=m_currentNode->GetData()->FillBuffer(incBuffer,m_pos+written,count-written, m_mute,m_volume);
		written+=inc;
		incBuffer+=inc;
		if (written<count) {
			m_currentNode=m_currentNode->GetNext();
		}
	}
	if (!m_currentNode){
		while(written<count) {
			*incBuffer=0;
			written++;
			incBuffer++;
			emptyItems++;
		}
	}
	m_pos+=written;
	return written-emptyItems;
}
void TLTrack::addXmlData(TiXmlElement *tracks)
{
	TiXmlElement *track = new TiXmlElement("track");
	tracks->LinkEndChild(track);
	track->SetAttribute("mute",m_mute);
	track->SetAttribute("volume",(int)(m_volume*100));
	for ( TLItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData();
		wxString tmp;
		tmp << current->GetPosition();
		wxString tmp2;
		tmp2 << current->GetSample()->GetId();
		TiXmlElement *item = new TiXmlElement("item");
		track->LinkEndChild(item);
		item->SetAttribute("sample",tmp2.mb_str());
		item->SetAttribute("pos",tmp.mb_str());
	}

}
void TLTrack::Clear()
{
//	m_itemList.DeleteContents(true);
	m_itemList.Clear();
}

void TLTrack::SetPlaybackPosition(int Position)
{
	m_pos=Position;
}

void TLTrack::SetMute(bool mute)
{
	m_mute=mute;
//	puts("TLTrack::SetMute");
}
bool TLTrack::IsMuted()
{
	return m_mute;
}
void TLTrack::SetVolume(double vol)
{
	m_volume=vol;
}
double TLTrack::GetVolume()
{
	return m_volume;
}

