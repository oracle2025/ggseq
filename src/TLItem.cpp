/* TLItem.cpp
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
#include <sndfile.h>

#include "TLItem.h"
#include "TLSample.h"
#include <iostream>

TLItem::TLItem(TLSample *sample, int trackNr, int position)
{
	m_sample=sample;
	m_position=position;
	m_sample->Ref();
	m_playingOffset=0;
	m_selected=false;
	m_trackNr = trackNr;
}
TLItem::~TLItem()
{
	std::cout << "TLItem gelöscht" << std::endl;
	m_sample->UnRef();
}
int TLItem::FillBuffer(float* outBuffer, int pos, int count, bool mute)
{
	int written=0;
	float *buffer = m_sample->GetBuffer();
	if (m_position+GetLength()<pos)
	{
		return 0;
	}
	if (m_position>=pos)
	{
		for (int i=pos;i<m_position && written<count;i++)
		{
			outBuffer[i-pos]=0.0;
			written++;
		}
	}
	if (pos>m_position+m_playingOffset)
		m_playingOffset = pos-m_position;
	while(written<count && m_playingOffset<GetLength())
	{
		if (mute) {
			outBuffer[written]=0.0;
		} else {
			outBuffer[written]=buffer[m_playingOffset];
		}
		written++;
		m_playingOffset++;
	}
	return written;

}
int TLItem::GetLength()
{
	return m_sample->GetLength();
}
int TLItem::GetTrack()
{
	return m_trackNr;
}
const int TLItem::GetPosition()
{
	return m_position;
}
int TLItem::GetEndPosition()
{
	return m_position+m_sample->GetLength();
}
void TLItem::SetPosition(int position)
{
	if (position%2) /*ist das notwendig um Stereo korrekt wiederzugeben ?*/
		m_position=position+1;
	else
		m_position=position;
}
void TLItem::ResetOffset()
{
	m_playingOffset=0;
}
TLSample *TLItem::GetSample()
{
	return m_sample;
}
void TLItem::Select()
{
	m_selected=true;
}
void TLItem::UnSelect()
{
	m_selected=false;
}
bool TLItem::IsSelected()
{
	return m_selected;
}

