/* SampleEdit.h
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

#ifndef _SAMPLE_EDIT_H_
#define _SAMPLE_EDIT_H_

class SampleEdit
{
	public:
		SampleEdit( float *buffer, int len );
		virtual ~SampleEdit();
		void SetTempo( float tempo );
		void SetTrims( int start, int end ); // set -1 for no trim
			/* Wie 2Channels handln?
			 * einfach beide Werte mal 2 nehmen.
			 */
		float *Convert( int &len );
		float *Trim( int &len );
	private:
		float *m_buffer;
		int    m_start, m_end, m_len;
		float  m_tempo;
};

#endif /* _SAMPLE_EDIT_H_ */
