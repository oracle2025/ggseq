/* TLData.cpp
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
#include <sndfile.h>
#include <wx/hashmap.h>
#include <wx/filename.h>

//#include <iostream>
#include <fstream>

#include "stuff.h"

#include "TLXmlLoader2.h"
#include "UpdateListener.h"

#include "tinyxml.h"

#include "Ruler.h"
#include "TLItem.h"
#include "TLTrack.h"
#include "TLData.h"
#include "TLSample.h"
#include "TLSampleManager.h"
#include "GgseqDocManager.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TLTrackList);
#define LENGTH_TO_ADD 1176000
#define LENGTH_ADD 1170000

TLData::TLData()
{
	m_sampleManager=new TLSampleManager();
	m_trackList = new TLTrackList();
	m_blocked=false;
	m_changed=false;
	m_filename=wxT("");
	SetPlaybackPosition(0);
	m_length=2000000;
	m_updateListener=NULL;
	m_masterVolume=1.0;
	m_loop_enabled=false;
	m_docManager = 0;
	m_trackReferenceCounter = 1;
	m_trackList->DeleteContents(true);
	m_panel = 0;
}
void TLData::SetDocManager(GgseqDocManager *docManager) { m_docManager = docManager; }
TLData::~TLData()
{
	m_trackList->Clear();
	delete m_trackList;
	delete m_sampleManager;
}
int TLData::GetTrackCount()
{
	return m_trackList->GetCount();
}
void TLData::SetLoopSnaps(gg_tl_dat pos1, gg_tl_dat pos2)
{
	if ( m_blocked )
		return; //TODO, sollte auch im UI reflektiert werden.
	m_loopPos1=pos1;/* Umrechnen nach abc, was auch immer(Wird im Ruler gemacht)*/
	m_loopPos2=pos2;
	m_loop_enabled=true;
	m_playbackPosition=m_loopPos1;
/*	puts("abc-Test");*/
}
TLTrackList::Node *TLData::GetFirst()
{
	return m_trackList->GetFirst();
}

TLItem *TLData::ItemAtPos(gg_tl_dat Position, int TrackNr)
{
	TLTrack *tlTrack = m_trackList->Item(TrackNr)->GetData();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::ItemAtPos!" );
	if (!tlTrack)
		return (TLItem*)NULL;
	return tlTrack->ItemAtPos(Position);
}

void TLData::DeleteItem( TLItem *item, int TrackNr/*, long referenceId */)
{
	TLTrack *tlTrack = m_trackList->Item(TrackNr)->GetData();
	long referenceId = item->GetReference();
	TLSample *sample=item->GetSample();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::DeleteItem!" );
	if (!tlTrack)
		return;
	tlTrack->DeleteItem(item);
	m_sampleManager->Clear(sample);
	m_changed=true;
	m_allItemsHash.erase(referenceId);
}
void TLData::ClearSample(TLSample *sample)
{
	m_sampleManager->Clear(sample);
}

TLItem *TLData::AddItem(TLSample *sample,gg_tl_dat  Position, int TrackNr, long referenceId, const NativeEnvData *env, bool toggleEnvelope )
{
	TLTrackList::Node *node =  m_trackList->Item(TrackNr);
	if (!node)
		return NULL;
	TLTrack *tlTrack = node->GetData();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::AddItem!" );
	if (!tlTrack)
		return NULL;
	m_changed=true;
	if (Position+sample->GetLength()>m_length-LENGTH_ADD) {
		m_length=Position+sample->GetLength()+LENGTH_TO_ADD;
	}
	TLItem *item = tlTrack->AddItem(sample, Position, referenceId);
	if ( env ) {
		item->SetEnvelopeData( *env );
	/*	item->m_leftFadeIn   = env[0];
		item->m_rightFadeIn  = env[1];
		item->m_leftFadeOut  = env[2];
		item->m_rightFadeOut = env[3];*/
//		for ( int i = 0; i < 4; i++ ) {
			//item->m_guiEnvData[i] = env[i]; 
//		}
	}
	item->m_toggleEnvelope = toggleEnvelope;
	m_allItemsHash[referenceId]=item;
	return item;
}
TLItem *TLData::AddItem(wxString& filename, gg_tl_dat Position, int TrackNr, long referenceId )
{
	TLTrack *tlTrack = m_trackList->Item(TrackNr)->GetData();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::AddItem!" );
	if (!tlTrack)
		return NULL;
	m_updateListener->StartUpdateProcess();
	TLSample *sample = m_sampleManager->GetSample(filename, m_updateListener);
	m_updateListener->EndUpdateProcess();
	if (sample) {
		m_changed=true;
		if (Position+sample->GetLength()>m_length-LENGTH_ADD) {
			m_length=Position+sample->GetLength()+LENGTH_TO_ADD;
		}
		TLItem *item = tlTrack->AddItem(sample, Position, referenceId );
		m_allItemsHash[referenceId]=item;
		return item;
	}
	return NULL;
}
TLItem *TLData::AddItem( const ItemEssentials &e )
{
	TLTrack *tlTrack = m_trackList->Item(e.trackId)->GetData();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::AddItem!" );
	if (!tlTrack)
		return NULL;
	m_updateListener->StartUpdateProcess();
	TLSample *sample = m_sampleManager->GetSample(e.filename, m_updateListener);
	m_updateListener->EndUpdateProcess();
	if (sample) {
		m_changed=true;
		if (e.position+sample->GetLength()>m_length-LENGTH_ADD) {
			m_length=e.position+sample->GetLength()+LENGTH_TO_ADD;
		}
		TLItem *item = tlTrack->AddItem(sample, e);
		m_allItemsHash[e.referenceId]=item;
		return item;
	}
	return NULL;

}
void TLData::SetItemReferenceId( TLItem *item, long referenceId )
{
	if (item->GetReference()) {
		wxLogError(wxT("Internal Error: This Item has already a referenceId"));
		return;
	}
	item->SetReference(referenceId);
	m_allItemsHash[referenceId] = item;
}
void TLData::SortAll()/*m_length aktualisieren*/
{
	m_length=0;
	for ( TLTrackList::Node *node = m_trackList->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->SortItems();
		if (current->GetLength()>m_length) { m_length=current->GetLength(); }
	}
	ResetOffsets();
}

void TLData::AddTrack( /*long referenceId,*/ int trackPos )
{
	/*if (referenceId<=0) {
		referenceId = m_trackReferenceCounter;
		m_trackReferenceCounter++;
	}*/
	TLTrack *track = new TLTrack(/*m_trackList->GetCount(),*/ this, m_panel);
	if ( trackPos < 0 ) {
		m_trackList->Append(track);
	} else {
		m_trackList->Insert(trackPos, track);
	}
}
void TLData::DeleteTrack(int TrackNr)
{
//	TLTrack *track = m_trackList
	if (TrackNr<0)
		TrackNr = m_trackList->GetCount()-1;
	TLTrackList::Node *node = m_trackList->Item( TrackNr );
	m_trackList->DeleteNode( node );
}
/*int TLData::GetTrackNrFromRef( long referenceId )
{
	m_trackList->Find(referenceId)->IndexOf();
}*/
void TLData::ResetOffsets()
{
	for ( TLTrackList::Node *node = m_trackList->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->ResetOffsets();
	}
	m_position=m_playbackPosition;
}
void TLData::Block()
{
	m_blocked=true;
}
void TLData::UnBlock()
{
	m_blocked=false;
}
bool TLData::IsBlocked()
{
	return m_blocked;
}

void TLData::loadXML(wxString filename)
{
/*	CSAX2Parser parser;
	TLXMLLoader processor(this,m_sampleManager);
	parser.SetContentHandler(&processor);
	Clear();
	m_filename=filename;
	try
	{
		parser.Parse((const char*)filename.mb_str());
	}
	catch (CXMLException& e)
	{
//		std::cerr << "Error: " << e.GetMsg() << std::endl;
		Clear();
		wxLogError(wxT("Konnte Datei \"%s\" nicht laden"),filename.c_str());
//		exit(1);
		
	}
*/
	Clear();
	TLXMLLoader2 loader(this,m_sampleManager);
	m_filename=filename;
/*	SimpleUpdateListener listener;*/
	m_updateListener->StartUpdateProcess();
	loader.LoadFile(filename,m_updateListener/*&listener*/ );
	m_updateListener->EndUpdateProcess();
	m_changed=false;

}
void TLData::Clear() //TODO: auch wieder 8 Tracks herstellen.
{
	m_docManager->Reset();
	for ( TLTrackList::Node *node = m_trackList->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->Clear();
	}
	m_sampleManager->ClearAll();
	while(GetTrackCount()>8) {
		DeleteTrack(0);
	}
	while(GetTrackCount()<8) {
		AddTrack(-1);
	}

	m_changed=false;
	m_filename=wxT("");
}

bool TLData::ExportPackage(wxString filename)//TODO: Konflikte bei gleichnamigen Dateien vermeiden.
{
//Liste vom SampleManager kriegen, addXmlData ein Flag hinzufügen, das die dateien nach /tmp kopiert, und relative Pathnames vergibt.
	wxString tmp_dir = wxFileName::CreateTempFileName(wxT(""));
	wxRemoveFile(tmp_dir);
	wxMkdir(tmp_dir);
	wxFileName fn(filename);
	wxString xml_name = tmp_dir + wxFileName::GetPathSeparator() + fn.GetName() + wxT(".ggseq");
	if ( !printXML(xml_name, true, tmp_dir) ) {
		return false;
	}
	// Zip Up
	wxSetWorkingDirectory(tmp_dir);
	// system('zip ' + filename + ' -r .' )
	wxExecute(wxString(wxT("zip ")) + filename + wxT(" -r ."), wxEXEC_SYNC);
	wxExecute(wxString(wxT("rm -R ")) + tmp_dir, wxEXEC_SYNC);
	return true;
}

bool TLData::printXML(wxString filename, bool relative, wxString tmp_path)
{
	TiXmlDocument doc(filename.mb_str());
	
	TiXmlDeclaration *dec = new TiXmlDeclaration("1.0","","no");
	doc.LinkEndChild(dec);
	TiXmlElement *el = new TiXmlElement("song");
	doc.LinkEndChild(el);
	el->SetAttribute("version",GG_VERSION);
	el->SetAttribute("snap", m_snapValue);

	TiXmlElement *samples = new TiXmlElement("samples");
	el->LinkEndChild(samples);

	m_sampleManager->addXmlData(samples, relative, tmp_path);

	TiXmlElement *tracks = new TiXmlElement("tracks");
	el->LinkEndChild(tracks);
	tracks->SetAttribute("count",GetTrackCount());

	for ( TLTrackList::Node *node = m_trackList->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->addXmlData(tracks);
	}

	if(!doc.SaveFile()) {
		wxLogError(wxT("Could not save File \"%s\""),filename.c_str());
		return false;
	}

	m_filename=filename;
	m_changed=false;
	return true;
}

bool TLData::UnsavedChanges()
{
	return m_changed;
}
wxString TLData::GetFilename()
{
	return m_filename;
}
bool TLData::Save(wxString filename)
{
	return printXML(filename);
//	m_filename=filename;
//	m_changed=false;
}
bool TLData::Save()
{
	return printXML(m_filename);
//	m_changed=false;
}
void TLData::Load(wxString filename)
{
	loadXML(filename);
	m_changed=false;
}
void TLData::SaveWAV(wxString filename)
{
	SNDFILE *sndfile;
	SF_INFO sfinfo;
	sfinfo.samplerate=44100;
	sfinfo.channels=2;
	sfinfo.format=SF_FORMAT_WAV|SF_FORMAT_PCM_16;
	sndfile=sf_open(filename.mb_str(),SFM_WRITE,&sfinfo);
	if (SF_ERR_NO_ERROR!=sf_error(sndfile)) {
//		std::cerr << sf_strerror(sndfile) << std::endl;
		wxLogError(wxT("Could not save File \"%s\""),filename.c_str());
		return;/*mit wxLog arbeiten*/
	}
	float buffer[512];
	gg_tl_dat tmp=m_playbackPosition;
	SetPlaybackPosition(0);
	SortAll();
	unsigned int res=FillBuffer(buffer,512);
	/*m_length*/
	m_updateListener->StartUpdateProcess();
	gg_tl_dat cnt=0;
	while(res>=512) {
		sf_write_float(sndfile,buffer,512);
		if (SF_ERR_NO_ERROR!=sf_error(sndfile))
		{
			wxLogError(wxT("Could not save File \"%s\""),filename.c_str());
			return;
		}
		res=FillBuffer(buffer,512);
		cnt+=res;
		if (!(cnt%51200))
			if (m_updateListener->Update((cnt*100)/m_length)==false)
				break;
	}
	sf_write_float(sndfile,buffer,res);
	m_updateListener->EndUpdateProcess();
	if (SF_ERR_NO_ERROR!=sf_error(sndfile))
	{
		return;
	}
	sf_close(sndfile);
	SetPlaybackPosition(tmp);
}
void TLData::SetItemPosition(TLItem *item,gg_tl_dat Position)
{
	item->SetPosition(Position);
	m_changed=true;
	if (item->GetPosition()+item->GetSample()->GetLength()>m_length-LENGTH_ADD) {
		m_length=item->GetPosition()+item->GetSample()->GetLength()+LENGTH_TO_ADD;
	}

}
gg_tl_dat TLData::GetLength()
{
	return m_length;
}

void TLData::SetPlaybackPosition(gg_tl_dat Position)
{
	m_playbackPosition=Position;
	m_position=m_playbackPosition;
	m_loop_enabled=false;
}
gg_tl_dat TLData::GetPlaybackPosition()
{
	return m_playbackPosition;
}
unsigned int TLData::FillLoopBuffer(float* outBuffer, unsigned int count)
{
	if ( !m_loop_enabled ) {
		return FillBuffer(outBuffer, count);
	}
	unsigned int result;
	if (m_position+count>m_loopPos2) {
		unsigned int cnt_to_play = count-(m_loopPos2-m_position);
		result = FillBuffer(outBuffer, cnt_to_play);
		m_position=m_playbackPosition;
		ResetOffsets();
		FillBuffer(outBuffer+result, count-result);
		return count;
	}
	result = FillBuffer(outBuffer, count);
	if (result<count) {
		m_position=m_playbackPosition;
		ResetOffsets();
		FillBuffer(outBuffer+result, count-result);
	}
	return count;
}
unsigned int TLData::FillBuffer(float* outBuffer, unsigned int count)
{
	float buffer1[count];
	float buffer2[count];
	unsigned int maxResultCount=0;
	unsigned int rv;
	TLTrackList::Node *node = m_trackList->GetFirst();
	if (!node)
		return 0;
	rv=node->GetData()->FillBuffer(buffer1,count,m_position);/*first Track*/
	maxResultCount=rv;
	node = node->GetNext();
	if (!node) { /*Only one Track*/
		for (unsigned int i=0; i<count; i++)
			outBuffer[i]=buffer1[i];
		m_position+=maxResultCount;
		return maxResultCount;
	}
	rv=node->GetData()->FillBuffer(buffer2,count,m_position);/*second Track*/
	if (rv>maxResultCount)
		maxResultCount=rv;
	MixChannels(buffer1,buffer2,outBuffer,count);
	node = node->GetNext();
	while(node) {
	/*	if (!node->GetData()->IsMuted()){*/
			rv=node->GetData()->FillBuffer(buffer1,count,m_position);
			if (rv>maxResultCount)
				maxResultCount=rv;
			MixChannels(outBuffer,buffer1,outBuffer,count);
		/*}*/
		node = node->GetNext();
	}
	for (unsigned int i=0;i<maxResultCount;i++) {
		outBuffer[i]=outBuffer[i]*m_masterVolume;
	}
	m_position+=maxResultCount;
	
	return maxResultCount;
}
unsigned int TLData::MixChannels(float *A, float *B, float* out, unsigned int count)/*Mix function for (-1)-(1) float audio*/
{
	unsigned int i;
	float *p_output = out;
	float *p_A = A;
	float *p_B = B;
	for ( i=count; i > 0;i--){
		if (*p_A<0 && *p_B<0) {
			*p_output =(*p_A+1)*(*p_B+1)-1;
		} else {
			*p_output =2*(*p_A+*p_B+2)-(*p_A+1)*(*p_B+1)-3;
		}
		p_output++;
		p_A++;
		p_B++;

	}
	return count;
}
TLColourManager *TLData::GetColourManager()
{
	return m_sampleManager->GetColourManager();
}
void TLData::SetTrackMute(bool mute, int TrackNr)
{
	TLTrackList::Node *node =  m_trackList->Item(TrackNr);
	if (!node)
		return;
	TLTrack *tlTrack = node->GetData();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::AddItem!" );
	if (!tlTrack)
		return;
	m_changed=true;
	tlTrack->SetMute(mute);

}
void TLData::SetTrackVolume(double vol, int TrackNr)
{
	TLTrackList::Node *node =  m_trackList->Item(TrackNr);
	if (!node)
		return;
	TLTrack *tlTrack = node->GetData();
	wxASSERT_MSG( (tlTrack != NULL), "Track-Index out of Range in TLData::AddItem!" );
	if (!tlTrack)
		return;
	m_changed=true;
	tlTrack->SetVolume(vol);
}
void TLData::SetUpdateListener(UpdateListener *updateListener)
{
	m_updateListener=updateListener;
}
void TLData::SetSnapValue(long snapValue)
{
	m_snapValue=snapValue;
}
long TLData::GetSnapValue()
{
	return m_snapValue;
}
void TLData::SetMasterVolume(float volume) { m_masterVolume=volume;} 
TLItem* TLData::GetItem(long referenceId)
{
	return m_allItemsHash[referenceId];
}
void TLData::SelectTrack( int TrackNr )
{
	for ( TLTrackList::Node *node = m_trackList->GetFirst(); node; node = node->GetNext() ) {
		TLTrack *current = node->GetData();
		current->SetSelected(false);
	}

	TLTrackList::Node *node =  m_trackList->Item(TrackNr);
	if (!node)
		return;
	TLTrack *tlTrack = node->GetData();
	tlTrack->SetSelected(true);
}
int TLData::GetTrackNr(TLTrack *track)
{
	return m_trackList->IndexOf(track);
}
