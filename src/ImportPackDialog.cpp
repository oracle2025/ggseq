/////////////////////////////////////////////////////////////////////////////
// Name:        ImportPackDialog.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "ImportPackDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "ImportPackDialog.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// ImportPackDialog
//----------------------------------------------------------------------------

// WDR: event table for ImportPackDialog

BEGIN_EVENT_TABLE(ImportPackDialog,wxDialog)
    EVT_BUTTON( ID_PACKAGE_BUTTON, ImportPackDialog::OnPackageButton )
    EVT_BUTTON( ID_CONTENTS_BUTTON, ImportPackDialog::OnContentsButton )
END_EVENT_TABLE()

ImportPackDialog::ImportPackDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function ImportPackDlgFunc for ImportPackDialog
    ImportPackDlgFunc( this, TRUE );
    Centre();
}

wxString ImportPackDialog::GetContentsPath()
{
    return GetContentsTextctrl()->GetValue();
}

wxString ImportPackDialog::GetPackageFile()
{
    return GetPackageTextctrl()->GetValue();
}

// WDR: handler implementations for ImportPackDialog

void ImportPackDialog::OnContentsButton( wxCommandEvent &event )
{
    wxDirDialog dlg(this);
    if ( dlg.ShowModal() == wxID_OK ) {
        GetContentsTextctrl()->SetValue(dlg.GetPath());
    }
}

void ImportPackDialog::OnPackageButton( wxCommandEvent &event )
{
    wxString fn = wxFileSelector(wxT("Open Package"),wxT(""),wxT(""),
        wxT("ggz"),wxT("Gungirl Package files (*.ggz)|*.ggz"),wxOPEN,this);
    if ( !fn.empty() ) {
        GetPackageTextctrl()->SetValue( fn );
    }
}




