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
class UpdateListener;
WX_DECLARE_LIST(TLTrack, TLTrackList);
WX_DECLARE_HASH_MAP( unsigned int, TLItem*, wxIntegerHash, wxIntegerEqual, TLItemHash );

#include "Ruler.h"


class TLData: public LoopSetupListener
{
	public:
		TLData();
		~TLData();
		TLTrackList::Node *GetFirst(); /*Soll weg*/
		int GetTrackCount();
		void AddTrack();
		void SetLoopSnaps(gg_tl_dat pos1, gg_tl_dat pos2);

		TLItem *AddItem(TLSample *sample,gg_tl_dat  Position, int TrackNr, long referenceId = 0 );
		TLItem *AddItem(wxString& filename, gg_tl_dat Position, int TrackNr, long referenceId = 0 );
		TLItem *ItemAtPos(gg_tl_dat Position, int TrackNr);
		void SetItemPosition(TLItem *item, gg_tl_dat Position);
		void DeleteItem(TLItem *item, int TrackNr/*, long referenceId*/ );
		void SetTrackMute(bool mute, int TrackNr);
		void SetTrackVolume(double vol, int TrackNr);

		void SortAll();
		void Clear();
		void Block();
		void UnBlock();
		bool IsBlocked();

		unsigned int FillBuffer(float* outBuffer, unsigned int count);
		unsigned int FillLoopBuffer(float* outBuffer, unsigned int count);
		void SetPlaybackPosition(gg_tl_dat Position);
		gg_tl_dat GetPlaybackPosition();
		gg_tl_dat GetLength();

		TLColourManager *GetColourManager();

		/* -- Load, Save -- */
		wxString GetFilename();
		bool UnsavedChanges();
		bool Save(wxString filename);
		bool Save();
		void Load(wxString filename);
		void SaveWAV(wxString filename);

		void ClearSample(TLSample *sample);
		void SetUpdateListener(UpdateListener *updateListener);
		void SetSnapValue(long snapValue);
		long GetSnapValue();
		void SetMasterVolume(float volume);
		gg_tl_dat m_position;/*Wird während des Abspielens inkrementiert*/
		TLItem* GetItem(long referenceId);
	private:
		gg_tl_dat m_loopPos1;
		gg_tl_dat m_loopPos2;
		bool m_loop_enabled;
		long m_snapValue;
		unsigned int MixChannels(float *A, float *B, float* out, unsigned int count);
		bool printXML(wxString filename);
		void loadXML(wxString filename);
		void ResetOffsets();
		TLTrackList m_trackList;
		TLSampleManager *m_sampleManager;
		wxString m_filename;
		bool m_changed;
		bool m_blocked;
		gg_tl_dat m_playbackPosition;
		gg_tl_dat m_length;
		float m_masterVolume;
		UpdateListener *m_updateListener;
		TLItemHash m_allItemsHash;
};

#endif /*_TLDATA_H_*/
