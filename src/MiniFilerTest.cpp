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
#include <wx/cmdline.h>
#include<wx/dnd.h>

#include "stuff.h"
#define _DISABLE_CONFIG_SHORTCUTS_

#include "TLPanel.h"
#include "TLMiniFiler2.h"
#include "MiniPlayer.h"
#include "UpdateListener.h"
#include "StatusProgressBar.h"
#include "DisableListener.h"
#include "SidePanel.h"
#include "FileInfoPanel.h"

#ifndef _DISABLE_CONFIG_SHORTCUTS_
	#include "ShortcutsDialog.h"
#endif

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
//#include "prefs_misc.xpm"


enum
{
	ID_But = 1,
#if 0
	ID_SaveAs, /*wxID_SAVEAS*/
	ID_Save, /*wxID_SAVE*/
	ID_Rewind, /*ID_REWIND*/
	ID_Play, /*ID_PLAY*/
	ID_Stop, /*ID_STOP*/
	ID_Load, /*wxID_OPEN*/
	ID_New, /*wxID_NEW*/
	ID_Exit,  /*wxID_EXIT*/
#endif
	ID_WavExport,
	ID_FileList,
	ID_DirTree,
	ID_SetColours,
	ID_SetSnap,
	ID_A_Test,
	ID_ScrollBar,
	ID_Preferences
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
		void OnPreferences(wxCommandEvent& event);
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
		void OnFLItemSelected(wxListEvent& event);
		void OnSelChanged(wxTreeEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);
		void OnMouseMotion(wxMouseEvent& event);
		void OnScroll(wxScrollEvent& event);
		void LoadFile(wxString& filename);
	private:
		void MakeToolBar();
		void MakeTlPanel(wxWindow *parent);
		void MakeMainWindow(wxWindow *parent);
		void MakeShortcuts();
		void RefreshWindowTitle();
		void Stop();
		FileList *m_fileList;
		wxGenericDirCtrl *m_DirTree;
		TLPanel *m_tp;
		wxToolBar *m_toolBar;
		wxTimer *m_Timer;
		MiniPlayerInterface *m_miniPlayer;
		UpdateListener *m_updateListener;
		bool m_DraggingFile;
		wxDragImage *m_dragImage;
		DECLARE_EVENT_TABLE()
};

class DropFiles: public wxFileDropTarget
{
	public:
		DropFiles (TestFrame1 *frame) {m_frame = frame;}
		virtual bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames);
	private:
		TestFrame1 *m_frame;
};

BEGIN_EVENT_TABLE(TestFrame1, wxFrame)
	EVT_TOOL(ID_PLAY,TestFrame1::OnPlay)
	EVT_TOOL(ID_REWIND,TestFrame1::OnRewind)
	EVT_TOOL(ID_STOP,TestFrame1::OnStop)
	EVT_TOOL(wxID_SAVEAS,TestFrame1::OnSaveAs)
	EVT_TOOL(wxID_SAVE,TestFrame1::OnSave)
	EVT_TOOL(wxID_OPEN,TestFrame1::OnLoad)
	EVT_TOOL(wxID_NEW,TestFrame1::OnNew)
	EVT_TOOL(ID_WavExport,TestFrame1::OnWavExport)
	EVT_TOOL(ID_SetColours,TestFrame1::OnSetColours)
	EVT_TOOL(ID_SetSnap,TestFrame1::OnSetSnap)
	EVT_TOOL(ID_Preferences,TestFrame1::OnPreferences)

	EVT_MENU(wxID_OPEN, TestFrame1::OnLoad)
	EVT_MENU(wxID_SAVE, TestFrame1::OnSave)
	EVT_MENU(wxID_SAVEAS, TestFrame1::OnSaveAs)
	EVT_MENU(wxID_NEW, TestFrame1::OnNew)
	EVT_MENU(ID_PLAY, TestFrame1::OnPlay)
	EVT_MENU(ID_REWIND, TestFrame1::OnRewind)
	EVT_MENU(ID_STOP, TestFrame1::OnStop)

	EVT_LIST_BEGIN_DRAG(ID_FileList,TestFrame1::OnFLStartDrag)
	EVT_LIST_ITEM_ACTIVATED(ID_FileList,TestFrame1::OnFLItemActivated)	
	EVT_LIST_ITEM_SELECTED(ID_FileList,TestFrame1::OnFLItemSelected)
	EVT_TREE_SEL_CHANGED(-1,TestFrame1::OnSelChanged)
	EVT_COMMAND_SCROLL_THUMBTRACK(ID_ScrollBar,TestFrame1::OnScroll)
	
	EVT_TIMER(-1,TestFrame1::OnTimer)
	EVT_CLOSE(TestFrame1::OnClose)
	
	EVT_MOTION(TestFrame1::OnMouseMotion)
	EVT_LEFT_UP(TestFrame1::OnMouseLeftUp)
END_EVENT_TABLE()

TestFrame1::TestFrame1(const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxFrame((wxFrame *)NULL, -1, title, pos, size,wxDEFAULT_FRAME_STYLE|GG_WINDOW_FLAGS)
{
	MakeToolBar();
	wxPanel *panel1=new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL|wxNO_FULL_REPAINT_ON_RESIZE); /*Für richtige Hintergrundfarbe in osx und win32*/
	wxBoxSizer *panelSizer = new wxBoxSizer( wxHORIZONTAL );

//	wxAcceleratorEntry entries[2];
//	entries[0].Set(wxACCEL_NORMAL,  WXK_SPACE/*(int) 'n'*/,     ID_A_Test);
//	entries[1].Set(wxACCEL_CTRL,  (int) 'o', wxID_OPEN);
//	wxAcceleratorTable accel(2, entries);
//	panel1->SetAcceleratorTable(accel);

	panel1->SetFocus();
//	panel1->Enable(false);

	StatusProgressBar *cc = new StatusProgressBar(this,-1);
	SetStatusBar(cc);
	m_updateListener=cc;

	cc->SetDisableListener(new DoubleDisabler(panel1,GetToolBar()));

	MakeMainWindow(panel1);
	
	m_tp->SetUpdateListener(cc);

	panelSizer->Add(panel1,1,wxEXPAND);
	SetSizer( panelSizer );
	panelSizer->SetSizeHints(this);
	SetSizeHints(470,500);

	m_Timer = new wxTimer(this);
	RefreshWindowTitle();
	m_DraggingFile=false;
	m_dragImage = new wxDragImage(wxBitmap(dndfile_xpm));

	MakeShortcuts();

	SetSize(size);
	SetDropTarget (new DropFiles (this));
}
void TestFrame1::MakeShortcuts()
{
	wxAcceleratorEntry entries[8];
	entries[0].Set(wxACCEL_NORMAL,  WXK_SPACE, ID_PLAY);
	entries[1].Set(wxACCEL_NORMAL, 'S' , ID_STOP);
	entries[2].Set(wxACCEL_NORMAL,'R', ID_REWIND);
	entries[3].Set(wxACCEL_CTRL,'N', wxID_NEW);
	entries[4].Set(wxACCEL_CTRL,'O', wxID_OPEN);
	entries[5].Set(wxACCEL_CTRL,'S', wxID_SAVE);
	entries[6].Set(wxACCEL_CTRL|wxACCEL_SHIFT,'S', wxID_SAVEAS);
	entries[7].Set(wxACCEL_CTRL,'Q', wxID_EXIT);
	wxAcceleratorTable accel(8, entries);
	SetAcceleratorTable(accel);
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
	wxSplitterWindow* SplitView = new wxSplitterWindow(parent,-1,wxDefaultPosition,wxDefaultSize,wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxNO_FULL_REPAINT_ON_RESIZE/*|wxSP_3DSASH| wxSP_FULLSASH*/);
	SplitView->SetMinimumPaneSize(20);
	wxBoxSizer *topsizer = new wxBoxSizer( wxHORIZONTAL );
	topsizer->Add(SplitView,1,wxEXPAND|wxALL,5);
	parent->SetSizer( topsizer );
	parent->SetClientSize(wxSize(300,400));
//	MiniFiler *mf=new MiniFiler(SplitView);
/*Hier kommt der neue Minifiler rein.*/

	wxPanel* SplitView2 = new wxPanel(SplitView);
	wxBoxSizer * SplitV2Sizer = new wxBoxSizer(wxVERTICAL);

//	wxSplitterWindow* SplitView2 = new wxSplitterWindow(SplitView,-1,wxDefaultPosition,wxDefaultSize,wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxNO_FULL_REPAINT_ON_RESIZE/*|wxSP_3DSASH| wxSP_FULLSASH*/);
//	MakeTlPanel(SplitView2);
//	SplitView2->SetMinimumPaneSize(20);

	wxPanel *panel1 = new wxPanel(SplitView2,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE);

	wxBoxSizer * tlPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxScrollBar *sb = new wxScrollBar(panel1,ID_ScrollBar);
	m_tp = new TLPanel(panel1,sb);
	m_tp->Fit();
	tlPanelSizer->Add(m_tp,1,wxEXPAND);
	tlPanelSizer->Add(sb,0,wxEXPAND);
	panel1->SetSizer(tlPanelSizer);
	tlPanelSizer->SetSizeHints(panel1);
//	sb->SetSizeHints(-1,25,-1,25);
//	panel1->SetSizeHints(200,200);
//	panel1->SetSize(200,200);
	panel1->Fit();

	SplitV2Sizer->Add(panel1,0,wxBOTTOM|wxEXPAND,3);
	
//	m_tp = new TLPanel(SplitView2);/*TODO Lautstärke regler einbauen*/


	wxPanel *miniplayerPanel = new wxPanel(SplitView2,-1,wxDefaultPosition,wxDefaultSize,wxNO_FULL_REPAINT_ON_RESIZE);
	SplitV2Sizer->Add(miniplayerPanel,1,wxEXPAND);
	
	wxPanel *sidePanelHolder = new wxPanel(miniplayerPanel,-1,wxDefaultPosition,wxDefaultSize,wxNO_FULL_REPAINT_ON_RESIZE);

	
	
	m_fileList = new FileList(sidePanelHolder,ID_FileList);
	wxBoxSizer *bs2 = new wxBoxSizer(wxHORIZONTAL);
	bs2->Add(m_fileList,1,wxEXPAND);
	FileInfoPanel *fp = new FileInfoPanel(sidePanelHolder);
	SidePanel* sm1= new SidePanel(sidePanelHolder, fp);
	bs2->Add(sm1,0,wxEXPAND);
	
	sidePanelHolder->SetSizer(bs2);
	bs2->Fit(sidePanelHolder);
	bs2->SetSizeHints(sidePanelHolder);
	
	wxBoxSizer *miniPanSizer = new wxBoxSizer(wxVERTICAL);
	miniPanSizer->Add(sidePanelHolder,1,wxEXPAND);
//--
	MiniPlayer *mP = new MiniPlayer(miniplayerPanel,m_tp->GetSoundManager(),m_updateListener ,fp);
	miniPanSizer->Add(mP,0,wxEXPAND|wxTOP,5);
	m_miniPlayer = mP;
	m_tp->SetMiniPlayer(m_miniPlayer);
	
	miniplayerPanel->SetSizer(miniPanSizer);
	miniPanSizer->SetSizeHints(miniplayerPanel);
	
	
	m_DirTree = NULL;
	m_DirTree = new wxGenericDirCtrl(SplitView,ID_DirTree,wxDirDialogDefaultFolderStr,wxDefaultPosition,wxDefaultSize,wxDIRCTRL_DIR_ONLY|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE);
	wxConfig config(wxT("ggseq"));
	m_DirTree->SetPath(config.Read(wxT("MiniFilerDirectory"), wxT("/")));

	SplitView->SplitVertically(m_DirTree,SplitView2);
	SplitV2Sizer->Fit(SplitView2);
	SplitView2->SetSizer(SplitV2Sizer);
	
//	SplitView2->SplitHorizontally(/*m_tp*/panel1,miniplayerPanel);
	SplitView->SetSashPosition(200);
//	SplitView2->SetSashPosition(280);
	
}
void TestFrame1::OnScroll(wxScrollEvent& event)
{
	m_tp->OnScroll(event);
}
void TestFrame1::OnPreferences(wxCommandEvent& event)
{
#ifndef _DISABLE_CONFIG_SHORTCUTS_
	ShortcutsDialog dlg(this);
	dlg.Centre();
	if (dlg.ShowModal()==wxID_OK) {
		SetAcceleratorTable(dlg.GetAccelTable());
		dlg.Save();
		m_fileList->SetFocus();
	}
#endif
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
//	wxLogStatus(wxT("Playing Sample: %s"),m_fileList->GetSelection().c_str());
//	m_tp->PlaySample(m_fileList->GetSelection());
	m_miniPlayer->SetFilename(m_fileList->GetSelection());
	m_miniPlayer->Play();
}
void TestFrame1::OnFLItemSelected(wxListEvent& event)
{
	m_miniPlayer->SetFilename(m_fileList->GetSelection());
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
	m_toolBar->AddTool(wxID_NEW,wxT("New"),wxBitmap(new1_xpm),wxT("New"));
	m_toolBar->AddTool(wxID_OPEN,wxT("Open..."),wxBitmap(open_xpm),wxT("Open..."));
	m_toolBar->AddTool(wxID_SAVE,wxT("Save"),wxBitmap(save_xpm),wxT("Save"));
	m_toolBar->AddTool(wxID_SAVEAS,wxT("Save as..."),wxBitmap(saveas_xpm),wxT("Save as..."));
	m_toolBar->AddTool(ID_WavExport,wxT("Export WAV..."),wxBitmap(wav_export_xpm),wxT("Export WAV..."));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool(ID_REWIND,wxT("Rewind"),wxBitmap(rewind_xpm),wxT("Rewind"));
	m_toolBar->AddTool(ID_PLAY,wxT("Play"),wxBitmap(play_xpm),wxT("Play"));
	m_toolBar->AddTool(ID_STOP,wxT("Stop"),wxBitmap(stop_xpm),wxT("Stop"));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool(ID_SetColours,wxT("Setup Colours..."),wxBitmap(colours2_xpm),wxT("Setup Colours..."));
	m_toolBar->AddTool(ID_SetSnap,wxT("Set Snap Width"),wxBitmap(snap_xpm),wxT("Set Snap Width"));
//	m_toolBar->AddTool(ID_Preferences,wxT("Preferences"),wxBitmap(prefs_misc_xpm),wxT("Preferences"));
	m_toolBar->Realize();
	m_toolBar->EnableTool(ID_STOP,false);
	this->SetToolBar(m_toolBar);
//	m_toolBar->Enable(false);
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
//	m_tp = new TLPanel(parent,-1,wxPoint(0,50),wxSize(150,150));
}
void TestFrame1::OnBut(wxCommandEvent& event)
{
}
void TestFrame1::OnPlay(wxCommandEvent& event)
{
	m_tp->Play();
	m_toolBar->EnableTool(ID_PLAY,false);
	m_toolBar->EnableTool(ID_STOP,true);
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
	m_toolBar->EnableTool(ID_PLAY,true);
	m_toolBar->EnableTool(ID_STOP,false);
}
void TestFrame1::LoadFile(wxString& filename)
{
	m_tp->Load(filename);	
}

bool DropFiles::OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames) {
    for (size_t n = 0; n < filenames.Count(); n++) {
        if (wxFileName(filenames[n]).IsDir()) {
            wxMessageBox (_("It is currently not allowed to drop \n"
                            "directories onto the editor!"),
                          _("Error"), wxOK | wxICON_EXCLAMATION);
            return FALSE;
        }
    }
    if (filenames.Count()<=0)
    	return FALSE;
    m_frame->LoadFile(filenames[0]);
    return TRUE;
}
class Test: public wxApp
{
	virtual bool OnInit();
	wxArrayString m_fnames;
	bool ProcessCmdLine (wxChar** argv, int argc = 0);
};

IMPLEMENT_APP(Test)

bool Test::OnInit()

{
	SetAppName(wxT("ggseq"));

	ProcessCmdLine (argv, argc);
	
	TestFrame1 *frame = new TestFrame1( wxT("Test"), wxPoint(50,50), wxSize(600,450));
	frame->SetIcon(wxICON(ggseq_32));
	frame->Show( TRUE );
	SetTopWindow( frame );
	if (m_fnames.GetCount()>0) {
		frame->LoadFile(m_fnames[0]);
	}
	
//	wxMessageDialog dlg(frame, wxT("Abc"));
//	dlg.ShowModal();
	return TRUE;
}
bool Test::ProcessCmdLine (wxChar** argv, int argc)
{
	// get and process command line
	static const wxCmdLineEntryDesc cmdLineDesc[] = {
		{wxCMD_LINE_PARAM,  NULL, NULL, _T("input files"),
		wxCMD_LINE_VAL_STRING,
		wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser parser (cmdLineDesc, argc, argv);
	// get filenames from the command line
	m_fnames.Clear();
	if (parser.Parse() == 0) {
		for (size_t paramNr=0; paramNr < parser.GetParamCount(); paramNr++) {
			m_fnames.Add (parser.GetParam (paramNr));
		}
	}
	return true;
}
