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

#ifndef __ggseq_ui_H__
#define __ggseq_ui_H__

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "ggseq_ui.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "ggseq_ui_wdr.h"
class FileInfoPanel;
class FileList;
class TLPanel;
class MiniPlayer;
class UpdateListener;
class wxDragImage;


// WDR: class declarations

//----------------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------------

class MyFrame: public wxFrame, UndoRedoChangeListener
{
public:
    // constructors and destructors
    MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    ~MyFrame();

    // Public Methods
    void LoadFile(wxString& filename); 
    wxStatusBar* OnCreateStatusBar(int number,
    long style, wxWindowID id, const wxString& name);
    void UndoRedoChanged();
private:
    // WDR: method declarations for MyFrame
    wxSplitterWindow* GetTimelineSplitter()  { return (wxSplitterWindow*) FindWindow( ID_TIMELINE_SPLITTER ); }
    wxSplitterWindow* GetMainSplitter()  { return (wxSplitterWindow*) FindWindow( ID_MAIN_SPLITTER ); }
    wxGenericDirCtrl* GetDirtree()  { return (wxGenericDirCtrl*) FindWindow( ID_DIRTREE ); }
    FileInfoPanel* GetFileInfoPanel()  { return (FileInfoPanel*) FindWindow( ID_TL_SIDEPANEL ); }
    FileList* GetFilelist()  { return (FileList*) FindWindow( ID_TL_FILELIST ); }
    TLPanel* GetTlPanel()  { return (TLPanel*) FindWindow( ID_TL_PANEL ); }
    MiniPlayer* GetMiniplayer()  { return (MiniPlayer*) FindWindow( ID_TL_MINIPLAYER ); }
    void CreateMyMenuBar();
    void CreateMyToolBar();
    void CreateAcceleratorTable();
    void RefreshWindowTitle();
    void Stop();
    void SetUndoRedoButtons();
private:
    // WDR: member variable declarations for MyFrame
    UpdateListener *m_updateListener;
    wxDragImage *m_dragImage;
    bool m_DraggingFile;
    wxTimer *m_timer;
    wxHtmlHelpController     m_HtmlHelp;

private:
    // WDR: handler declarations for MyFrame
    void OnHelp( wxCommandEvent &event );
    void OnTimelineSplitterChanging( wxSplitterEvent &event );
    void OnExportPack( wxCommandEvent &event );
    void OnImportPack( wxCommandEvent &event );
    void OnZoom150( wxCommandEvent &event );
    void OnZoom100( wxCommandEvent &event );
    void OnZoom50( wxCommandEvent &event );
    void OnDeleteTrack( wxCommandEvent &event );
    void OnAddTrack( wxCommandEvent &event );
    void OnPreferences( wxCommandEvent &event );
    void OnStop( wxCommandEvent &event );
    void OnRewind( wxCommandEvent &event );
    void OnPlay( wxCommandEvent &event );
    void OnScroll( wxScrollEvent &event );
    void OnScroll2( wxScrollEvent &event );
    void OnMasterVolume( wxScrollEvent &event );
    void OnRedo( wxCommandEvent &event );
    void OnUndo( wxCommandEvent &event );
    void OnExportWav( wxCommandEvent &event );
    void OnSaveAs( wxCommandEvent &event );
    void OnSave( wxCommandEvent &event );
    void OnOpen( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    void OnFLStartDrag(wxListEvent& event);
    void OnMouseStopDrag(wxMouseEvent& event);
    void OnMouseDragging(wxMouseEvent& event);
    void OnFLItemActivated(wxListEvent& event);
    void OnFLItemSelected(wxListEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnActivate( wxActivateEvent &event );
private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// GgseqApp
//----------------------------------------------------------------------------

class GgseqApp: public wxApp
{
public:
    GgseqApp();

    virtual bool OnInit();
    virtual int OnExit();
    wxArrayString m_fnames;
    bool ProcessCmdLine (wxChar** argv, int argc = 0);
};

#endif
