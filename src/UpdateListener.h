/* UpdateListener.h
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
#ifndef _UPDATELISTENER_H_
#define _UPDATELISTENER_H_

class UpdateListener
{
	public:
		virtual bool Update(int status)=0;
		virtual void StartUpdateProcess()=0;
		virtual void EndUpdateProcess()=0;
};

class SimpleUpdateListener: public UpdateListener
{
	public:
		virtual bool Update(int status);
		virtual void StartUpdateProcess();
		virtual void EndUpdateProcess();
};

class RecursiveUpdateListener: public UpdateListener
{
	public:
		RecursiveUpdateListener(UpdateListener *updateListener, int start, int stop);
		virtual bool Update(int status);
		virtual void StartUpdateProcess();
		virtual void EndUpdateProcess();
	private:
		int m_start;
		int m_stop;
		UpdateListener *m_updateListener;
};
#endif /*_UPDATELISTENER_H_*/
