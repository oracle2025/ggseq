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

#include "TLXmlLoader2.h"
#include "TLData.h"
#include "TLSampleManager.h"
#include "tinyxml.h"

TLXMLLoader2::TLXMLLoader2(TLData *data, TLSampleManager *sm)
{
	m_data=data;
	m_sampleManager=sm;
}

void TLXMLLoader2::LoadFile(wxString filename)
{
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
	TiXmlNode *node = element->FirstChild("samples");
	if (node==NULL) {
		Error(filename);
		return;
	}
	node = node->FirstChild("sample");
	if (node==NULL) {
		Error(filename);
		return;
	}
	element = node->ToElement();
	
	while (element) {
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


		m_sampleManager->AddSample(tt,id); /*TODO abbrechen einbauen*/
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
	node = node->FirstChild("track");
	if (node==NULL) {
		Error(filename);
		return;
	}
	element = node->ToElement();
	int trackNr = 0;
	while (element) {
		int id;
		int pos;
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
				if (element_item->Attribute("pos",&pos)==NULL) {
					Error(filename);
					return;
				}
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
	wxLogError(wxT("Konnte Datei \"%s\" nicht laden"),msg.c_str());
	m_data->Clear();
}
