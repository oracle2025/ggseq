/////////////////////////////////////////////////////////////////////////////
// Name:        ImportPackDialog.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __ImportPackDialog_H__
#define __ImportPackDialog_H__

#ifdef __GNUG__
    #pragma interface "ImportPackDialog.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ggseq_ui_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// ImportPackDialog
//----------------------------------------------------------------------------

class ImportPackDialog: public wxDialog
{
public:
    // constructors and destructors
    ImportPackDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
    // WDR: method declarations for ImportPackDialog
    wxTextCtrl* GetPackageTextctrl()  { return (wxTextCtrl*) FindWindow( ID_PACKAGE_TEXTCTRL ); }
    wxTextCtrl* GetContentsTextctrl()  { return (wxTextCtrl*) FindWindow( ID_CONTENTS_TEXTCTRL ); }
    wxString GetContentsPath();
    wxString GetPackageFile();
    
private:
    // WDR: member variable declarations for ImportPackDialog
    
private:
    // WDR: handler declarations for ImportPackDialog
    void OnContentsButton( wxCommandEvent &event );
    void OnPackageButton( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};




#endif
