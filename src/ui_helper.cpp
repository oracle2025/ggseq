/* ui_helper.cpp
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

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dirctrl.h>
#include <wx/listctrl.h>

#include "stuff.h"
#include "ui_helper.h"
#include "TLMiniFiler2.h"
#include "MiniPlayer.h"
#include "TLPanel.h"
#include "BigScrollBar.h"
#include "FileInfoPanel.h"
#include "SidePanel.h"
#include "Ruler.h"


wxWindow *MakeTlPanel( wxWindow* parent, wxWindowID id )
{
	wxPanel *panel1 = new wxPanel(parent,-1,
		wxDefaultPosition,
		wxDefaultSize,
		wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);

	wxBoxSizer * tlPanelSizer = new wxBoxSizer(wxVERTICAL);
	BigScrollBar *sb =  new BigScrollBar(panel1,ID_BIG_SCROLLBAR);
	Ruler *ruler = new Ruler(panel1,-1,wxDefaultPosition,wxSize(10,15),wxRAISED_BORDER);
	wxScrollBar *sb2 = new wxScrollBar(panel1, ID_SCROLLBAR2, wxDefaultPosition, wxDefaultSize, wxVERTICAL);
	sb2->SetScrollbar(0,16,50,15);
	TLPanel *tp = new TLPanel(panel1, sb,ruler,sb2, id);
	tp->Fit();
	wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(tp,1,wxEXPAND);
	sizer2->Add(sb2, 0, wxEXPAND);

	wxBoxSizer *sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(sb, 1, wxEXPAND);
	//wxSystemSettings::GetMetric(wxSYS_HTHUMB_X)
	sizer3->Add(17, 0);


	tlPanelSizer->Add(ruler,0,wxEXPAND);
	tlPanelSizer->Add(sizer2,1,wxEXPAND);
	tlPanelSizer->Add(sizer3,0,wxEXPAND);
	panel1->SetSizer(tlPanelSizer);
	tlPanelSizer->SetSizeHints(panel1);
	panel1->Fit();
	return panel1;
}
wxWindow *MakeSidePanel( wxWindow* parent, wxWindowID id , wxBoxSizer *sizer )
{
	FileInfoPanel *fp = new FileInfoPanel(parent, id);
	SidePanel* sm1= new SidePanel(parent, fp, sizer );
	return sm1;
}
wxWindow *MakeFileList( wxWindow* parent, wxWindowID id )
{
	return new FileList(parent,id);
}
wxWindow *MakeDirTree( wxWindow* parent, wxWindowID id )
{
	wxGenericDirCtrl *dirTree = new wxGenericDirCtrl(parent,
		id,
		wxDirDialogDefaultFolderStr,
		wxDefaultPosition,
		wxDefaultSize,
		wxDIRCTRL_DIR_ONLY|wxNO_FULL_REPAINT_ON_RESIZE|wxSUNKEN_BORDER);
	return dirTree;
}
wxWindow *MakeMiniPlayer( wxWindow* parent, wxWindowID id )
{
	return new MiniPlayer(parent, id);
//	return new wxPanel(parent, id);
}
                                                                                                   

