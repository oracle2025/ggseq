/* TLMiniFiler2.cpp
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/listctrl.h>
#include <wx/dir.h>

#include "TLMiniFiler2.h"

//DEFINE_EVENT_TYPE(wxEVT_MF_ACTIVATE_COMMAND)
FileList::FileList(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	:wxListCtrl(parent,id,pos,size,style|wxSUNKEN_BORDER)
{}
void FileList::SetDirectory(const wxString& dirname)
{
	m_dirname=dirname;
	DeleteAllItems();
	wxDir dir(dirname);
	if ( !dir.IsOpened() )
		return;
	wxString filename;
	bool cont = dir.GetFirst(&filename,wxEmptyString, wxDIR_FILES);
	while ( cont ) {
		InsertItem(0,filename);
		cont = dir.GetNext(&filename);
	}
}
wxString FileList::GetSelection()
{
	long sel=GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
	if (sel >=0)
		return m_dirname+wxT("/")+GetItemText(sel);
	else
		return wxT("");
}
