/* UpdateListener.cpp
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

#include "UpdateListener.h"
#include <iostream>

bool SimpleUpdateListener::Update(int status)
{
	std::cout << "Status: " << status << std::endl;
	return true;
}
void SimpleUpdateListener::StartUpdateProcess()
{
}
void SimpleUpdateListener::EndUpdateProcess()
{
}


RecursiveUpdateListener::RecursiveUpdateListener(UpdateListener *updateListener, int start, int stop)
{
	m_start=start;
	m_stop=stop;
	m_updateListener=updateListener;
}
bool RecursiveUpdateListener::Update(int status)
{
	return m_updateListener->Update(m_start+(status*(m_stop-m_start))/100);
}
void RecursiveUpdateListener::StartUpdateProcess()
{
}
void RecursiveUpdateListener::EndUpdateProcess()
{
}

