/* TLXMLLoader2.cpp
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
#include "wx/encconv.h"
#include "stuff.h"

#include "TLData.h"
#include "TLSampleManager.h"
#include "tinyxml.h"
#include "UpdateListener.h"

#include "TLXmlLoader2.h"

TLXMLLoader2::TLXMLLoader2(TLData *data, TLSampleManager *sm)
{
	m_data=data;
	m_sampleManager=sm;
}

void TLXMLLoader2::LoadFile(wxString filename, UpdateListener* updateListener)
{
	wxString version_string = wxT("0.0");
	TiXmlDocument doc(filename.mb_str());
	if(!doc.LoadFile()) {
		wxLogError(wxT("Konnte Datei \"%s\" nicht laden"),filename.c_str());
		m_data->Clear();
		return;
	}
	TiXmlElement *element = doc.RootElement();
	if (element==NULL) {
		Error(filename);
		return;
	}
	int snap=1;
	if (element->Attribute("snap",&snap)) {
		m_data->SetSnapValue(snap);
	}
	const char *ver;
	ver=element->Attribute("version");
	if (ver) {
/*-*/
		wchar_t out[strlen(ver)];
		wxEncodingConverter conv;
		conv.Init(wxFONTENCODING_ISO8859_2,wxFONTENCODING_UNICODE);
		conv.Convert(ver,out);
		wxString tt; 
		tt << out;
/*-*/
		version_string=tt;
		if (version_string!=wxT("0.1") && version_string!=wxT("0.1.1") && version_string!=wxT(GG_VERSION)) {
			wxLogError(wxT("Couldn't load file \"%s\"\nSaved with wrong Program Version"),filename.c_str());
			m_data->Clear();
			return;
		}
	}
	TiXmlNode *node = element->FirstChild("samples");
	if (node==NULL) {
		Error(filename);
		return;
	}

	int sampleNum=1;
	if (node->ToElement()->Attribute("count",&sampleNum)==NULL) {
		/**/
		if (version_string!=wxT("0.0")) {
			Error(filename);
			return;
		} else {
			sampleNum=1;
		}
	}
	
	node = node->FirstChild("sample");
	if (node==NULL) {
		//Error(filename);
		return;
	}
	element = node->ToElement();
	
	for (int i=0; element; i++) {
		if (updateListener)
			if(updateListener->Update((i*100)/sampleNum)==false)
				return;
		int id;
		if (element->Attribute("id",&id)==NULL) {
			Error(filename);
			return;
		}
		node = element->FirstChild();
		if (node==NULL) {
			Error(filename);
			return;
		}
		TiXmlText *text = node->ToText();
		if (text==NULL) {
			Error(filename);
			return;
		}
/*-*/
	wchar_t out[strlen(text->Value())];
	wxEncodingConverter conv;
	conv.Init(wxFONTENCODING_ISO8859_2,wxFONTENCODING_UNICODE);
	conv.Convert(text->Value(),out);
	wxString tt; 
	tt << out;
/*-*/
		RecursiveUpdateListener s(updateListener, (i*100)/sampleNum, ((i+1)*100)/sampleNum);
		m_sampleManager->AddSample(tt,id,&s); /*TODO abbrechen einbauen*/
		element=element->NextSiblingElement("sample");
	}
	element = doc.RootElement();
	if (element==NULL) {
		Error(filename);
		return;
	}
	node = element->FirstChild("tracks");
	if (node==NULL) {
		Error(filename);
		return;
	}
	int trackCnt=1;
	if (node->ToElement()->Attribute("count",&trackCnt)==NULL) {
		Error(filename);
		return;
	}
	while(m_data->GetTrackCount()>trackCnt) {
		m_data->DeleteTrack(0);
	}
	while(m_data->GetTrackCount()<trackCnt) {
		m_data->AddTrack(-1);
	}
	node = node->FirstChild("track");
	if (node==NULL) {
		Error(filename);
		return;
	}
	element = node->ToElement();
	int trackNr = 0;
	while (element) {
		int id;
//		int pos;
		char buffer[100];
		int mute;
		int volume;
		/*Volume und Mute einlesen*/
		if (element->Attribute("mute",&mute)!=NULL) {
			m_data->SetTrackMute(mute,trackNr);
		}
		if (element->Attribute("volume",&volume)!=NULL) {
			m_data->SetTrackVolume(((double)volume)/100.0,trackNr);
		}

		
		node = element->FirstChild("item");
		if (node!=NULL) {
			TiXmlElement *element_item = node->ToElement();
			while (element_item) {
				if (element_item->Attribute("sample",&id)==NULL) {
					Error(filename);
					return;
				}
				strncpy(buffer,element_item->Attribute("pos"),sizeof(buffer));
				if (buffer==NULL) {
					Error(filename);
					return;
				}
				gg_tl_dat pos = strtoll(buffer,NULL,10);
				TLSample *sample=m_sampleManager->GetSample(id);
				if (sample)
					m_data->AddItem(sample,pos,trackNr);
				element_item=element_item->NextSiblingElement("item");
			}
		}
		trackNr++;
		element=element->NextSiblingElement("track");
	}
}
void TLXMLLoader2::Error(wxString msg)
{
//	wxLogError(msg);
	wxLogError(wxT("Couldn't load file \"%s\""),msg.c_str());
	m_data->Clear();
}
