/* GgseqGlobalProps.h
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
#ifndef _GGSEQ_GLOBAL_PROPS_H_
#define _GGSEQ_GLOBAL_PROPS_H_

//TODO: Singelton statt diesem hier
class SoundManager;
class MiniPlayerInterface;
class FileInfoListener;
class UpdateListener;
class GgseqDocManager;
class GgseqGlobalProps
{
	public:
		GgseqGlobalProps();
		float GetZoom();
		SoundManager *GetSoundManager();
		void SetSoundManager( SoundManager *soundManager );
		MiniPlayerInterface *GetMiniPlayer();
		void SetMiniPlayer( MiniPlayerInterface *miniPlayer );
		void SetFileInfoListener( FileInfoListener *fileInfoListener );
		FileInfoListener *GetFileInfoListener();
		void SetDocManager( GgseqDocManager *docManager );
		GgseqDocManager *GetDocManager();
		/*UpdateListener *GetProgressBar();
		void SetProgressBar( UpdateListener *pBar );*/
		// GetTopLevelWindow();
		// GetLoopSetupListener();
	/* - Add Listeners - */
		//AddSnapChangeListener
		//AddZoomChangeListener
		//AddScrollChangeListener
		//AddUndoRedoChangeListener
	/* - Emit Signals - */
		//EmitSnapChange();
		//EmitZoomChange();
		//EmitScrollChange();
		//EmitUndoRedoChange();
	private:
		SoundManager *m_soundManager;
		MiniPlayerInterface *m_miniPlayer;
		FileInfoListener *m_fileInfoListener;
		UpdateListener *m_pBar;
		GgseqDocManager *m_docManager;
};
extern GgseqGlobalProps g_ggseqProps;

#endif /* _GGSEQ_GLOBAL_PROPS_H_ */
