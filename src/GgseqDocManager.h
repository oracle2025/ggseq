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
class GgseqUndoItem;
class TLSelectionSet;
WX_DECLARE_LIST(GgseqCommand, GgseqCommandList);
WX_DECLARE_LIST(GgseqUndoItem, GgseqUndoItemList);
class TLPanel;

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
		long GetNewRefId();
		void Reset();
	private:
		TLData *m_document;
		GgseqCommandList m_commandList;
		GgseqCommandList m_redoList;
		long m_referenceCount;
};

class GgseqCommand
{
	public:
		void SetDocManager( GgseqDocManager *docManager );
		virtual void Do() = 0;
		virtual void Undo() = 0;
		virtual ~GgseqCommand() {};
		bool GetError();
	protected:
		TLData *m_document;
		long m_referenceId;
		GgseqDocManager *m_docManager;
		bool m_error;
};

class GgseqSingleItemCommand : public GgseqCommand
{
	protected:
		wxString m_filename;
		int64_t m_position;
		unsigned int m_trackId; /*Track ID*/
		wxRect m_FadeInOut[4]; //???
		bool m_toggleEnvelope;
};

class GgseqAddItemCommand : public GgseqSingleItemCommand
{
	public:
		GgseqAddItemCommand( TLData *doc, const wxString& filename,
		                     int64_t position, unsigned int trackId,
		                     TLSample *sample = (TLSample*) 0 );
		void Do();
		void Undo();
	private:
		TLSample *m_sample;
};

class GgseqDeleteItemCommand : public GgseqSingleItemCommand
{
	public:
		GgseqDeleteItemCommand( TLData *doc, TLItem *item );
		void Do();
		void Undo();
	private:
		TLItem *m_item;
};

class GgseqMoveItemCommand : public GgseqSingleItemCommand
{
	public:
		GgseqMoveItemCommand( TLData *doc, TLItem *srcItem,
		                      int64_t destPosition, unsigned int destTrackId );
		void Do();
		void Undo();
	private:
		TLItem *m_item;
};

class GgseqBunchOfItemsCommand : public GgseqCommand
{
	public:
	virtual ~GgseqBunchOfItemsCommand();
	protected:
		GgseqUndoItemList m_itemList;
};

class GgseqAddItemsCommand : public GgseqBunchOfItemsCommand /*wird beim Kopieren mit der rechten Maustaste verwendet.*/
{
	public:
		GgseqAddItemsCommand( TLData *doc,
			TLSelectionSet *selSet,
			TLSelectionSet **newSet,
			int64_t offsetPos,
			unsigned int trackId );
		void Do();
		void Undo();
	private:
		int64_t m_position;
		unsigned int m_trackId;
		TLSelectionSet **m_selSetPointer;
		TLSelectionSet *m_selectionSet;
};

class GgseqMoveItemsCommand : public GgseqBunchOfItemsCommand
{
	public:
		GgseqMoveItemsCommand( TLData *doc,
			TLSelectionSet *selSet,
			TLSelectionSet **newSet,
			int64_t offsetPos,
			unsigned int trackId );
		void Do();
		void Undo();
	private:
		int64_t m_position;
		unsigned int m_trackId;
		TLSelectionSet **m_selSetPointer;
		TLSelectionSet *m_selectionSet;
};
class GgseqDeleteItemsCommand : public GgseqBunchOfItemsCommand
{
	public:
		GgseqDeleteItemsCommand( TLData *doc,
			TLSelectionSet *selSet );
		void Do();
		void Undo();
};

class GgseqTrackCommand : public GgseqCommand
{
	public:
		virtual ~GgseqTrackCommand();
	protected:
		GgseqUndoItemList m_itemList;/*nur die TrackID ignorieren.*/
		int m_trackNr;
		TLPanel *m_panel;
};

class GgseqDeleteTrackCommand : public GgseqTrackCommand
{
	public:
		GgseqDeleteTrackCommand( TLData *doc, TLTrack *track, TLPanel *panel );
		void Do();
		void Undo();
};
class TLPanel;
class GgseqAddTrackCommand : public GgseqTrackCommand
{
	public:
		GgseqAddTrackCommand( TLData *doc, int trackNr=-1, TLPanel *panel=0 );
		void Do();
		void Undo();
};
class GgseqMoveTrackCommand : public GgseqTrackCommand
{
};
//TODO: Die ganzen Listen beim beenden löschen.


class GgseqEnvelopeItemCommand : public GgseqCommand
{
	public:
		GgseqEnvelopeItemCommand( TLData *doc, TLItem *item, EnvelopePoint *envelope );
		~GgseqEnvelopeItemCommand();
		void Do();
		void Undo();
	private:
		EnvelopePoint m_fades[4];
		TLItem *m_item;
};
class GgseqToggleEnvelopeItemCommand : public GgseqCommand
{
	public:
		GgseqToggleEnvelopeItemCommand( TLData *doc, TLItem *item );
		~GgseqToggleEnvelopeItemCommand();
		void Do();
		void Undo();
	private:
		TLItem *m_item;
};
class GgseqTrimNStretchItemCommand
{
};
class GgseqVolumeTrackCommand
{
};
// kein GgseqMuteTrackCommand;

#endif /* _GGSEQ_DOC_MANAGER_H_ */

