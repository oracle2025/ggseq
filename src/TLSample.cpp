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
#include "UpdateListener.h"
#include <iostream>

TLSample::TLSample(const wxString &filename, int id,TLColourManager *colourMan, UpdateListener* updateListener)/*100000 Frames als intervall*/
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
		wxLogError(wxT("Error Opening Soundfile\n%s"), sf_strerror(sndfile));
		sf_close(sndfile);
		return;
	}
	m_infoFrames=sfinfo.frames;
	m_infoSampleRate=sfinfo.samplerate;
	m_infoChannels=sfinfo.channels;

	if (sfinfo.channels!=2 && sfinfo.channels!=1) {
		wxLogError(wxT("Wrong number of channels!\n(Only mono and stereo allowed)"));
		sf_close(sndfile);
		return;
	}
	if (sfinfo.frames==0) {
		wxLogError(wxT("Soundfile has length Zero!"));
		sf_close(sndfile);
		return;
	}
	m_length = sfinfo.frames*2;
	m_buffer=new float[m_length];

	int factors;
	if (sfinfo.samplerate==44100 && sfinfo.channels==2) {
		factors=100;
	} else if (sfinfo.samplerate!=44100 && sfinfo.channels==1) {
		factors=33;
	} else {
		factors=50;
	}
	/*dieser aufruf ist zeitkritisch*/
	for(int i=0;i<=sfinfo.frames;i+=100000) {//i sind frames ?
		if (updateListener)
			if(updateListener->Update((i*factors)/sfinfo.frames)==false) {
				sf_close(sndfile);
				return;
			}
		int cnt;
		if (i+100000>sfinfo.frames)
			cnt=sfinfo.frames-i;
		else
			cnt=100000;


		sf_readf_float(sndfile,m_buffer+i*sfinfo.channels,cnt);//segfault
		if (SF_ERR_NO_ERROR!=sf_error(sndfile)) { /*TODO Error loggen !*/
			wxLogError(wxT("Error Reading from Soundfile\n%s"), sf_strerror(sndfile));
			sf_close(sndfile);
			return;
		}

	}

	sf_close(sndfile);
	int prog_offset=factors;
	if (sfinfo.channels==1) {/*Mono-Sample strecken.*/ /*dieser aufruf ist zeitkritisch*/
		for (int i=sfinfo.frames-1;i>=0;i--) {
			m_buffer[2*i+1]=m_buffer[i];
			m_buffer[2*i]=m_buffer[i];
			if (i%100000==0) {
				if (updateListener)
					if (updateListener->Update(prog_offset+((sfinfo.frames-i)*factors)/sfinfo.frames)==false) {
						return;
					}
			}
		}
		prog_offset+=factors;
	}
	/*Hier auf passende Samplerate strecken*/
	if (sfinfo.samplerate!=44100) {
//		wxLogMessage(wxT("No 44100 Samplerate")); // TODO Fehler ausgeben.
		SRC_DATA src_data;
		src_data.src_ratio=44100.0/sfinfo.samplerate; /*output_sample_rate / input_sample_rate*/
		src_data.input_frames=100000;//m_length/2;
		src_data.output_frames=((m_length/2)*src_data.src_ratio)+25;
		src_data.data_in=m_buffer;
		float *out_buffer=new float[src_data.output_frames*2];
		src_data.data_out=out_buffer;

		int error;
		SRC_STATE *src_state = src_new(SRC_SINC_MEDIUM_QUALITY,2,&error);
		
		
//		src_simple(&src_data,SRC_SINC_MEDIUM_QUALITY,2);/*dieser aufruf ist zeitkritisch*/
		/*100000*/
		long buffer_length=m_length/2;
		long frames_generated=0;
//		for (long i=0;src_data.input_frames_used;i+=src_data.input_frames_used) {
		src_data.end_of_input=0;
		for(long i=0;buffer_length;i+=src_data.input_frames_used) {
			if (buffer_length<100000)
				src_data.input_frames=buffer_length;
			error=src_process(src_state, &src_data);
			if(error!=0) {
				wxLogError(wxT("Error converting Samplerate\n%s"), src_strerror(error));
			}
			src_data.output_frames-=src_data.output_frames_gen;
			frames_generated+=src_data.output_frames_gen;
			src_data.data_out+=src_data.output_frames_gen*2;
			src_data.data_in+=src_data.input_frames_used*2;
			buffer_length-=src_data.input_frames_used;
			if (updateListener)
				if (updateListener->Update(prog_offset+(i*factors)/(m_length/2))==false) {
					delete out_buffer;
					src_delete(src_state);
					return;
				}
		}
//		src_process(src_state, &src_data);

		src_delete(src_state);
		delete m_buffer;
		m_buffer=out_buffer;//src_data.data_out;
		m_length=frames_generated*2;//src_data.output_frames_gen*2;
//		return;
	}
	
/*	if (SF_ERR_NO_ERROR!=sf_error(sndfile))
	{
		sf_close(sndfile);
		wxLogError(wxT("2: %s"),sf_strerror(sndfile));
		return;
	}
	sf_close(sndfile);*/
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
	dc.DrawText(fn.GetName(),1,1);
	dc.DestroyClippingRegion();
}
wxColour TLSample::GetColour()
{
	return m_colourMan->GetColour(m_filename);
}
