/* PlayerInterface.h
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

#ifndef _PLAYER_INTERFACE_H_
#define _PLAYER_INTERFACE_H_

class TLData;

class PlayerInterface
{
	public:
		// constructors and destructors
		virtual ~PlayerInterface(){};
		
		// method declarations
		virtual int FillBuffer( float *buffer, unsigned long frames )=0;
		virtual gg_tl_dat GetPosition()=0;
		virtual bool Valid(){ return true; }
};

class TimelinePlayer : public PlayerInterface
{
	public:
		// constructors and destructors
		TimelinePlayer( TLData *data );
		~TimelinePlayer();

		// method declarations
		int FillBuffer( float *buffer, unsigned long frames );
		gg_tl_dat GetPosition();
	private:
		// member variable declarations
		TLData *m_data;

};

class SamplePlayer : public PlayerInterface
{
	public:
		// constructors and destructors
		SamplePlayer( TLSample *sample );
		SamplePlayer( wxString filename, long &length,
				long &frames, long &channels,
				long &sampleRate,
				UpdateListener *updateListener=NULL );
		~SamplePlayer();

		// method declarations
		int FillBuffer( float *buffer, unsigned long frames );
		gg_tl_dat GetPosition(){ return m_position; }
		bool Valid(){ return m_sample; }
	private:
		// member variable declarations
		TLSample *m_sample;
		unsigned long m_position;
		// method declarations
		unsigned int FillBuffer_Sample(float* outBuffer, unsigned int count);

};

class LoopPlayer : public PlayerInterface
{
	public:
		// constructors and destructors
		LoopPlayer( float* sample, gg_tl_dat len );
		~LoopPlayer();

		// method declarations
		int FillBuffer( float *buffer, unsigned long frames );
		gg_tl_dat GetPosition(){ return m_loopPos; }
	private:
		// member variable declarations
		float *m_loopSample;
		gg_tl_dat m_loopLen;
		gg_tl_dat m_loopPos;
};
#endif /* _PLAYER_INTERFACE_H_ */
