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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <wx/listimpl.cpp>

#include "stuff.h"
#include "TLData.h"
#include "GgseqDocManager.h"

#define MAX_UNDO 50

WX_DEFINE_LIST(GgseqCommandList);

/* -- GgseqDocManager -- */
GgseqDocManager::GgseqDocManager( TLData *data )
{
	m_document = data;
}
GgseqDocManager::~GgseqDocManager()
{/*Listen löschen ?? nur move item und add item haben referencen, und die werden woanders gelöscht.*/
	m_commandList.DeleteContents(true);
	m_commandList.Clear();
}
void GgseqDocManager::SubmitCommand( GgseqCommand *command )
{
	command->Do();
	m_commandList.Append(command);
	/*Wenn noch 'sachen' in der RedoListe sind, diese löschen*/
}
void GgseqDocManager::Undo()
{/*-> Dingsda in die Redo liste tun:*/
	
}
void GgseqDocManager::Redo()
{
}
bool GgseqDocManager::CanUndo()
{
	return false;
}
bool GgseqDocManager::CanRedo()
{
	return false;
}

/* -- GgseqAddItemCommand -- */
GgseqAddItemCommand::GgseqAddItemCommand( TLData *doc, const wxString& filename,
                                          int64_t position, unsigned int trackId )

{
	m_document = doc;
	m_filename = filename;
	m_position = position;
	m_trackId = trackId;
}
void GgseqAddItemCommand::Do()
{
	m_itemReference = m_document->AddItem( m_filename, m_position, m_trackId );
}
void GgseqAddItemCommand::Undo()
{
	m_document->DeleteItem( m_itemReference, m_trackId );
	m_itemReference = (TLItem*) 0;
}

/* -- GgseqDeleteItemCommand -- */
GgseqDeleteItemCommand::GgseqDeleteItemCommand( TLData *doc, TLItem *item )
{
	m_document = doc;
	m_item = item;
}
void GgseqDeleteItemCommand::Do()
{
	//m_filename =
	//m_position =
	//m_trackId = 
	m_document->DeleteItem( m_item, m_trackId );
	m_item = (TLItem*) 0;
}
void GgseqDeleteItemCommand::Undo()
{
	m_item = m_document->AddItem( m_filename, m_position, m_trackId );
}

/* -- GgseqMoveItemCommand -- */
GgseqMoveItemCommand::GgseqMoveItemCommand( TLData *doc, TLItem *srcItem,
                                            int64_t destPosition, unsigned int destTrackId )
{
	m_document = doc;
	m_itemReference = srcItem;
	m_position = destPosition;
	m_trackId = destTrackId;
}
void GgseqMoveItemCommand::Do()
{
}
void GgseqMoveItemCommand::Undo()
{
}
