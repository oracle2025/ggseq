/* TLXMLLoader2.h
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

#ifndef _TLXMLLOADER2_H_
#define _TLXMLLOADER2_H_

class TLData;
class TLSampleManager;

class TLXMLLoader2
{
	public:
		TLXMLLoader2(TLData *data, TLSampleManager *sm);
		void LoadFile(wxString filename);
	private:
		void Error(wxString msg);
		TLData *m_data;
		TLSampleManager *m_sampleManager;
};


#endif /* _TLXMLLOADER2_H_ */ 
