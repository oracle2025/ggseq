/* TLSample.cpp
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
#include <sndfile.h>
#include <samplerate.h>

#include "TLSample.h"
#include "TLColourManager.h"
//#include <iostream>

TLSample::TLSample(const wxString &filename, int id,TLColourManager *colourMan)
{
	SNDFILE *sndfile;
	SF_INFO sfinfo;
	m_valid=false;
	m_buffer=NULL;
	m_colourMan=colourMan;
	m_id=id;
	m_refCount=0;
	m_filename=filename;
	sndfile=sf_open(filename.mb_str(),SFM_READ,&sfinfo);
	if (SF_ERR_NO_ERROR!=sf_error(sndfile)) { /*TODO Error loggen !*/
//		std::cerr << sf_strerror(sndfile) << std::endl;
		return;
	}
	if (sfinfo.channels!=2 && sfinfo.channels!=1) {
		sf_close(sndfile);
		return;
	}
/*	if (sfinfo.samplerate!=44100) {
		sf_close(sndfile);
		return;
	}*/
	m_length = sfinfo.frames*2;
	m_buffer=new float[m_length];
	sf_readf_float(sndfile,m_buffer,sfinfo.frames);
	if (sfinfo.channels==1) /*Mono-Sample strecken.*/
		for (int i=sfinfo.frames-1;i>=0;i--) {
			m_buffer[2*i+1]=m_buffer[i];
			m_buffer[2*i]=m_buffer[i];
		}
	/*Hier auf passende Samplerate strecken*/
	if (sfinfo.samplerate!=44100) {
//		wxLogMessage(wxT("No 44100 Samplerate")); // TODO Fehler ausgeben.
		SRC_DATA src_data;
		src_data.src_ratio=44100.0/sfinfo.samplerate;
			/*output_sample_rate / input_sample_rate*/
		src_data.input_frames=m_length/2;
		src_data.output_frames=(src_data.input_frames*src_data.src_ratio)+5;
		
		src_data.data_in=m_buffer;
		src_data.data_out=new float[src_data.output_frames*2];
		
		
puts("---");
		src_simple(&src_data,SRC_SINC_MEDIUM_QUALITY,2);
puts("---");
		delete src_data.data_in;
		m_buffer=src_data.data_out;
		m_length=src_data.output_frames_gen*2;
/*		wxString Message;
		Message << wxT("input_frames: ") << src_data.input_frames
		<< wxT("\n output_frames: ") << src_data.output_frames
		<< wxT("\n src_data.src_ratio: ") << src_data.src_ratio
		<< wxT("\n output_frames_gen; ") << src_data.output_frames_gen;
		wxLogMessage(Message);*/
//		return;
	}
	
	if (SF_ERR_NO_ERROR!=sf_error(sndfile))
	{
		sf_close(sndfile);/*???*/
		//std::cerr << sf_strerror(sndfile) << std::endl;
		return;
	}
	sf_close(sndfile);
	m_valid=true;
}
void TLSample::UnRef()
{
	m_refCount--;
}
void TLSample::Ref()
{
	m_refCount++;
}
int TLSample::GetRefCount()
{
	return m_refCount;
}
int TLSample::GetId()
{
	return m_id;
}
	
TLSample::~TLSample()
{
	if (m_buffer!=NULL)
	{
//		std::cout << "TLSample gelöscht " << m_filename.mb_str() << std::endl;
		delete m_buffer;
	}
}
float *TLSample::GetBuffer()
{
	return m_buffer;
}

int TLSample::GetLength()
{
	return m_length;
}

bool TLSample::IsValid()
{
	return m_valid;
}

wxString TLSample::GetFilename()
{
	return m_filename;
}

wxIcon TLSample::GetIcon()
{
	int width=m_length/3793;
	wxBitmap b1(width,25);
	wxMemoryDC dc1;
	dc1.SelectObject(b1);
	wxBrush br1=dc1.GetBrush();
	br1.SetColour(GetColour());
	dc1.SetBrush(br1);
//	dc1.SetBrush(*wxRED_BRUSH);
	dc1.DrawRectangle(0,0,width,25);
	wxIcon icon1;
	icon1.CopyFromBitmap(b1);
	return icon1;
}
void TLSample::Draw(wxDC& dc)
{
	wxBrush b1=dc.GetBrush();
	b1.SetColour(GetColour());
	dc.SetBrush(b1);
	int width=m_length/3793;
	dc.DrawRectangle(0,0,width,25);
	dc.SetClippingRegion(0,0,width,25);
	dc.SetFont(*wxSMALL_FONT);
	wxFileName fn(GetFilename());
//	dc.DrawText(fn.GetName(),1,1);
	dc.DestroyClippingRegion();
}
wxColour TLSample::GetColour()
{
	return m_colourMan->GetColour(m_filename);
}
