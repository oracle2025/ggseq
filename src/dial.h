/* dial.h
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
 
#ifndef _DIAL_H_
#define _DIAL_H_

/*BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_MF_ACTIVATE_COMMAND, 7781)
END_DECLARE_EVENT_TYPES()

#define EVT_MF_ACTIVATE_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MF_ACTIVATE_COMMAND, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
    ),
*/


class wxDialTip;

class wxDial : public wxControl
{
	public:
		wxDial(wxWindow* parent,
			wxWindowID id, int value ,
			int minValue, int maxValue,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = 0,
			const wxString& name = wxT("dial"))
		{
			Create(parent,id,value,minValue,maxValue,pos,size,style,name);
		}
		~wxDial();
		bool Create(wxWindow* parent,
			wxWindowID id, int value ,
			int minValue, int maxValue,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = 0,
			const wxString& name = wxT("dial"));
		void SetValue(int value);
		void OnPaint(wxPaintEvent& event);
		void OnEraseBackground(wxPaintEvent& event);
//		void OnMouseMotion(wxMouseEvent& event);
		void OnMouseDown(wxMouseEvent& event);
//		void OnMouseUp(wxMouseEvent& event);
		void TipEventMotion(wxPoint point);
		void TipEventLeftUp(wxPoint point);
		void DrawBgCircle(wxDC *dc);
		int GetDialMinSize();
	private:
		void DrawDialTip();
		double CalcAngle(int center_x, int center_y, int pos_x, int pos_y);
		void SetValueFromAngle(double angle);
		void DrawArrow(wxDC *dc, int x_c, int y_c, double angle, int size);
		wxDialTip *m_dialTip;
		int m_Value;
		int m_MinValue;
		int m_MaxValue;
		double m_Angle;
		bool m_leftStuck;
		bool m_rightStuck;
		DECLARE_EVENT_TABLE()
};

#endif
