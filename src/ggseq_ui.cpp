/* ggseq_ui.cpp
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

#ifdef __GNUG__
    #pragma implementation "ggseq_ui.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/dirctrl.h>
#include <wx/filename.h>
#include <wx/dnd.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/cmdline.h>
#include <wx/dragimag.h>
#include <wx/config.h>

// Include private headers
#include "ggseq_ui.h"
#include "stuff.h"
#include "TLMiniFiler2.h"
#include "TLPanel.h"
#include "MiniPlayer.h"
#include "UpdateListener.h"
#include "StatusProgressBar.h"
#include "DisableListener.h"
#include "FileInfoPanel.h"
#include "ImportPackDialog.h"
//#include "PreferencesDialog.h"

// Include Images
#include "ggseq_32.xpm"
#include "dndfile.xpm"

class DropFiles: public wxFileDropTarget
{
    public:
        DropFiles (MyFrame *frame) {m_frame = frame;}
        virtual bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames);
    private:
        MyFrame *m_frame;
};
bool DropFiles::OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames) {
    for (size_t n = 0; n < filenames.Count(); n++) {
        if (wxFileName(filenames[n]).IsDir()) {
            wxMessageBox (_("It is currently not allowed to drop \n"
                            "directories onto Gungirl Sequencer!"),
                          _("Error"), wxOK | wxICON_EXCLAMATION);
            return FALSE;
        }
    }
    if (filenames.Count()<=0)
        return FALSE;
    m_frame->LoadFile(filenames[0]);
    return TRUE;
}



// WDR: class implementations

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

// WDR: event table for MyFrame

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(ID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(ID_QUIT, MyFrame::OnQuit)
    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_MENU( ID_NEW, MyFrame::OnNew )
    EVT_MENU( ID_OPEN, MyFrame::OnOpen )
    EVT_MENU( ID_SAVE, MyFrame::OnSave )
    EVT_MENU( ID_SAVEAS, MyFrame::OnSaveAs )
    EVT_MENU( ID_WAV_EXPORT, MyFrame::OnExportWav )
    EVT_MENU( ID_UNDO, MyFrame::OnUndo )
    EVT_MENU( ID_REDO, MyFrame::OnRedo )
    EVT_COMMAND_SCROLL_THUMBTRACK( ID_VOLUME_SLIDER, MyFrame::OnMasterVolume )
    EVT_COMMAND_SCROLL_THUMBTRACK( ID_BIG_SCROLLBAR, MyFrame::OnScroll )
    EVT_COMMAND_SCROLL_THUMBTRACK( ID_SCROLLBAR2, MyFrame::OnScroll2 )
    EVT_LIST_BEGIN_DRAG(ID_TL_FILELIST,MyFrame::OnFLStartDrag)
    EVT_MOTION(MyFrame::OnMouseDragging)
    EVT_LEFT_UP(MyFrame::OnMouseStopDrag)
    EVT_LIST_ITEM_ACTIVATED(ID_TL_FILELIST,MyFrame::OnFLItemActivated)
    EVT_LIST_ITEM_SELECTED(ID_TL_FILELIST,MyFrame::OnFLItemSelected)
    EVT_TREE_SEL_CHANGED(-1,MyFrame::OnSelChanged)
    EVT_TIMER(-1,MyFrame::OnTimer)
    EVT_TOOL( ID_PLAY, MyFrame::OnPlay )
    EVT_TOOL( ID_REWIND, MyFrame::OnRewind )
    EVT_TOOL( ID_STOP, MyFrame::OnStop )
    EVT_MENU( ID_PREFERENCES, MyFrame::OnPreferences )
    EVT_MENU( ID_ADD_TRACK, MyFrame::OnAddTrack )
    EVT_MENU( ID_DELETE_TRACK, MyFrame::OnDeleteTrack )
    EVT_MENU( ID_ZOOM_50, MyFrame::OnZoom50 )
    EVT_MENU( ID_ZOOM_100, MyFrame::OnZoom100 )
    EVT_MENU( ID_ZOOM_150, MyFrame::OnZoom150 )
    EVT_MENU( ID_IMPORT_PACK, MyFrame::OnImportPack )
    EVT_MENU( ID_SAVEAS_PACK, MyFrame::OnExportPack )
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    CreateMyMenuBar();
    
    CreateMyToolBar();
    
    CreateStatusBar(1, wxST_SIZEGRIP );
    SetStatusText( wxT("Welcome!") );

    CreateAcceleratorTable();

    // insert main window here
    wxPanel *panel1 = new wxPanel(this, -1);
    MainFrameFunc(panel1);

    m_dragImage = new wxDragImage(wxBitmap(dndfile_xpm));
    m_DraggingFile=false;


    // Connecting various Components
    //GetMiniplayer()->SetSoundManager( GetTlPanel()->GetSoundManager() );
//    GetMiniplayer()->SetFileInfoListener( GetFileInfoPanel() );
    GetMiniplayer()->SetUpdateListener( m_updateListener );
    RefreshWindowTitle();
    SetDropTarget (new DropFiles (this));
    GetTlPanel()->SetUpdateListener(m_updateListener);
    ((StatusProgressBar*)m_updateListener)->SetDisableListener(new DoubleDisabler(panel1,GetToolBar())); //TODO: Wer l�scht die Dinger?, Men� kann nicht komplett disabled werden.
//    GetTlPanel()->SetMiniPlayer(GetMiniplayer());
    m_timer = new wxTimer(this);
    wxConfigBase *conf=wxConfigBase::Get();
    GetDirtree()->SetPath(conf->Read(wxT("MiniFilerDirectory"), wxT("/")));

    // GUI - Hints
    GetMenuBar()->Enable( ID_MOVE_TRACK_UP, false );
    GetMenuBar()->Enable( ID_MOVE_TRACK_DOWN, false );
    GetMenuBar()->Enable( ID_SHOW_MIXER, false );
    SetSizeHints(470,500);
    GetMainSplitter()->SetSashPosition(200);
    GetTlPanel()->SetFocus();//N�tig, damit unter wxGTK die Cursor funktionieren.
}
MyFrame::~MyFrame()
{
    wxConfigBase *conf=wxConfigBase::Get();
    conf->SetPath(wxT("/"));
    conf->Write(wxT("MiniFilerDirectory"), GetDirtree()->GetPath());
    conf->Write(wxT("Version"), wxT(GG_VERSION));
//    if ( GetTlPanel()->GetFilename().Cmp(wxT("Unnamed")) ) {
        conf->Write(wxT("LastProject"), GetTlPanel()->GetFilename());
//    }
    conf->Flush();
    delete m_dragImage;
}
void MyFrame::CreateMyMenuBar()
{
#ifdef __WXMAC__
    wxApp::s_macAboutMenuItemId = ID_ABOUT;
#endif

    SetMenuBar( MyMenuBarFunc() );
}

void MyFrame::CreateMyToolBar()
{
    wxToolBar *tb = CreateToolBar( wxTB_HORIZONTAL|wxNO_BORDER /* |wxTB_FLAT */ );
    
    MyToolBarFunc( tb );
}
void MyFrame::CreateAcceleratorTable()
{
    wxAcceleratorEntry entries[8];
    entries[0].Set(wxACCEL_NORMAL,  WXK_SPACE, ID_PLAY);
    entries[1].Set(wxACCEL_NORMAL, 'S' , ID_STOP);
    entries[2].Set(wxACCEL_NORMAL,'R', ID_REWIND);
    entries[3].Set(wxACCEL_CTRL,'N', ID_NEW);
    entries[4].Set(wxACCEL_CTRL,'O', ID_OPEN);
    entries[5].Set(wxACCEL_CTRL,'S', ID_SAVE);
    entries[6].Set(wxACCEL_CTRL|wxACCEL_SHIFT,'S', ID_SAVEAS);
    entries[7].Set(wxACCEL_CTRL,'Q', ID_QUIT);
    wxAcceleratorTable accel(8, entries);
    SetAcceleratorTable(accel);
}
wxStatusBar* MyFrame::OnCreateStatusBar(int number, long style, wxWindowID id,
  const wxString& name)
{
  StatusProgressBar *status_bar = new StatusProgressBar(this);
  m_updateListener=status_bar;
  status_bar->SetFieldsCount(number);
  return status_bar;
}
void MyFrame::RefreshWindowTitle()
{
    wxFileName fn(GetTlPanel()->GetFilename());
    SetTitle(fn.GetFullName() + wxT(" - Gungirl Sequencer"));
}
void MyFrame::LoadFile(wxString& filename)
{
    GetTlPanel()->Load(filename);   
}
void MyFrame::Stop()
{
    m_timer->Stop();
    GetToolBar()->EnableTool(ID_PLAY,true);
    GetToolBar()->EnableTool(ID_STOP,false);
}


// WDR: handler implementations for MyFrame

void MyFrame::OnExportPack( wxCommandEvent &event )
{
    GetTlPanel()->ExportPackage();
}

void MyFrame::OnImportPack( wxCommandEvent &event )
{
    ImportPackDialog dlg(this, -1, wxT("Import Package"));
    if ( dlg.ShowModal()==wxID_OK ) {
        //puts(dlg.GetContentsPath().mb_str());
        //puts(dlg.GetPackageFile().mb_str());
	GetTlPanel()->ImportPackage(dlg.GetPackageFile(), dlg.GetContentsPath());
    	RefreshWindowTitle();
    }
}

void MyFrame::OnZoom150( wxCommandEvent &event )
{
    GetTlPanel()->SetZoom( 1.5 );
    Refresh();
}

void MyFrame::OnZoom100( wxCommandEvent &event )
{
    GetTlPanel()->SetZoom( 1.0 );
    Refresh();
}

void MyFrame::OnZoom50( wxCommandEvent &event )
{
    GetTlPanel()->SetZoom( 0.5 );
    Refresh();
}

void MyFrame::OnDeleteTrack( wxCommandEvent &event )
{
    GetTlPanel()->DeleteTrack();
}

void MyFrame::OnAddTrack( wxCommandEvent &event )
{
    GetTlPanel()->AddTrack();
}

void MyFrame::OnPreferences( wxCommandEvent &event )
{
    GetTlPanel()->SetPrefs();
    /*PreferencesDialog prefs(this, -1, wxT("Preferences"));
    prefs.Centre();
    prefs.ShowModal();*/
}

void MyFrame::OnStop( wxCommandEvent &event )
{
    Stop();
    GetTlPanel()->Stop(); //TODO: in die Funktion Stop() verlagern
}

void MyFrame::OnRewind( wxCommandEvent &event )
{
    Stop();
    GetTlPanel()->Stop();
    GetTlPanel()->Rewind();
}

void MyFrame::OnPlay( wxCommandEvent &event )
{
    GetTlPanel()->Play();
    GetToolBar()->EnableTool(ID_PLAY,false);
    GetToolBar()->EnableTool(ID_STOP,true);
    m_timer->Start(100);
}
void MyFrame::OnTimer(wxTimerEvent& event)
{
    if(!GetTlPanel()->UpdateCaret()) {
        Stop();
    }
}
void MyFrame::OnFLStartDrag(wxListEvent& event)
{
    m_dragImage->BeginDrag(wxPoint(0,0),this);
    m_dragImage->Move(ScreenToClient(GetFilelist()->ClientToScreen(event.GetPoint())));
    m_dragImage->Show();
    m_DraggingFile=true;
}
void MyFrame::OnMouseStopDrag(wxMouseEvent& event)
{
    if (!m_DraggingFile)
        return;
    m_DraggingFile=false;
    m_dragImage->Hide();
    m_dragImage->EndDrag();
    int x_=event.m_x;
    int y_=event.m_y;
    ClientToScreen(&x_,&y_);
    GetTlPanel()->DropFileAt(x_,y_,GetFilelist()->GetSelection());

}
void MyFrame::OnMouseDragging(wxMouseEvent& event)
{
    if (!m_DraggingFile)
        return;
    m_dragImage->Move(wxPoint(event.m_x,event.m_y));
}

void MyFrame::OnFLItemActivated(wxListEvent& event)
{
    OnFLItemSelected(event);
    GetMiniplayer()->Play();
}
void MyFrame::OnFLItemSelected(wxListEvent& event)
{
    GetMiniplayer()->SetFilename(GetFilelist()->GetSelection());
}
void MyFrame::OnSelChanged(wxTreeEvent& event)
{
    if (!GetDirtree() || !GetFilelist())
        return;
    GetFilelist()->SetDirectory(GetDirtree()->GetPath());
}
void MyFrame::OnMasterVolume( wxScrollEvent &event )
{
    GetTlPanel()->SetMasterVolume( 1.0-((float) event.GetPosition())/100 );
}
void MyFrame::OnScroll( wxScrollEvent &event )
{
    GetTlPanel()->OnScroll(event);
}
void MyFrame::OnScroll2( wxScrollEvent &event )
{
    GetTlPanel()->OnScroll2(event);
}

void MyFrame::OnRedo( wxCommandEvent &event )
{
    GetTlPanel()->Redo();
}

void MyFrame::OnUndo( wxCommandEvent &event )
{
    GetTlPanel()->Undo();
}

void MyFrame::OnExportWav( wxCommandEvent &event )
{
    Stop();
    GetTlPanel()->WavExport();
}

void MyFrame::OnSaveAs( wxCommandEvent &event )
{
    GetTlPanel()->SaveAs();
    RefreshWindowTitle();
}

void MyFrame::OnSave( wxCommandEvent &event )
{
    GetTlPanel()->Save();
    RefreshWindowTitle();
}

void MyFrame::OnOpen( wxCommandEvent &event )
{
    Stop();
    GetTlPanel()->Load();
    RefreshWindowTitle();
}

void MyFrame::OnNew( wxCommandEvent &event )
{
    Stop();
    GetTlPanel()->New();
    RefreshWindowTitle();
}

void MyFrame::OnAbout( wxCommandEvent &event )
{
    wxMessageDialog dialog( this, wxT("Welcome to Gungirl Sequencer " GG_VERSION "\n(C)opyright Richard Spindler\n\nhttp://ggseq.sourceforge.net/"),
        wxT("About Gungirl Sequencer"), wxOK|wxICON_INFORMATION );
    dialog.ShowModal();
}

void MyFrame::OnQuit( wxCommandEvent &event )
{

     Close( TRUE );
}

void MyFrame::OnCloseWindow( wxCloseEvent &event )
{
    // if ! saved changes -> return
    if (!GetTlPanel()->SaveOnExit()) {
        event.Veto();
    } else {
        m_timer->Stop();
        GetTlPanel()->StopAll();
        this->Destroy();
    }
    
//    Destroy();
}

//------------------------------------------------------------------------------
// GgseqApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(GgseqApp)

GgseqApp::GgseqApp()
{
}

bool GgseqApp::OnInit()
{
    SetAppName(wxT("ggseq"));
    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);
    ProcessCmdLine (argv, argc);
    MyFrame *frame = new MyFrame( NULL, -1,
        wxT("SuperApp"), wxPoint(20,20), wxSize(600,340) );
    frame->SetIcon(wxICON(ggseq_32));
    frame->Show( TRUE );
    
    SetTopWindow( frame );
    if (m_fnames.GetCount()>0) {
        frame->LoadFile(m_fnames[0]);
    } else {
    	wxConfigBase *conf=wxConfigBase::Get();
    	conf->SetPath(wxT("/"));
	bool loadLastProject=false;
	conf->Read(wxT("LoadLastProject"), &loadLastProject);
	if (loadLastProject) {
	    wxString lastFile;
	    conf->Read(wxT("LastProject"), &lastFile );
	    if (wxFileExists(lastFile)) {
                frame->LoadFile(lastFile);
            }
	}
    }/* else LoadLastProject? */
    //frame->argv0=argv[0];

    return TRUE;
}

int GgseqApp::OnExit()
{
    return 0;
}
bool GgseqApp::ProcessCmdLine (wxChar** argv, int argc)
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
