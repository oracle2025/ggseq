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

#include "stuff.h"
#include "TLItem.h"
#include "TLTrack.h"
#include "TLSample.h"
#include "TLData.h"
#include "TLPanel.h"

/*XML*/
#include "tinyxml.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TLItemList);

TLTrack::TLTrack(/*int trackNr,*/ GetTrackNrListener *trackListener, TLPanel* panel)
{
	m_itemList = new TLItemList(wxKEY_INTEGER);
	m_height=25;
//	m_trackNr=trackNr;
	m_itemList->DeleteContents(true);
	m_volume=1.0;
	m_mute=false;
	m_length=0;
	m_selected = 0;
	m_trackListener = trackListener;
	m_panel = panel;
	m_panel->AddControls(this);
}
int TLTrack::GetTrackNr()
{
	return m_trackListener->GetTrackNr(this);
}
TLTrack::~TLTrack()
{
	m_panel->DeleteControls(this);
	m_itemList->Clear();
	delete m_itemList;
	m_itemList = (TLItemList*) 0;
}

TLItemList::Node *TLTrack::GetFirst()
{
	return m_itemList->GetFirst();
}
TLItem *TLTrack::ItemAtPos(gg_tl_dat Position)
{
	for ( TLItemList::Node *node = m_itemList->GetLast(); node; node = node->GetPrevious() ) {
		TLItem *current = node->GetData();
		if ( ( current->GetPosition() < Position ) && ( current->GetExtEndPosition() > Position ) ) {
			return current;
		}
	}
	return (TLItem*)NULL;
}
void TLTrack::DeleteItem(TLItem *item /*, long referenceId*/)
{
//	m_itemList->DeleteNode(m_itemList->Find(referenceId));
	m_itemList->DeleteObject(item);
}
TLItem *TLTrack::AddItem(TLSample *sample, gg_tl_dat position, long referenceId )
{
	TLItem *tmp = new TLItem(sample,/*m_trackNr,*/position, referenceId, this);
	m_itemList->Append(referenceId, tmp);
	return tmp;
}
TLItem *TLTrack::AddItem( TLSample *sample, const ItemEssentials &e )
{
	TLItem *tmp = new TLItem( sample, e, this );
	m_itemList->Append( e.referenceId, tmp );
	return tmp;
}
static int TlListCompare(const TLItem **arg1, const TLItem **arg2);

void TLTrack::SortItems()
{
	m_itemList->Sort(TlListCompare);
	TLItemList::Node *node= m_itemList->GetLast();
	if (node) {
		TLItem *item=node->GetData();
		m_length=item->GetLen()+item->GetPosition();
	}
}
gg_tl_dat TLTrack::GetLength()
{
	return m_length;
}
static int TlListCompare(const TLItem **arg1, const TLItem **arg2)
{
	return ((*arg1)->m_position > (*arg2)->m_position) ? 1 :
	 	(((*arg1)->m_position == (*arg2)->m_position) ? 0 : -1);
}
int TLTrack::GetHeight()
{
	return m_height;
}
void TLTrack::ResetOffsets()
{
	m_currentNode = m_itemList->GetFirst();
}
unsigned int TLTrack::FillBuffer(float* outBuffer, unsigned int count, gg_tl_dat position)
{
	unsigned int inc;
	unsigned int written=0;
	unsigned int emptyItems=0;
	float* incBuffer=outBuffer;
	while(written<count && m_currentNode) {
		inc=m_currentNode->GetData()->FillBuffer(incBuffer,position+written,count-written, m_mute,m_volume);
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
	return written-emptyItems;
}
void TLTrack::addXmlData(TiXmlElement *tracks)
{
	TiXmlElement *track = new TiXmlElement("track");
	tracks->LinkEndChild(track);
	track->SetAttribute("mute",m_mute);
	track->SetAttribute("volume",(int)(m_volume*100));
	ItemEssentials e;
	for ( TLItemList::Node *node = m_itemList->GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData();
		wxString tmp;
//		char buffer[100];
		tmp.Printf(wxT("%lld"),current->GetPosition());
//		tmp << (unsigned long)current->GetPosition();/*TODO richtig umwandeln*/
		
		wxString tmp2;
		tmp2 << current->GetSample()->GetId();
		TiXmlElement *item = new TiXmlElement("item");
		track->LinkEndChild(item);
		item->SetAttribute("sample",tmp2.mb_str());
		item->SetAttribute("pos",tmp.mb_str());
		current->GetEssentials(e);
		item->SetAttribute("envelope", wxString::Format( wxT("%d"), e.toggleEnvelope ).mb_str() );
		item->SetDoubleAttribute( "leftFadeLevel", e.nativeEnvData.leftFadeLevel );
		item->SetDoubleAttribute( "leftFadePos", e.nativeEnvData.leftFadePos );
		item->SetDoubleAttribute( "middleLevel", e.nativeEnvData.middleLevel );
		item->SetDoubleAttribute( "rightFadeLevel", e.nativeEnvData.rightFadeLevel );
		item->SetDoubleAttribute( "rightFadePos", e.nativeEnvData.rightFadePos );
		item->SetDoubleAttribute( "timestretch", e.timestretch );
/*		item->SetAttribute("leftFadeLevel", wxString::Format( wxT("%.30e"), e.nativeEnvData.leftFadeLevel ).mb_str() );
		item->SetAttribute("leftFadePos", wxString::Format( wxT("%.30e"), e.nativeEnvData.leftFadePos ).mb_str() );
		item->SetAttribute("middleLevel", wxString::Format( wxT("%.30e"), e.nativeEnvData.middleLevel ).mb_str() );
		item->SetAttribute("rightFadeLevel", wxString::Format( wxT("%.30e"), e.nativeEnvData.rightFadeLevel ).mb_str() );
		item->SetAttribute("rightFadePos", wxString::Format( wxT("%.30e"), e.nativeEnvData.rightFadePos ).mb_str() );
		item->SetAttribute("timestretch", wxString::Format( wxT("%.30e"), e.timestretch ).mb_str() );*/
		item->SetAttribute("leftTrim", wxString::Format( wxT("%lld"), e.leftTrim ).mb_str() );
		item->SetAttribute("rightTrim", wxString::Format( wxT("%lld"), e.rightTrim ).mb_str() );
		item->SetAttribute("extended", e.extended );
	}

}
void TLTrack::Clear()
{
	m_itemList->Clear();
	m_mute = false;
	m_volume = 1.0;
}

void TLTrack::SetMute(bool mute)
{
	m_mute=mute;
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
//long TLTrack::GetReference() { return m_referenceId; }

