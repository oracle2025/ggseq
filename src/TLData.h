/* TLData.h
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

#ifndef _TLDATA_H_
#define _TLDATA_H_

class TLItem;
class TLSample;
class TLTrack;
class TLSampleManager;
class TLColourManager;
WX_DECLARE_LIST(TLTrack, TLTrackList);

class TLData
{
	public:
		TLData();
		~TLData();
		TLTrackList::Node *GetFirst(); /*Soll weg*/
		int GetTrackCount();
		void AddTrack();

		TLItem *AddItem(TLSample *sample, int Position, int TrackNr);
		TLItem *AddItem(wxString filename, int Position, int TrackNr);
		TLItem *ItemAtPos(int Position, int TrackNr);
		void SetItemPosition(TLItem *item,int Position);
		void DeleteItem(TLItem *item, int TrackNr);

		void SortAll();
		void Clear();
		void Block();
		void UnBlock();
		bool IsBlocked();

		unsigned int FillBuffer(float* outBuffer, unsigned int count);
		void SetPlaybackPosition(long Position);
		long GetPlaybackPosition();
		int GetLength();

		TLColourManager *GetColourManager();

		/* -- Load, Save -- */
		wxString GetFilename();
		bool UnsavedChanges();
		void Save(wxString filename);
		void Save();
		void Load(wxString filename);
		void SaveWAV(wxString filename);

		void ClearSample(TLSample *sample);
	private:
		int mixChannels(float *A, float *B, float* out);
		void printXML(wxString filename);
		void loadXML(wxString filename);
		void ResetOffsets();
		TLTrackList m_trackList;
		TLSampleManager *m_sampleManager;
		wxString m_filename;
		bool m_changed;
		bool m_blocked;
		long m_playbackPosition;
		int m_length;
};

#endif /*_TLDATA_H_*/
