/* MiniFilerTest.cpp
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

#include <wx/splitter.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/dirctrl.h>
#include <wx/config.h>
#include <wx/dragimag.h>

#include "TLPanel.h"
#include "TLMiniFiler2.h"

#include "new1.xpm"
#include "open.xpm"
#include "save.xpm"
#include "saveas.xpm"
#include "wav_export.xpm"
#include "rewind.xpm"
#include "play.xpm"
#include "stop.xpm"
#include "colours2.xpm"
#include "dndfile.xpm"
#include "snap.xpm"
#include "ggseq_32.xpm"
enum
{
	ID_But = 1,
	ID_SaveAs,
	ID_Save,
	ID_Rewind,
	ID_Play,
	ID_Stop,
	ID_Load,
	ID_New,
	ID_WavExport,
	ID_FileList,
	ID_DirTree,
	ID_SetColours,
	ID_SetSnap,
	ID_A_Test
};

class TestFrame1: public wxFrame
{
	public:
		TestFrame1(const wxString& title, const wxPoint& pos, const wxSize& size);
		~TestFrame1();
		void OnBut(wxCommandEvent& event);
		void OnPlay(wxCommandEvent& event);
		void OnStop(wxCommandEvent& event);
		void OnRewind(wxCommandEvent& event);
		void OnDonePlaying(wxCommandEvent& event);
		void OnStartDrag(wxCommandEvent& event);
//		void OnActivate(wxCommandEvent& event);
//		void OnUpdateCaret(wxCommandEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnSaveAs(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnLoad(wxCommandEvent& event);
		void OnNew(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnWavExport(wxCommandEvent& event);
		void OnSetColours(wxCommandEvent& event);
		void OnSetSnap(wxCommandEvent& event);
		void OnA_Test(wxCommandEvent& event);
		void OnFLStartDrag(wxListEvent& event);
		void OnFLItemActivated(wxListEvent& event);
		void OnSelChanged(wxTreeEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
	private:
		void MakeToolBar();
		void MakeTlPanel(wxWindow *parent);
		void MakeMainWindow(wxWindow *parent);
		void RefreshWindowTitle();
		void Stop();
		FileList *m_fileList;
		wxGenericDirCtrl *m_DirTree;
		TLPanel *m_tp;
		wxToolBar *m_toolBar;
		wxTimer *m_Timer;
		bool m_DraggingFile;
		wxDragImage *m_dragImage;
		DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TestFrame1, wxFrame)
//	EVT_BUTTON(ID_But,TestFrame1::OnBut)
	EVT_TOOL(ID_Play,TestFrame1::OnPlay)
	EVT_TOOL(ID_Rewind,TestFrame1::OnRewind)
	EVT_TOOL(ID_Stop,TestFrame1::OnStop)
	EVT_TOOL(ID_SaveAs,TestFrame1::OnSaveAs)
	EVT_TOOL(ID_Save,TestFrame1::OnSave)
	EVT_TOOL(ID_Load,TestFrame1::OnLoad)
	EVT_TOOL(ID_New,TestFrame1::OnNew)
	EVT_TOOL(ID_WavExport,TestFrame1::OnWavExport)
	EVT_TOOL(ID_SetColours,TestFrame1::OnSetColours)
	EVT_TOOL(ID_SetSnap,TestFrame1::OnSetSnap)
//	EVT_DONE_PLAYING_COMMAND(-1, TestFrame1::OnDonePlaying)
//	EVT_UPDATE_CARET_COMMAND(-1, TestFrame1::OnUpdateCaret)
	
/*	EVT_MF_STARTDRAG_COMMAND(-1, TestFrame1::OnStartDrag)*/
//	EVT_MF_ACTIVATE_COMMAND(-1, TestFrame1::OnActivate)
	EVT_LIST_BEGIN_DRAG(ID_FileList,TestFrame1::OnFLStartDrag)
	EVT_LIST_ITEM_ACTIVATED(ID_FileList,TestFrame1::OnFLItemActivated)	
	EVT_TREE_SEL_CHANGED(-1,TestFrame1::OnSelChanged)
	
	EVT_MENU(ID_A_Test, TestFrame1::OnA_Test)
	
	EVT_TIMER(-1,TestFrame1::OnTimer)
	EVT_CLOSE(TestFrame1::OnClose)
	
	EVT_MOTION(TestFrame1::OnMouseMotion)
	EVT_LEFT_UP(TestFrame1::OnMouseLeftUp)
END_EVENT_TABLE()

TestFrame1::TestFrame1(const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	MakeToolBar();
	wxPanel *panel1=new wxPanel(this); /*Für richtige Hintergrundfarbe in osx und win32*/
	wxBoxSizer *panelSizer = new wxBoxSizer( wxHORIZONTAL );

	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_NORMAL,  WXK_SPACE/*(int) 'n'*/,     ID_A_Test);
	wxAcceleratorTable accel(1, entries);
	panel1->SetAcceleratorTable(accel);
	panel1->SetFocus();

	MakeMainWindow(panel1);

	panelSizer->Add(panel1,1,wxEXPAND);
	SetSizer( panelSizer );
	panelSizer->SetSizeHints(this);

	m_Timer = new wxTimer(this);
	RefreshWindowTitle();
	m_DraggingFile=false;
	m_dragImage = new wxDragImage(wxBitmap(dndfile_xpm));
	CreateStatusBar();
	SetSize(size);
}
TestFrame1::~TestFrame1()
{
	wxConfig config(wxT("ggseq"));
	config.Write(wxT("MiniFilerDirectory"),m_DirTree->GetPath());
	delete m_dragImage;
}
void TestFrame1::OnA_Test(wxCommandEvent& event)
{
	puts("A_Test");
}
void TestFrame1::MakeMainWindow(wxWindow *parent)
{
	wxSplitterWindow* SplitView = new wxSplitterWindow(parent,-1,wxDefaultPosition,wxDefaultSize,wxSP_LIVE_UPDATE|wxSP_NOBORDER/*|wxSP_3DSASH| wxSP_FULLSASH*/);
	SplitView->SetMinimumPaneSize(20);
	wxBoxSizer *topsizer = new wxBoxSizer( wxHORIZONTAL );
	topsizer->Add(SplitView,1,wxEXPAND|wxALL,5);
	parent->SetSizer( topsizer );
	parent->SetClientSize(wxSize(300,400));
//	MiniFiler *mf=new MiniFiler(SplitView);
/*Hier kommt der neue Minifiler rein.*/

	wxSplitterWindow* SplitView2 = new wxSplitterWindow(SplitView,-1,wxDefaultPosition,wxDefaultSize,wxSP_LIVE_UPDATE|wxSP_NOBORDER/*|wxSP_3DSASH| wxSP_FULLSASH*/);
//	MakeTlPanel(SplitView2);
	SplitView2->SetMinimumPaneSize(20);
	m_tp = new TLPanel(SplitView2);

	m_fileList = new FileList(SplitView2,ID_FileList);
	m_DirTree = NULL;
	m_DirTree = new wxGenericDirCtrl(SplitView,ID_DirTree,wxDirDialogDefaultFolderStr,wxDefaultPosition,wxDefaultSize,wxDIRCTRL_DIR_ONLY|wxSUNKEN_BORDER);
	wxConfig config(wxT("ggseq"));
	m_DirTree->SetPath(config.Read(wxT("MiniFilerDirectory"), wxT("/")));

	SplitView->SplitVertically(m_DirTree,SplitView2);
	SplitView2->SplitHorizontally(m_tp,m_fileList);
	SplitView->SetSashPosition(200);
	SplitView2->SetSashPosition(270);
	
}
void TestFrame1::OnFLStartDrag(wxListEvent& event)
{
/*	DnDData Data(m_fileList->GetSelection());
	DnDContainer container;
	container.m_class = &Data;
	AudioDataObject my_data;
	my_data.SetData(0,(void*)(&container));
	wxDropSource dragSource(this);
	dragSource.SetData(my_data);
	dragSource.DoDragDrop(true);*/

/*	wxTextDataObject data;
	data.SetText(m_fileList->GetSelection());
	wxDropSource dragSource( this );
	dragSource.SetData( data );
	dragSource.DoDragDrop( TRUE );*/

/*	m_DragWindow->SetFilename(m_fileList->GetSelection());
	wxPoint pos=m_fileList->ClientToScreen(event.GetPoint());
	m_DragWindow->Move(pos);
	m_DragWindow->Show();
	m_DragWindow->CaptureMouse();*/
	
	m_dragImage->BeginDrag(wxPoint(0,0),this);
	m_dragImage->Move(ScreenToClient(m_fileList->ClientToScreen(event.GetPoint())));
	m_dragImage->Show();
	m_DraggingFile=true;

}
void TestFrame1::OnMouseLeftUp(wxMouseEvent& event)/*DragImage Ende*/
{
	if (!m_DraggingFile)
		return;
	m_DraggingFile=false;
	m_dragImage->Hide();
	m_dragImage->EndDrag();
	int x_=event.m_x;
	int y_=event.m_y;
	ClientToScreen(&x_,&y_);
	m_tp->DropFileAt(x_,y_,m_fileList->GetSelection());
}
void TestFrame1::OnMouseMotion(wxMouseEvent& event)/*DragImage Move*/
{
	if (!m_DraggingFile)
		return;
	m_dragImage->Move(wxPoint(event.m_x,event.m_y));
}
void TestFrame1::OnFLItemActivated(wxListEvent& event)
{
	wxLogStatus(wxT("Playing Sample: %s"),m_fileList->GetSelection().c_str());
	m_tp->PlaySample(m_fileList->GetSelection());
}
void TestFrame1::OnSelChanged(wxTreeEvent& event)
{
	if (!m_DirTree)
		return;
	m_fileList->SetDirectory(m_DirTree->GetPath());
}

void TestFrame1::MakeToolBar()
{
	m_toolBar = new wxToolBar(this,-1,wxDefaultPosition,wxDefaultSize,wxTB_HORIZONTAL |/* wxNO_BORDER|*/wxTB_NODIVIDER|wxTB_FLAT  );
	m_toolBar->AddTool(ID_New,wxT("New"),wxBitmap(new1_xpm),wxT("New"));
	m_toolBar->AddTool(ID_Load,wxT("Open..."),wxBitmap(open_xpm),wxT("Open..."));
	m_toolBar->AddTool(ID_Save,wxT("Save"),wxBitmap(save_xpm),wxT("Save"));
	m_toolBar->AddTool(ID_SaveAs,wxT("Save as..."),wxBitmap(saveas_xpm),wxT("Save as..."));
	m_toolBar->AddTool(ID_WavExport,wxT("Export WAV..."),wxBitmap(wav_export_xpm),wxT("Export WAV..."));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool(ID_Rewind,wxT("Rewind"),wxBitmap(rewind_xpm),wxT("Rewind"));
	m_toolBar->AddTool(ID_Play,wxT("Play"),wxBitmap(play_xpm),wxT("Play"));
	m_toolBar->AddTool(ID_Stop,wxT("Stop"),wxBitmap(stop_xpm),wxT("Stop"));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool(ID_SetColours,wxT("Setup Colours..."),wxBitmap(colours2_xpm),wxT("Setup Colours..."));
	m_toolBar->AddTool(ID_SetSnap,wxT("Set Snap Width"),wxBitmap(snap_xpm),wxT("Set Snap Width"));
	m_toolBar->Realize();
	m_toolBar->EnableTool(ID_Stop,false);
	this->SetToolBar(m_toolBar);
}
void TestFrame1::MakeTlPanel(wxWindow *parent)
{
/*	m_TlData = new TLData();
	
	m_TlData->AddTrack();
	m_TlData->AddTrack();
	m_TlData->AddTrack();
	m_TlData->AddTrack();
	m_TlData->AddTrack();
	m_TlData->AddTrack();
	m_TlData->AddTrack();
	m_TlData->AddTrack();

	TLView *tv = new TLView(m_TlData);
	tv->SetVisibleFrame(100,100);
	tv->SetVisibleLength(1420000);
	tv->SetPosition(0);*/
	m_tp = new TLPanel(parent,-1,wxPoint(0,50),wxSize(150,150));
}
void TestFrame1::OnBut(wxCommandEvent& event)
{
}
void TestFrame1::OnPlay(wxCommandEvent& event)
{
	m_tp->Play();
	m_toolBar->EnableTool(ID_Play,false);
	m_toolBar->EnableTool(ID_Stop,true);
	m_Timer->Start(100);
}

void TestFrame1::OnStop(wxCommandEvent& event)
{
	Stop();
	m_tp->Stop();
}
void TestFrame1::OnRewind(wxCommandEvent& event)
{
	m_tp->Rewind();
//	m_tp->HideCaret();
}
void TestFrame1::OnDonePlaying(wxCommandEvent& event)
{
	Stop();
	m_tp->Stop();
}
void TestFrame1::OnStartDrag(wxCommandEvent& event)
{
/*	DnDData Data(event.GetString());
	DnDContainer container;
	container.m_class = &Data;
	AudioDataObject my_data;
	my_data.SetData(0,(void*)(&container));
	wxDropSource dragSource(this);
	dragSource.SetData(my_data);
	dragSource.DoDragDrop(true);*/
}
/*void TestFrame1::OnActivate(wxCommandEvent& event)
{
	wxLogStatus(wxT("Playing Sample: %s"),event.GetString().c_str());
	m_tp->PlaySample(event.GetString());
}*/
/*void TestFrame1::OnUpdateCaret(wxCommandEvent& event)
{
	m_tp->SetCaretPosition(event.GetInt());
}*/

void TestFrame1::OnTimer(wxTimerEvent& event)
{
	if (!m_tp->UpdateCaret()) {
		Stop();
	}
}

void TestFrame1::OnSaveAs(wxCommandEvent& event)
{
	m_tp->SaveAs();
	RefreshWindowTitle();
}
void TestFrame1::OnLoad(wxCommandEvent& event)
{
	Stop();
	m_tp->Load();
	RefreshWindowTitle();
}
void TestFrame1::OnSave(wxCommandEvent& event)
{
	m_tp->Save();
	RefreshWindowTitle();
}
void TestFrame1::OnClose(wxCloseEvent& event)
{
	if (!m_tp->SaveOnExit()) {
		event.Veto();
	} else {
		m_Timer->Stop();
		m_tp->StopAll();
		this->Destroy();
	}
}
void TestFrame1::OnNew(wxCommandEvent& event)
{
	Stop();
	m_tp->New();
	RefreshWindowTitle();
}
void TestFrame1::OnWavExport(wxCommandEvent& event)
{
	Stop();
	m_tp->WavExport();
}

void TestFrame1::RefreshWindowTitle()
{
	wxFileName fn(m_tp->GetFilename());
	
	SetTitle(fn.GetFullName() + wxT(" - Gungirl Sequencer"));		
}
void TestFrame1::OnSetColours(wxCommandEvent& event)
{
	m_tp->SetColours(m_DirTree->GetPath());
}
void TestFrame1::OnSetSnap(wxCommandEvent& event)
{
	m_tp->SetSnap();
}
void TestFrame1::Stop()
{
	m_Timer->Stop();
	m_toolBar->EnableTool(ID_Play,true);
	m_toolBar->EnableTool(ID_Stop,false);
}

class Test: public wxApp
{
		virtual bool OnInit();
};

IMPLEMENT_APP(Test)

bool Test::OnInit()

{
	SetAppName(wxT("ggseq"));
	TestFrame1 *frame = new TestFrame1( wxT("Test"), wxPoint(50,50), wxSize(600,400));
	frame->SetIcon(wxICON(ggseq_32));
	frame->Show( TRUE );
	SetTopWindow( frame );
	return TRUE;
}
