/* TLSelectionSet.cpp
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "TLSample.h"
#include "TLItem.h"
#include "TLData.h"
#include "TLSelectionSet.h"
#include <iostream>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TLSelItemList);

TLSelectionItem::TLSelectionItem(TLSample *sample, TLItem *item)
{
	m_sample=sample;
	m_item=item;
	m_sample->Ref();
	m_item->Select();
}
TLSelectionItem::~TLSelectionItem()
{
	m_sample->UnRef();
	m_item->UnSelect();/*TODO: müßte eigentlich segfaulten*/
	std::cout << "TLSelItem gelöscht" << std::endl;
}
TLSample *TLSelectionItem::GetSample()
{
	return m_sample;
}
TLItem *TLSelectionItem::GetItem()
{
	return m_item;
}
long TLSelectionItem::GetPosition()
{
	return m_item->GetPosition();
}
int TLSelectionItem::GetTrack()
{
	return m_item->GetTrack();
}

TLSelectionSet::TLSelectionSet()
{
	m_itemList.DeleteContents(true);
}
TLSelectionSet::~TLSelectionSet()
{
	m_itemList.Clear();
}
void TLSelectionSet::AddSample(TLSample *sample, TLItem *item)
{
	TLSelectionItem *tmp = new TLSelectionItem(sample, item);
	m_itemList.Append(tmp);
}
void TLSelectionSet::Clear(TLData* data)
{
	m_itemList.DeleteContents(false);
	for ( TLSelItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		TLSample* sample = current->GetSample();
		delete current;
		data->ClearSample(sample);
	}
	m_itemList.Clear();
	m_itemList.DeleteContents(true);

}
void TLSelectionSet::DeleteFrom(TLData* data)
{
	for ( TLSelItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		TLItem* item = current->GetItem();
		int track = current->GetTrack();
		data->DeleteItem(item,track);
	}
	Clear(data);
}
void TLSelectionSet::AddTo(TLData* data, int offsetPosition, int track)
{
//	CalcBounds();
	for ( TLSelItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		data->AddItem(current->GetSample(),offsetPosition+current->GetPosition()-m_x1,track+current->GetTrack()-m_trackNr1);
	}
}
/*void TLSelectionSet::Move(TLData* data, int offsetPosition, int track)*/
bool TLSelectionSet::IsActive()
{
	return m_itemList.GetCount()>0;
}
/*void TLSelectionSet::NormalizeOffset()
{
	m_offset=m_itemList.GetFirst()->GetData()->GetPosition();
	m_trackOffset=m_itemList.GetFirst()->GetData()->GetTrack();
	for ( TLSelItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		if (current->GetPosition()<m_offset)
			m_offset=current->GetPosition();
		if (current->GetTrack()<m_trackOffset)
			m_trackOffset=current->GetTrack();
	}
}*/
void TLSelectionSet::CalcBounds()
{
	if (!IsActive())
		return;
	m_trackNr1=m_itemList.GetFirst()->GetData()->GetTrack();
	m_trackNr2=m_trackNr1;
	m_x1=m_itemList.GetFirst()->GetData()->GetPosition();
	m_x2=m_x1;
	for ( TLSelItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		if (current->GetPosition()<m_x1)
			m_x1=current->GetPosition();
		if (current->GetTrack()<m_trackNr1)
			m_trackNr1=current->GetTrack();

		if (current->GetItem()->GetEndPosition()>m_x2)  /*TODO GetSample GetLength verwenden*/
			m_x2=current->GetItem()->GetEndPosition();
		if (current->GetTrack()>m_trackNr2)
			m_trackNr2=current->GetTrack();
	}
//	std::cout << "ff: " << m_x1 << " " << m_x2 << " " << m_trackNr1 << " " << m_trackNr2 << std::endl;
}
/*void TLSelectionSet::SetBounds(int trackNr1, int trackNr2, long x1, long x2)
{
	m_trackNr1=trackNr1;
	m_trackNr2=trackNr2;
	m_x1=x1;
	m_x2=x2;
}*/
int TLSelectionSet::GetTrack1()
{
	return m_trackNr1;
}
int TLSelectionSet::GetTrack2()
{
	return m_trackNr2;
}
long TLSelectionSet::GetX1()
{
	return m_x1;
}
long TLSelectionSet::GetX2()
{
	return m_x2;
}
TLSelItemList::Node * TLSelectionSet::GetFirst()
{
	return m_itemList.GetFirst();
}
