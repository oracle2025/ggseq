/* FileInfoPanel.cpp
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

#include "FileInfoPanel.h"
#include "ggEvtHandler.h"
#include "stuff.h"

enum
{
	ID_TEXT = 1,
	ID_TEXTCTRL
};

FileInfoPanel::FileInfoPanel(wxWindow* parent, wxWindowID id )
	: wxPanel(parent, id,
	wxDefaultPosition,wxDefaultSize,
	wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN)
{
	g_ggseqProps.SetFileInfoListener( this );
	InfoPanel(this);
}
void FileInfoPanel::SetInfo(const wxString &filename, long frames, long channels, long sampleRate)
{

	m_fileNameTC->SetValue(filename);
	wxString ab;
	ab << frames;
	m_framesTC->SetValue(ab);
	m_lengthTC->SetValue(GenerateLengthString(frames,sampleRate));
	ab=wxT("");
	ab << channels;
	m_channelsTC->SetValue(ab);
	ab=wxT("");
	ab << sampleRate;
	m_sampleRateTC->SetValue(ab);
}
FileInfoPanel::~FileInfoPanel()
{
#ifdef __WXMSW__
    FindWindow(wxT("item4"))->PopEventHandler(true);
    FindWindow(wxT("item6"))->PopEventHandler(true);
    FindWindow(wxT("item8"))->PopEventHandler(true);
    FindWindow(wxT("item10"))->PopEventHandler(true);
    FindWindow(wxT("item12"))->PopEventHandler(true);
#endif
}
wxString FileInfoPanel::GenerateLengthString(long frames, long sampleRate)
{
	wxString result;
	wxString tmp;

	if (sampleRate < 1)
		return result ;

	int seconds = frames / sampleRate ;

	result.Printf(wxT("%02d:"), seconds / 60 / 60) ;
//	snprintf (str, sizeof (str) - 1, "%02d:", seconds / 60 / 60) ;

	seconds = seconds % (60 * 60) ;
	tmp.Printf(wxT("%02d:"), seconds / 60) ;
	result << tmp;
//	snprintf (str + strlen (str), sizeof (str) - strlen (str) - 1, "%02d:", seconds / 60) ;

	seconds = seconds % 60 ;
	tmp.Printf(wxT("%02d."), seconds) ;
	result << tmp;
//	snprintf (str + strlen (str), sizeof (str) - strlen (str) - 1, "%02d.", seconds) ;

	long long f; //loff_t f; /*nötig um mit großen Zahlen zu hantieren ?*/
	f=frames;
	seconds = ((1000 * f) / sampleRate) % 1000 ;
	tmp.Printf(wxT("%03d"), seconds) ;
	result << tmp;
//	snprintf (str + strlen (str), sizeof (str) - strlen (str) - 1, "%03d", seconds) ;
	

	return result;
}
wxSizer *FileInfoPanel::InfoPanel( wxWindow *parent, bool call_fit, bool set_sizer)
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
#ifdef __WXMSW__
    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
#else
    wxStaticBox *item2 = new wxStaticBox( parent, -1, wxT("") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
#endif

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 2, 5, 5 );
    item3->AddGrowableCol( 1 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("Filename"), wxDefaultPosition, wxDefaultSize, 0 ,wxT("item4"));
    item3->Add( item4, 0, wxALIGN_CENTER_VERTICAL, 5 );

    m_fileNameTC = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item3->Add( m_fileNameTC, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Length"), wxDefaultPosition, wxDefaultSize, 0 ,wxT("item6"));
    item3->Add( item6, 0, wxALIGN_CENTER_VERTICAL, 5 );

    m_lengthTC = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item3->Add( m_lengthTC, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_TEXT, wxT("Frames"), wxDefaultPosition, wxDefaultSize, 0 ,wxT("item8"));
    item3->Add( item8, 0, wxALIGN_CENTER_VERTICAL, 5 );

    m_framesTC = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item3->Add( m_framesTC, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item10 = new wxStaticText( parent, ID_TEXT, wxT("Channels"), wxDefaultPosition, wxDefaultSize, 0 ,wxT("item10"));
    item3->Add( item10, 0, wxALIGN_CENTER_VERTICAL, 5 );

    m_channelsTC = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item3->Add( m_channelsTC, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText *item12 = new wxStaticText( parent, ID_TEXT, wxT("Sample Rate"), wxDefaultPosition, wxDefaultSize, 0 ,wxT("item12"));
    item3->Add( item12, 0, wxALIGN_CENTER_VERTICAL, 5 );

    m_sampleRateTC = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY );
    item3->Add( m_sampleRateTC, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item3, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
#ifdef __WXMSW__
    item4->PushEventHandler(new NoBgEvtHandler());
    item6->PushEventHandler(new NoBgEvtHandler());
    item8->PushEventHandler(new NoBgEvtHandler());
    item10->PushEventHandler(new NoBgEvtHandler());
    item12->PushEventHandler(new NoBgEvtHandler());
#endif
    if (set_sizer)
    {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}

