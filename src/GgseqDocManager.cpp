/* GgseqDocManager.cpp
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

#include <iostream>

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <wx/listimpl.cpp>
#include <wx/hashmap.h>

#include "stuff.h"
#include "TLData.h"
#include "TLTrack.h"
#include "TLItem.h"
#include "TLSample.h"
#include "TLSelectionSet.h"
#include "GgseqDocManager.h"
#include "Listeners.h"

#define MAX_UNDO 50

WX_DEFINE_LIST(GgseqCommandList);

class GgseqUndoItem
{
	public:
/*		GgseqUndoItem(
			wxString filename,
			int64_t position,
			unsigned int trackId,
			long referenceId = 0,
			TLItem *itemReference = (TLItem*)0
		)*/
		GgseqUndoItem(
			const ItemEssentials &e,
			TLItem *itemReference = (TLItem*)0
		)	{
			m_itemReference = itemReference;
			m_essentials = e;
/*			m_filename = filename;
			m_position = position;
			m_trackId = trackId;
			m_referenceId = referenceId;*/
		}
		TLItem *m_itemReference;/*ist nicht vermeidbar sein*/
		//long m_referenceId;
		//wxString m_filename;
		//int64_t m_position;
		//unsigned int m_trackId;
		ItemEssentials m_essentials;
};
WX_DEFINE_LIST(GgseqUndoItemList);

/* -- GgseqDocManager -- */
GgseqDocManager::GgseqDocManager( TLData *data )
{
	m_document = data;
	m_urChangeListener = 0;
	m_referenceCount = 1;
	g_ggseqProps.SetDocManager(this);
}
void GgseqDocManager::SetUndoRedoChangeListener(UndoRedoChangeListener *urChangeListener)
{
	m_urChangeListener = urChangeListener;
}
GgseqDocManager::~GgseqDocManager()
{
	m_commandList.DeleteContents(true);
	m_commandList.Clear();
	m_redoList.DeleteContents(true);
	m_redoList.Clear();
}
void GgseqDocManager::Reset()
{
	m_commandList.DeleteContents(true);
	m_commandList.Clear();
	m_commandList.DeleteContents(false);
	m_redoList.DeleteContents(true);
	m_redoList.Clear();
	m_redoList.DeleteContents(false);
}
void GgseqDocManager::SubmitCommand( GgseqCommand *command )
{
	command->SetDocManager(this);
	command->Do();
	if (command->GetError() ) {
		delete command;
	} else {
		if (m_commandList.GetCount()>MAX_UNDO) {
			m_commandList.DeleteContents(true);
			m_commandList.DeleteNode(m_commandList.GetFirst());
			m_commandList.DeleteContents(false);
		}
		m_commandList.Append(command);
	/*Wenn noch 'sachen' in der RedoListe sind, diese löschen*/
		m_redoList.DeleteContents(true);
		m_redoList.Clear();
		m_redoList.DeleteContents(false);
	}
	if (m_urChangeListener)
		m_urChangeListener->UndoRedoChanged();
}
void GgseqCommand::SetDocManager( GgseqDocManager *docManager )
{
	m_docManager = docManager;
}
bool GgseqCommand::GetError() { return m_error; }
TLItem *GgseqModifyItemCommand::GetItem()
{
	if (!m_referenceId) {
		m_referenceId = m_docManager->GetNewRefId();
		if (m_item)
			m_document->SetItemReferenceId( m_item, m_referenceId );
	}
	TLItem *item;
	if (m_item) {
		item = m_item;
		m_item = (TLItem*) 0;
	} else {
		item = m_document->GetItem( m_referenceId );
	}
	return item;
}
void GgseqDocManager::Undo()
{
	GgseqCommandList::Node *node = m_commandList.GetLast();
	GgseqCommand *command = node->GetData();
	command->Undo();
	m_commandList.DeleteNode(node);
	m_redoList.Append(command);
}
void GgseqDocManager::Redo()
{
	GgseqCommandList::Node *node = m_redoList.GetLast();
	GgseqCommand *command = node->GetData();
	command->Do();
	m_redoList.DeleteNode(node);
	m_commandList.Append(command);
}
bool GgseqDocManager::CanUndo()
{
	return m_commandList.GetCount() > 0;
}
bool GgseqDocManager::CanRedo()
{
	return m_redoList.GetCount() > 0;
}
long GgseqDocManager::GetNewRefId()
{
	return m_referenceCount++;
}
/* -- GgseqAddItemCommand -- */
GgseqAddItemCommand::GgseqAddItemCommand( TLData *doc, const wxString& filename,
                                          int64_t position, unsigned int trackId,
                                          TLSample *sample )
{
	m_sample = sample;
	m_document = doc;
	m_error = 0;
	
	m_essentials.position = position;
	m_essentials.filename = filename;
	m_essentials.trackId = trackId;
	m_essentials.referenceId = 0;
/*	m_filename = filename;
	m_position = position;
	m_trackId = trackId;
	m_referenceId = 0;*/
}
void GgseqAddItemCommand::Do()
{
	if (!m_essentials.referenceId)
		m_essentials.referenceId = m_docManager->GetNewRefId();
	if (m_sample) {
		m_document->AddItem( m_sample, m_essentials.position,
				m_essentials.trackId, m_essentials.referenceId );
	} else {
		if (m_document->AddItem( m_essentials.filename,
					m_essentials.position,
					m_essentials.trackId,
					m_essentials.referenceId ) == 0)
			m_error = 1;
	}
}
void GgseqAddItemCommand::Undo()
{
	TLItem *item = m_document->GetItem(m_essentials.referenceId);
	m_document->DeleteItem( item , m_essentials.trackId );
	m_sample = (TLSample*) 0;
}

/* -- GgseqDeleteItemCommand -- */
GgseqDeleteItemCommand::GgseqDeleteItemCommand( TLData *doc, TLItem *item )
{
	m_document = doc;
	m_item = item;
	m_error = 0;

	item->GetEssentials( m_essentials );
	
/*	m_referenceId = item->GetReference();
	m_filename = item->GetSample()->GetFilename();
	m_position = item->GetPosition();
	m_trackId = item->GetTrack();*/
}
void GgseqDeleteItemCommand::Do()
{
	if (!m_essentials.referenceId)
		m_essentials.referenceId = m_docManager->GetNewRefId();
	if (m_item) {
		m_document->DeleteItem( m_item, m_essentials.trackId );
		m_item = (TLItem*) 0;
	} else {
		TLItem *item = m_document->GetItem(m_essentials.referenceId); /* xxx (siehe oben)*/
		m_document->DeleteItem( item, m_essentials.trackId );
	}
}
void GgseqDeleteItemCommand::Undo()
{
//	m_document->AddItem( m_filename, m_position, m_trackId, m_referenceId );
	m_document->AddItem( m_essentials );
}

/* -- GgseqMoveItemCommand -- */
GgseqMoveItemCommand::GgseqMoveItemCommand( TLData *doc, TLItem *srcItem,
                                            int64_t destPosition, unsigned int destTrackId )
{
	m_document = doc;
	m_error = 0;
	
//	m_referenceId = srcItem->GetReference();
//	m_position = destPosition;
//	m_trackId = destTrackId;
	m_item = srcItem;
	m_item->GetEssentials( m_essentials );
	m_essentials.position = destPosition;
	m_essentials.trackId = destTrackId;
}
void GgseqMoveItemCommand::Do()
{
	if (!m_essentials.referenceId)
		m_essentials.referenceId = m_docManager->GetNewRefId();
	TLItem *item;
	if (m_item) {
		item = m_item;
		m_item = (TLItem*) 0;
	} else {
		item = m_document->GetItem(m_essentials.referenceId);
	}
	//m_filename = item->GetSample()->GetFilename();
	//item->GetEnvelopeData(m_envelopeData);
	//m_toggleEnvelope = item->m_toggleEnvelope;
	int64_t oldPositon = item->GetPosition();
	unsigned int oldTrackId = item->GetTrack();
	TLSample *sample = item->GetSample();
	sample->Ref();
	m_document->DeleteItem( item, oldTrackId );
	
	//m_document->AddItem( sample, m_position, m_trackId , m_referenceId, &m_envelopeData, m_toggleEnvelope );
	m_document->AddItem( m_essentials );
	
	m_essentials.position = oldPositon;
	m_essentials.trackId = oldTrackId;
	sample->UnRef();
	
}
void GgseqMoveItemCommand::Undo()
{
	TLItem *item = m_document->GetItem(m_essentials.referenceId);
	int64_t newPositon = item->GetPosition();
	unsigned int newTrackId = item->GetTrack();
	TLSample *sample = item->GetSample();
	sample->Ref();
	m_document->DeleteItem( item, newTrackId );
	//m_document->AddItem( sample, m_position, m_trackId, m_referenceId, &m_envelopeData );
	m_document->AddItem( m_essentials );
	m_essentials.position = newPositon;
	m_essentials.trackId = newTrackId;
	sample->UnRef();
}

/* -- GgseqBunchOfItemsCommand -- */
GgseqBunchOfItemsCommand::~GgseqBunchOfItemsCommand()
{
	m_itemList.DeleteContents(true);
	m_itemList.Clear();
}
/* -- GgseqAddItemsCommand -- */
GgseqAddItemsCommand::GgseqAddItemsCommand( TLData *doc, TLSelectionSet *selSet, TLSelectionSet **newSet, int64_t offsetPos, unsigned int trackId )
{
	m_document = doc;
	m_position = offsetPos;
	m_trackId = trackId;
	m_selSetPointer = newSet;
	ItemEssentials e;
	for ( TLSelItemList::Node *node = selSet->GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		current->GetItem()->GetEssentials(e);
		e.position = m_position + e.position - selSet->GetX1();
		e.trackId = m_trackId + e.trackId - selSet->GetTrack1();
		m_itemList.Append(
			new GgseqUndoItem( e )
/*			new GgseqUndoItem( current->GetSample()->GetFilename(),
				m_position + current->GetPosition() - selSet->GetX1(),
				m_trackId + current->GetTrack() - selSet->GetTrack1() )*/
		);
	}
	m_error = 0;
}

void GgseqAddItemsCommand::Do()
{
	if (m_selSetPointer != 0) {
		*m_selSetPointer = new TLSelectionSet();
	}
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		if (current->m_essentials.referenceId == 0)
			current->m_essentials.referenceId = m_docManager->GetNewRefId();
		TLItem *item = m_document->AddItem(
			current->m_essentials
//			current->m_filename,
//			current->m_position,
//			current->m_trackId,
//			current->m_referenceId
		);
		if ( item && m_selSetPointer ) {
			(*m_selSetPointer)->AddSample(item->GetSample(), item);
		}
	}
	if (m_selSetPointer != 0) {
		(*m_selSetPointer)->CalcBounds();
		m_selSetPointer = 0;
	}
}

void GgseqAddItemsCommand::Undo()
{
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		TLItem *item = m_document->GetItem(current->m_essentials.referenceId);
		m_document->DeleteItem( item, current->m_essentials.trackId );
	}
}
/* -- GgseqMoveItemsCommand -- */
GgseqMoveItemsCommand::GgseqMoveItemsCommand( TLData *doc,
	TLSelectionSet *selSet, TLSelectionSet **newSet,
	int64_t offsetPos, unsigned int trackId )
{
	m_document = doc;
	m_position = offsetPos;
	m_trackId = trackId;
	m_selSetPointer = newSet;
	ItemEssentials e;
	for ( TLSelItemList::Node *node = selSet->GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		current->GetItem()->GetEssentials(e);
		e.position = m_position + e.position - selSet->GetX1();
		e.trackId = m_trackId + e.trackId - selSet->GetTrack1();
		m_itemList.Append(
			new GgseqUndoItem( e,
				current->GetItem() )
/*			new GgseqUndoItem( current->GetSample()->GetFilename(),
				m_position + current->GetPosition() - selSet->GetX1(),
				m_trackId + current->GetTrack() - selSet->GetTrack1(),
				current->GetItem()->GetReference(), current->GetItem())*/
		);
	}
	selSet->Clear(m_document);
	m_error = 0;

}
void GgseqMoveItemsCommand::Do()
{
	if (m_selSetPointer != 0) {
		*m_selSetPointer = new TLSelectionSet();
	}
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		TLItem *item;
		if (current->m_itemReference) {
			item = current->m_itemReference;
			current->m_itemReference = (TLItem*)0;
			if (!current->m_essentials.referenceId)
				current->m_essentials.referenceId = m_docManager->GetNewRefId();
		} else {
			item = m_document->GetItem(current->m_essentials.referenceId);
		}
		int64_t oldPosition = item->GetPosition();
		unsigned int oldTrackId = item->GetTrack();
		TLSample *sample = item->GetSample();
		sample->Ref();
		m_document->DeleteItem( item, oldTrackId );
		item = m_document->AddItem( current->m_essentials
/*			sample,
			current->m_position,
			current->m_trackId,
			current->m_referenceId*/
		);
		current->m_essentials.position = oldPosition;
		current->m_essentials.trackId = oldTrackId;
		sample->UnRef();

		if ( item && m_selSetPointer ) {
			(*m_selSetPointer)->AddSample(item->GetSample(), item);
		}
	}
	if (m_selSetPointer != 0) {
		(*m_selSetPointer)->CalcBounds();
		m_selSetPointer = 0;
	}
}
void GgseqMoveItemsCommand::Undo()
{
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		TLItem *item = m_document->GetItem(current->m_essentials.referenceId);
		int64_t newPosition = item->GetPosition();
		unsigned int newTrackId = item->GetTrack();
		TLSample *sample = item->GetSample();
		sample->Ref();
		m_document->DeleteItem( item, newTrackId );
		m_document->AddItem( current->m_essentials
				/*sample,
			current->m_position,
			current->m_trackId,
			current->m_referenceId*/ );
		current->m_essentials.position = newPosition;
		current->m_essentials.trackId = newTrackId;
		sample->UnRef();
	}
}
/* -- GgseqDeleteItemsCommand -- */

GgseqDeleteItemsCommand::GgseqDeleteItemsCommand( TLData *doc, TLSelectionSet *selSet )
{
	m_document = doc;
	ItemEssentials e;
	for ( TLSelItemList::Node *node = selSet->GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		current->GetItem()->GetEssentials(e);
		m_itemList.Append(
			new GgseqUndoItem( e,
				/*current->GetSample()->GetFilename(),
				current->GetPosition(),
				current->GetTrack(),
				current->GetItem()->GetReference(),*/ current->GetItem())
		);
	}
	selSet->Clear(m_document);
	m_error = 0;
}
void GgseqDeleteItemsCommand::Do()
{
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		TLItem *item;
		if (current->m_itemReference) {
			item = current->m_itemReference;
			current->m_itemReference = (TLItem*)0;
			if (!current->m_essentials.referenceId)
				current->m_essentials.referenceId = m_docManager->GetNewRefId();
		} else {
			item = m_document->GetItem(current->m_essentials.referenceId);
		}
		m_document->DeleteItem( item, item->GetTrack() );
	}

}
void GgseqDeleteItemsCommand::Undo()
{
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		m_document->AddItem( current->m_essentials
				/*current->m_filename,
			current->m_position,
			current->m_trackId,
			current->m_referenceId*/ );
	}

}
/* -- GgseqTrackCommand -- */
GgseqTrackCommand::~GgseqTrackCommand()
{
	m_itemList.DeleteContents(true);
	m_itemList.Clear();
}
/* -- GgseqDeleteTrackCommand -- */
GgseqDeleteTrackCommand::GgseqDeleteTrackCommand( TLData *doc, TLTrack *track, TLPanel *panel )//TODO Panel entfernen
{
	m_error = 0;
	m_document = doc;
	m_trackNr = track->GetTrackNr();
	m_panel = panel;
	ItemEssentials e;
	for ( TLItemList::Node *node = track->GetFirst(); node; node = node->GetNext() ) {
		TLItem *current = node->GetData();
		current->GetEssentials(e);
		m_itemList.Append(
			new GgseqUndoItem( e,
				/*current->GetSample()->GetFilename(),
				current->GetPosition(),
				m_trackNr,
				current->GetReference(),*/ current)
		);
	}
	m_error = 0;

}
void GgseqDeleteTrackCommand::Do()
{
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		TLItem *item;
		if ( current->m_itemReference ) {
			item = current->m_itemReference;
			current->m_itemReference = (TLItem*)0;
			if ( !current->m_essentials.referenceId )
				current->m_essentials.referenceId = m_docManager->GetNewRefId();
		} else {
			item = m_document->GetItem( current->m_essentials.referenceId );
		}
		m_document->DeleteItem( item, m_trackNr );
	}

	m_document->DeleteTrack(m_trackNr);
}
void GgseqDeleteTrackCommand::Undo()
{
	m_document->AddTrack(m_trackNr);
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		m_document->AddItem( current->m_essentials
			/*current->m_filename,
			current->m_position,
			m_trackNr,
			current->m_referenceId*/
			);
	}

}
GgseqAddTrackCommand::GgseqAddTrackCommand( TLData *doc, int trackNr, TLPanel *panel )
{
	m_document = doc;
	m_trackNr  = trackNr;
	m_panel    = panel;
	m_error    = 0;
}
void GgseqAddTrackCommand::Do()
{
	m_document->AddTrack( m_trackNr );
}
void GgseqAddTrackCommand::Undo()
{
	m_document->DeleteTrack( m_trackNr );
}

GgseqEnvelopeItemCommand::GgseqEnvelopeItemCommand(
		TLData *doc, TLItem *item, const NativeEnvData &envelope )
{
/*	for ( int i = 0; i < 4; i++ ) {
		m_fades[i] = envelope[i];
	}*/
	m_nativeEnvelope = envelope;
	m_item     = item;
	m_document = doc;
	m_error    = 0;
	m_referenceId = item->GetReference();
}
GgseqEnvelopeItemCommand::~GgseqEnvelopeItemCommand()
{
}
void GgseqEnvelopeItemCommand::Do()
{
/*	if (!m_referenceId) {
		m_referenceId = m_docManager->GetNewRefId();
		if (m_item)
			m_document->SetItemReferenceId( m_item, m_referenceId );
	}
	TLItem *item;
	if (m_item) {
		item = m_item;
		m_item = (TLItem*) 0;
	} else {
		item = m_document->GetItem( m_referenceId );
	}*/
	TLItem *item = GetItem();
/*	for ( int i = 0; i < 4; i++ ) {//TODO: auch swap Funktion verwenden
		EnvelopePoint tmp;
		tmp = item->m_sampleEnvData[i];
		item->m_sampleEnvData[i] = m_fades[i];
		m_fades[i] = tmp;
	}*/
	NativeEnvData tmp;
	item->GetEnvelopeData( tmp );
	item->SetEnvelopeData( m_nativeEnvelope );
	m_nativeEnvelope = tmp;
}
void GgseqEnvelopeItemCommand::Undo()
{
	TLItem *item = m_document->GetItem(m_referenceId);
	wxASSERT(item);
	NativeEnvData tmp;
	item->GetEnvelopeData( tmp );
	item->SetEnvelopeData( m_nativeEnvelope );
	m_nativeEnvelope = tmp;
}
//---
GgseqToggleEnvelopeItemCommand::GgseqToggleEnvelopeItemCommand( TLData *doc, TLItem *item )
{
	m_document    = doc;
	m_item        = item;
	m_referenceId = item->GetReference();
	m_error       = 0;
}
GgseqToggleEnvelopeItemCommand::~GgseqToggleEnvelopeItemCommand()
{
}
void GgseqToggleEnvelopeItemCommand::Do()
{
/*	if (!m_referenceId) {
		m_referenceId = m_docManager->GetNewRefId();
		if (m_item)
			m_document->SetItemReferenceId( m_item, m_referenceId );
	}
	TLItem *item;
	if (m_item) {
		item = m_item;
		m_item = (TLItem*) 0;
	} else {
		item = m_document->GetItem( m_referenceId );
	}*/
	TLItem *item = GetItem();
	item->m_toggleEnvelope = !item->m_toggleEnvelope;
}
void GgseqToggleEnvelopeItemCommand::Undo()
{
	TLItem *item = m_document->GetItem(m_referenceId);
	wxASSERT(item);
	item->m_toggleEnvelope = !item->m_toggleEnvelope;
}
//---
GgseqTrimNStretchItemCommand::GgseqTrimNStretchItemCommand(
		TLData *doc, TLItem *item,
		gg_tl_dat leftTrim,
		gg_tl_dat rightTrim, float timestretch )
{
	m_document    = doc;
	m_item        = item;
	m_leftTrim    = leftTrim;
	m_rightTrim   = rightTrim;
	m_timestretch = timestretch;
	m_error       = 0;
	m_referenceId = item->GetReference();
	
}
GgseqTrimNStretchItemCommand::~GgseqTrimNStretchItemCommand()
{
}
/*void swap_l( gg_tl_dat &a, gg_tl_dat &b )
{
	gg_tl_dat c = a;
	a = b;
	b = c;
}
void swap_f( float &a, float &b )
{
	float c = a;
	a = b;
	b = c;
}*/
void GgseqTrimNStretchItemCommand::Do()
{
	TLItem *item  = GetItem();
	gg_tl_dat tmp_leftTrim    = item->GetLeftTrim();
	gg_tl_dat tmp_rightTrim   = item->GetRightTrim();
	float tmp_timestretch = item->GetTimestretch();
	item->SetTrimNStretch( m_leftTrim, m_rightTrim, m_timestretch );
	m_leftTrim    = tmp_leftTrim;
	m_rightTrim   = tmp_rightTrim;
	m_timestretch = tmp_timestretch;
	/*swap_l( item->m_leftTrim, m_leftTrim );
	swap_l( item->m_rightTrim, m_rightTrim );
	swap_f( item->m_timestretch, m_timestretch );*/
//	item->UpdateBuffer();
}
void GgseqTrimNStretchItemCommand::Undo()
{
	std::cout << m_referenceId << std::endl;
	TLItem *item = m_document->GetItem(m_referenceId);
	wxASSERT(item);
	gg_tl_dat tmp_leftTrim    = item->GetLeftTrim();
	gg_tl_dat tmp_rightTrim   = item->GetRightTrim();
	float tmp_timestretch = item->GetTimestretch();
	item->SetTrimNStretch( m_leftTrim, m_rightTrim, m_timestretch );
	m_leftTrim    = tmp_leftTrim;
	m_rightTrim   = tmp_rightTrim;
	m_timestretch = tmp_timestretch;
/*	swap_l( item->m_leftTrim, m_leftTrim );
	swap_l( item->m_rightTrim, m_rightTrim );
	swap_f( item->m_timestretch, m_timestretch );*/
//	item->UpdateBuffer();
}
