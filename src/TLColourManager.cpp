/* TLColourManager.cpp
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

#include <wx/filename.h>
#include <wx/config.h>

#include "TLColourManager.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(TLDirColourList);

TLDirColour::TLDirColour(wxString dir, wxColour colour)
{
	m_directory=dir;
	m_colour=colour;
}
TLColourManager::TLColourManager()
{
	m_colourList.DeleteContents(true);
	TLDirColour *tdc1;
	wxConfig config(wxT("ggseq"));
	config.SetPath(wxT("/Colours"));
	wxArrayString aNames;
	wxString str;
	long dummy;
	bool bCont = config.GetFirstEntry(str, dummy);
	while ( bCont ) {
		aNames.Add(str);
		bCont = config.GetNextEntry(str, dummy);
	}
	for (unsigned int i=0;i<aNames.Count();i++) {
		wxString str1=aNames.Item(i);
		long d = config.Read(str1,-1);
		str1.Replace(wxT("\\"),wxT("/"));
		if (d<0)
			tdc1=new TLDirColour(str1,*wxGREEN);
		else
			tdc1=new TLDirColour(str1,wxColour(d));
		m_colourList.Append(tdc1);
	}
}
TLColourManager::~TLColourManager()
{
	wxConfig config(wxT("ggseq"));
	if (config.Exists(wxT("/Colours"))) {
		config.DeleteGroup(wxT("/Colours"));
	}
	config.Flush();
	config.SetPath(wxT("/Colours"));
	for ( TLDirColourList::Node *node = m_colourList.GetFirst(); node; node = node->GetNext() ) {
		TLDirColour *current = node->GetData();
		unsigned long var = current->m_colour.Blue();
		var=var*256;
		var +=current->m_colour.Green();
		var=var*256;
		var +=current->m_colour.Red();
		wxString str = current->m_directory;
		str.Replace(wxT("/"),wxT("\\"));
		config.Write(str,(long)var);
	}
	m_colourList.Clear();
}
wxColour TLColourManager::GetColour(wxString dirname)
{
	wxString str1;
	for ( TLDirColourList::Node *node = m_colourList.GetFirst(); node; node = node->GetNext() ) {
		TLDirColour *current = node->GetData();
		str1=dirname.Mid(0,current->m_directory.Len());
		if (str1==current->m_directory)
			return current->m_colour;
	}
	return *wxRED;
}
void TLColourManager::SetColour(wxString dirname, wxColour colour)
{
	TLDirColour *tdc1=new TLDirColour(dirname,colour);
	m_colourList.Append(tdc1);
}
void TLColourManager::Clear()
{
	m_colourList.Clear();
}
TLDirColourList::Node *TLColourManager::GetColours()
{
	return m_colourList.GetFirst();
}
