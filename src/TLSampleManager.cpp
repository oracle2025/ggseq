/* TLSampleManager.cpp
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
#include <wx/filename.h>
/*XML*/
#include "tinyxml.h"

#include "stuff.h"
#include "TLSampleManager.h"
#include "TLSample.h"
#include "TLColourManager.h"
#include "UpdateListener.h"

WX_DEFINE_LIST(TLSampleList);

TLSampleManager::TLSampleManager()
{
	m_MaxId=0;
	m_colourMan=new TLColourManager();
	m_sampleList.DeleteContents(true);
}

TLSampleManager::~TLSampleManager()
{
	m_sampleList.Clear();
	delete m_colourMan;
}

TLSample *TLSampleManager::GetSample(wxString filename, UpdateListener* updateListener)
{
	wxString fnStr=NormalizePath(filename);
	for ( TLSampleList::Node *node = m_sampleList.GetFirst(); node; node = node->GetNext() ) {
		TLSample *current = node->GetData();
		if (fnStr==current->GetFilename())
			return current;
	}
	return AddSample(fnStr, m_MaxId, updateListener);
}

TLSample *TLSampleManager::GetSample(int id)
{
	for ( TLSampleList::Node *node = m_sampleList.GetFirst(); node; node = node->GetNext() ) {
		TLSample *current = node->GetData();
		if (id==current->GetId())
			return current;
	}
	return NULL;
}

TLSample *TLSampleManager::AddSample(wxString filename, int id, UpdateListener* updateListener)
{
	wxString fnStr=NormalizePath(filename);
	TLSample *tmp = new TLSample(fnStr, id,m_colourMan, updateListener);
	if (tmp->IsValid()) {
		m_sampleList.Append(tmp);
		if (id>=m_MaxId)
			m_MaxId=id+1;
	} else {
		if (updateListener->Update(100)!=false)
			wxLogError(wxT("Couldn't load Samplefile \"%s\""),filename.c_str());
		delete tmp;
		tmp=NULL;
	}
	return tmp;
}

void TLSampleManager::Clear(TLSample* tlSample)
{
	if (tlSample->GetRefCount())
		return;
	m_sampleList.DeleteObject(tlSample);
}
wxString TLSampleManager::NormalizePath(wxString filename)
{

	wxFileName fn(filename);
	fn.Normalize();
	return fn.GetFullPath();
}
void TLSampleManager::addXmlData(TiXmlElement *samples)
{
//	wxString count;
//	count << m_sampleList.GetCount();
	samples->SetAttribute("count",m_sampleList.GetCount());
	for ( TLSampleList::Node *node = m_sampleList.GetFirst(); node; node = node->GetNext() )
	{
		TLSample *current = node->GetData();
//		wxString tmp;
//		tmp << current->GetId();
		TiXmlElement *sample = new TiXmlElement("sample");
		samples->LinkEndChild(sample);
		sample->SetAttribute("id",current->GetId()/*(const char*)tmp.mb_str()*/);
		sample->LinkEndChild(new TiXmlText(current->GetFilename().mb_str()));
	}

}
void TLSampleManager::ClearAll()
{
//	m_sampleList.DeleteContents(true);
	m_sampleList.Clear();	
}
TLColourManager *TLSampleManager::GetColourManager()
{
	return m_colourMan;
}
