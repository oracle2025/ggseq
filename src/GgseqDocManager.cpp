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

#include "stuff.h"
#include "TLData.h"
#include "TLItem.h"
#include "TLSample.h"
#include "TLSelectionSet.h"
#include "GgseqDocManager.h"

#define MAX_UNDO 50

WX_DEFINE_LIST(GgseqCommandList);

//static unsigned int global_referenceCount = 0;

class GgseqUndoItem
{
	public:
		GgseqUndoItem(
		//	TLItem *itemReference,
			wxString filename,
			int64_t position,
			unsigned int trackId,
			long referenceId = 0,
			TLItem *itemReference = (TLItem*)0
		)
		{
			m_itemReference = itemReference;
			m_filename = filename;
			m_position = position;
			m_trackId = trackId;
			m_referenceId = referenceId;
		}
		TLItem *m_itemReference;/*ist nicht vermeidbar sein*/
		long m_referenceId;
		wxString m_filename;
		int64_t m_position;
		unsigned int m_trackId;
};
WX_DEFINE_LIST(GgseqUndoItemList);

/* -- GgseqDocManager -- */
GgseqDocManager::GgseqDocManager( TLData *data )
{
	m_document = data;
	m_referenceCount = 1;
}
GgseqDocManager::~GgseqDocManager()
{/*Listen löschen ?? nur move item und add item haben referencen, und die werden woanders gelöscht.*/
	m_commandList.DeleteContents(true);
	m_commandList.Clear();
	m_redoList.DeleteContents(true);
	m_redoList.Clear();
}
void GgseqDocManager::SubmitCommand( GgseqCommand *command )
{
	command->SetDocManager(this);/*TODO eigentlich nur für AddItem nötig.*/
	command->Do();
	if (command->GetError() ) {
		delete command;
	} else {
		if (m_commandList.GetCount>MAX_UNDO) {
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
}
//void GgseqCommand::SetReferenceId( unsigned int referenceId )
//{
//	m_referenceId = referenceId;
//}
void GgseqCommand::SetDocManager( GgseqDocManager *docManager )
{
	m_docManager = docManager;
}
bool GgseqCommand::GetError() { return m_error; }
void GgseqDocManager::Undo()
{/*-> Dingsda in die Redo liste tun:*/
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
	m_filename = filename;
	m_position = position;
	m_trackId = trackId;
	m_referenceId = 0;
	m_error = 0;
}
void GgseqAddItemCommand::Do()
{
	if (!m_referenceId)
		m_referenceId = m_docManager->GetNewRefId();
	if (m_sample) {
		m_document->AddItem( m_sample, m_position, m_trackId, m_referenceId );
	} else {
		if (m_document->AddItem( m_filename, m_position, m_trackId, m_referenceId ) == 0)
			m_error = 1;
	}
}
void GgseqAddItemCommand::Undo()
{
		std::cout << "AddItem::Undo " << m_referenceId << std::endl;

	TLItem *item = m_document->GetItem(m_referenceId);
	m_document->DeleteItem( item , m_trackId );
	m_sample = (TLSample*) 0;
}

/* -- GgseqDeleteItemCommand -- */
GgseqDeleteItemCommand::GgseqDeleteItemCommand( TLData *doc, TLItem *item )
{
	m_document = doc;
//	m_referenceId = referenceId;
	m_item = item;
	m_referenceId = item->GetReference();
	if (!m_referenceId)
		m_referenceId = m_docManager->GetNewRefId();
	m_filename = item->GetSample()->GetFilename();
	m_position = item->GetPosition();
	m_trackId = item->GetTrack();
	m_error = 0;
}
void GgseqDeleteItemCommand::Do()
{
	if (m_item) {
		m_document->DeleteItem( m_item, m_trackId );
		m_item = (TLItem*) 0;
	} else {
		TLItem *item = m_document->GetItem(m_referenceId); /* xxx (siehe oben)*/
		m_document->DeleteItem( item, m_trackId );
	}
}
void GgseqDeleteItemCommand::Undo()
{
	/*m_item = */m_document->AddItem( m_filename, m_position, m_trackId, m_referenceId );
}

/* -- GgseqMoveItemCommand -- */
GgseqMoveItemCommand::GgseqMoveItemCommand( TLData *doc, TLItem *srcItem,
                                            int64_t destPosition, unsigned int destTrackId )
{
puts("GgseqMoveItemCommand");
	m_document = doc;
//	m_itemReference = srcItem;
	m_referenceId = srcItem->GetReference();
	if (!m_referenceId)
		m_referenceId = m_docManager->GetNewRefId();
	m_position = destPosition;
	m_trackId = destTrackId;
	m_item = srcItem;
	m_error = 0;
}
void GgseqMoveItemCommand::Do()
{
	TLItem *item;
	if (m_item) {
		item = m_item;
		m_item = (TLItem*) 0;
	} else {
		item = m_document->GetItem(m_referenceId);
	}
	m_filename = item->GetSample()->GetFilename();
	int64_t oldPositon = item->GetPosition();
	unsigned int oldTrackId = item->GetTrack();
	TLSample *sample = item->GetSample();
	sample->Ref();
	m_document->DeleteItem( item, oldTrackId );
	m_document->AddItem( sample, m_position, m_trackId , m_referenceId );
	m_position = oldPositon;
	m_trackId = oldTrackId;
	sample->UnRef();
	
}
void GgseqMoveItemCommand::Undo()
{
	TLItem *item = m_document->GetItem(m_referenceId);
	int64_t newPositon = item->GetPosition();
	unsigned int newTrackId = item->GetTrack();
//	TLItem *newItem = item;
	TLSample *sample = item->GetSample();
	sample->Ref();
	m_document->DeleteItem( item, newTrackId );
	m_document->AddItem( sample, m_position, m_trackId, m_referenceId );
	m_position = newPositon;
	m_trackId = newTrackId;
	sample->UnRef();
	//m_itemReference = (TLItem*) 0;
}
/* -- GgseqAddItemsCommand -- */
GgseqAddItemsCommand::GgseqAddItemsCommand( TLData *doc, TLSelectionSet *selSet, TLSelectionSet **newSet, int64_t offsetPos, unsigned int trackId )/*Wird wohl immer seine ReferenceIds selbst erzeugen*/
{
	m_document = doc;
	m_position = offsetPos;
	m_trackId = trackId;
	m_selSetPointer = newSet;
//	m_selectionSet = selSet;
//	m_itemList.Append();
#if 1
	for ( TLSelItemList::Node *node = selSet->GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();/*selset->AddTo gibt ein neues SelSet zurück?*/
		m_itemList.Append(
			new GgseqUndoItem( current->GetSample()->GetFilename(),
				m_position + current->GetPosition() - selSet->GetX1(),
				m_trackId + current->GetTrack() - selSet->GetTrack1() )
		);
	}
#endif
	m_error = 0;
}

void GgseqAddItemsCommand::Do()
{
	if (m_selSetPointer != 0) {
		*m_selSetPointer = new TLSelectionSet();
	}
	//m_selectionSet = m_selectionSet->AddTo(m_document, m_position, m_trackId);
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		if (current->m_referenceId == 0)
			current->m_referenceId = m_docManager->GetNewRefId();
		TLItem *item = m_document->AddItem(
			current->m_filename,
			current->m_position,
			current->m_trackId,
			current->m_referenceId
		);
		/*current->m_itemReference = m_document->AddItem(
			current->m_filename,
			current->m_position,
			current->m_trackId
		);*/
		//if (!m_referenceId)
		//m_referenceId = m_docManager->GetNewRefId();
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
		TLItem *item = m_document->GetItem(current->m_referenceId);
		m_document->DeleteItem( item, current->m_trackId );
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
	for ( TLSelItemList::Node *node = selSet->GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		m_itemList.Append(
			new GgseqUndoItem( current->GetSample()->GetFilename(),
				m_position + current->GetPosition() - selSet->GetX1(),
				m_trackId + current->GetTrack() - selSet->GetTrack1(),
				current->GetItem()->GetReference(), current->GetItem())
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
			if (!current->m_referenceId)
				current->m_referenceId = m_docManager->GetNewRefId();
		} else {
			item = m_document->GetItem(current->m_referenceId);
		}
		std::cout << "MoveItems::Do " << current->m_referenceId << std::endl;
		int64_t oldPosition = item->GetPosition();
		unsigned int oldTrackId = item->GetTrack();
		TLSample *sample = item->GetSample();
		sample->Ref();
		m_document->DeleteItem( item, oldTrackId );
		item = m_document->AddItem(
			sample,
			current->m_position,
			current->m_trackId,
			current->m_referenceId
		);
		current->m_position = oldPosition;
		current->m_trackId = oldTrackId;
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
		TLItem *item = m_document->GetItem(current->m_referenceId);
		int64_t newPosition = item->GetPosition();
		unsigned int newTrackId = item->GetTrack();
		TLSample *sample = item->GetSample();
		sample->Ref();
		m_document->DeleteItem( item, newTrackId );
		m_document->AddItem( sample,
			current->m_position,
			current->m_trackId,
			current->m_referenceId );
		current->m_position = newPosition;
		current->m_trackId = newTrackId;
		sample->UnRef();
	}
}
/* -- GgseqDeleteItemsCommand -- */

GgseqDeleteItemsCommand::GgseqDeleteItemsCommand( TLData *doc, TLSelectionSet *selSet )
{
	m_document = doc;
	for ( TLSelItemList::Node *node = selSet->GetFirst(); node; node = node->GetNext() ) {
		TLSelectionItem *current = node->GetData();
		m_itemList.Append(
			new GgseqUndoItem( current->GetSample()->GetFilename(),
				current->GetPosition(),
				current->GetTrack(),
				current->GetItem()->GetReference(), current->GetItem())
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
			if (!current->m_referenceId)
				current->m_referenceId = m_docManager->GetNewRefId();
		} else {
			item = m_document->GetItem(current->m_referenceId);
		}
		m_document->DeleteItem( item, item->GetTrack() );
	}

}
void GgseqDeleteItemsCommand::Undo()
{
	for ( GgseqUndoItemList::Node *node = m_itemList.GetFirst(); node; node = node->GetNext() ) {
		GgseqUndoItem *current = node->GetData();
		m_document->AddItem( current->m_filename,
			current->m_position,
			current->m_trackId,
			current->m_referenceId );
	}

}

