/* GgseqDocManager.h
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


#ifndef _GGSEQ_DOC_MANAGER_H_
#define _GGSEQ_DOC_MANAGER_H_

class GgseqCommand;
WX_DECLARE_LIST(GgseqCommand, GgseqCommandList);

class GgseqDocManager
{
	public:
		GgseqDocManager( TLData *data );
		~GgseqDocManager();
		void SubmitCommand( GgseqCommand *command ); /*DocManager takes Ownership*/
		void Undo();
		void Redo();
		bool CanUndo();
		bool CanRedo();
	private:
		TLData *m_document;
		GgseqCommandList m_commandList;
		GgseqCommandList m_redoList;
};

class GgseqCommand
{
	public:
		virtual void Do() = 0;
		virtual void Undo() = 0;
//		virtual ~GgseqCommand() = 0;
	protected:
		TLData *m_document;
};

class GgseqSingleItemCommand : public GgseqCommand
{
	public:
		virtual void Do() = 0;
		virtual void Undo() = 0;
	protected:
		wxString m_filename;
		int64_t m_position;
		unsigned int m_trackId; /*Track ID*/
};

class GgseqAddItemCommand : public GgseqSingleItemCommand
{
	public:
		GgseqAddItemCommand( TLData *doc, const wxString& filename,
		                     int64_t position, unsigned int trackId );
		void Do();
		void Undo();
	private:
		TLItem *m_itemReference;
};

class GgseqDeleteItemCommand : public GgseqSingleItemCommand
{
	public:
		GgseqDeleteItemCommand( TLData *doc, TLItem *item );
		void Do();
		void Undo();
	private:
		TLItem *m_item;
/*		Do(){
			m_filename, position, track <-item;
			Document->Delete(item_Reference);
		}*/
};

class GgseqMoveItemCommand : public GgseqSingleItemCommand
{
	public:
		GgseqMoveItemCommand( TLData *doc, TLItem *srcItem,
		                      int64_t destPosition, unsigned int destTrackId );
		void Do();
		void Undo();
/*	Do(){
		Document->Remove(item_reference);
		store old item position, track;
		m_itemReference=Document->Add(sample_reference, position, track);
	}*/
	private:
		TLItem *m_itemReference;
};

#endif /* _GGSEQ_DOC_MANAGER_H_ */

