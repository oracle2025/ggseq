/* SoundManager.h
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

#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_
class TLData;
class TLSample;
class UpdateListener;
class PlayerInterface;

class SoundManager// : public wxEvtHandler
{
	public:
		SoundManager(TLData *TlData);
		~SoundManager();
		void Play( PlayerInterface *playerIface );
		void Stop();
		bool Done();
		gg_tl_dat GetPosition();
		
	private:
		PlayerInterface *m_playerInterface;
		void StartStream(void* callback);
		void StopStream();
		TLData *m_data;
};

#endif /*_SOUNDMANAGER_H_*/
