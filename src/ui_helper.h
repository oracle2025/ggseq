/* ui_helper.h
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

#ifndef _UI_HELPER_H_
#define _UI_HELPER_H_

wxWindow *MakeTlPanel( wxWindow* parent, wxWindowID id );
wxWindow *MakeSidePanel( wxWindow* parent, wxWindowID id, wxBoxSizer *sizer );
wxWindow *MakeFileList( wxWindow* parent, wxWindowID id );
wxWindow *MakeDirTree( wxWindow* parent, wxWindowID id );
wxWindow *MakeMiniPlayer( wxWindow* parent, wxWindowID id );

#endif /* _UI_HELPER_H_ */
