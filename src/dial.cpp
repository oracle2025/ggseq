/* dial.cpp
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
 
#include <math.h>
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/popupwin.h"
#include "dial.h"
#include <iostream>

#define PI 3.1415926535897931

//DEFINE_EVENT_TYPE(wxEVT_MF_ACTIVATE_COMMAND)
/*   wxCommandEvent eventCustom(wxEVT_MY_CUSTOM_COMMAND);

    wxPostEvent(this, eventCustom);*/

class wxDialTip : public wxPopupWindow
{
	public:
			wxDialTip(wxDial* parent);
			void OnMouseMotion(wxMouseEvent& event);
			void OnMouseUp(wxMouseEvent& event);
	private:
			wxDial *m_parentbla;
		DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(wxDialTip, wxPopupWindow)
	EVT_MOTION(wxDialTip::OnMouseMotion)
	EVT_LEFT_UP(wxDialTip::OnMouseUp)
END_EVENT_TABLE()

wxDialTip::wxDialTip(wxDial* parent):wxPopupWindow(parent)
{
	m_parentbla=parent;
}
void wxDialTip::OnMouseMotion(wxMouseEvent& event)
{
	if (event.m_leftDown || event.LeftUp())
		m_parentbla->TipEventMotion(ClientToScreen(wxPoint(event.m_x,event.m_y)));
}
void wxDialTip::OnMouseUp(wxMouseEvent& event)
{
	m_parentbla->TipEventLeftUp(ClientToScreen(wxPoint(event.m_x,event.m_y)));
}


BEGIN_EVENT_TABLE(wxDial, wxControl)
	EVT_PAINT(wxDial::OnPaint)
	EVT_ERASE_BACKGROUND(wxDial::OnEraseBackground)
	EVT_LEFT_DOWN(wxDial::OnMouseDown)
//	EVT_MOTION(wxDial::OnMouseMotion)
//	EVT_LEFT_UP(wxDial::OnMouseUp)
END_EVENT_TABLE()

bool wxDial::Create(wxWindow* parent,
				wxWindowID id, int value ,
				int minValue, int maxValue,
				const wxPoint& pos,
				const wxSize& size,
				long style,
				const wxString& name)
{
	bool RetVal = wxControl::Create(parent, id, pos, size, style|wxNO_BORDER);
	m_Value = value;
	m_MinValue = minValue;
	m_MaxValue = maxValue;

	m_Angle = 0;
	m_leftStuck = false;
	m_rightStuck = false;
	m_dialTip=new wxDialTip(this);
	SetValue(m_Value);
	return RetVal;
}

wxDial::~wxDial()
{
}

void wxDial::SetValue(int value)
{
	m_Value = value;
	m_Angle = ((double)(m_Value-m_MinValue)/(double)(m_MaxValue-m_MinValue))*(PI/2 + PI);
	Refresh(true);
}

void wxDial::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc_screen(this);
	wxMemoryDC dc;
	wxBitmap bmp1(GetSize().GetWidth(),GetSize().GetHeight());
	dc.SelectObject(bmp1);

	DrawBgCircle(&dc);
	int center_x = GetSize().GetWidth()/2;
	int center_y = GetSize().GetHeight()/2;
	int siz_1_8 = GetDialMinSize()/8;

	DrawArrow(&dc, center_x,center_y, m_Angle, siz_1_8*3);
	dc_screen.Blit(0,0,GetSize().GetWidth(),GetSize().GetHeight() , &dc, 0,0);
}

void wxDial::OnEraseBackground(wxEraseEvent& event)
{
}

/* Malt einen Zeiger, im angegebenen Winkel in den DC, x_c, und y_c geben den Mittelpunkt an, size die Größe*/
void wxDial::DrawArrow(wxDC *dc, int x_c, int y_c, double angle, int size)
{
	double _angle = -angle+PI*(3.0/4.0);
	int x = (x_c-(int)(sin(_angle)*size));
	int y = (y_c-(int)(cos(_angle)*size));
	
/*	wxPoint points[3];
	points[0]=wxPoint(x,y);
	points[1]=wxPoint((int)(x_c-(sin(_angle+2.7925268031909272)*(size-5))),(int)(y_c+(cos(_angle+2.7925268031909272)*(size-5))));
	points[2]=wxPoint((int)(x_c-(sin(_angle-2.7925268031909272)*(size-5))),(int)(y_c+(cos(_angle-2.7925268031909272)*(size-5))));

	dc->SetBrush(wxBrush( wxColour(126,126,126),wxSOLID));
	dc->DrawPolygon(3,points);*/
	dc->SetPen(wxPen(wxColour(126,126,126),size/3,wxSOLID));
	dc->DrawLine(x_c,y_c,x,y);
	dc->SetPen(wxPen(*wxBLACK,1,wxTRANSPARENT));
	dc->SetBrush(wxBrush( *wxRED,wxSOLID));
	dc->DrawCircle(x,y,size/4);
}
void wxDial::TipEventMotion(wxPoint point)
{
	int mx_value = point.x;
	int my_value = point.y;
	wxPoint xy = ScreenToClient(wxPoint(point.x,point.y));
	mx_value = xy.x;
	my_value = xy.y;
	
	m_Angle=CalcAngle(GetSize().GetWidth()/2,GetSize().GetHeight()/2,mx_value,my_value);
	
	if (!(m_leftStuck && m_Angle>PI)) {
		m_leftStuck=false;
	}
	if (!(m_rightStuck && (m_Angle<M_PI_2 || m_Angle>M_PI_2+M_PI))) {
		m_rightStuck=false;
	}

	if ((m_Angle>PI/2+PI)&& !(m_leftStuck||m_rightStuck)) {
		if (m_Angle>3*PI/4+PI) {
			m_Angle = 0;
			m_leftStuck=true;
		}
		else {
			m_Angle = PI/2+PI;
			m_rightStuck=true;
		}
	}
	if (m_leftStuck) {
		m_Angle=0;
	} else if (m_rightStuck) {
		m_Angle=M_PI_2+M_PI;
	}

	SetValueFromAngle(m_Angle);
	DrawDialTip();
	Refresh(0);
	
	wxScrollEvent scrEvent(wxEVT_SCROLL_THUMBTRACK,GetId(),m_Value);
	wxPostEvent(this,scrEvent);
	
}
void wxDial::TipEventLeftUp(wxPoint point)
{
	m_dialTip->ReleaseMouse();
	m_dialTip->Hide();
}
void wxDial::OnMouseDown(wxMouseEvent& event)
{
	wxPoint pos=ClientToScreen(wxPoint(0,0));
	m_dialTip->SetSize(pos.x,pos.y+GetSize().GetHeight(),40,15);
	m_dialTip->Show();
	m_dialTip->CaptureMouse();
	DrawDialTip();
}
void wxDial::DrawDialTip() /*die events motion und left-up mussen verfolgt werden*/
{
	wxClientDC dc(m_dialTip);
	dc.DrawRectangle(0,0,40,15);
	wxString gg;
	gg << m_Value;
	dc.DrawText(gg,2,0);
}
double wxDial::CalcAngle(int center_x, int center_y, int pos_x, int pos_y)
{
	int x = pos_x-center_x;
	int y = pos_y-center_y;
	double rad = sqrt(pow(x,2)+pow(y,2));
	double angle;
	if (rad) {
		angle = acos(x/rad);
	} else {
		angle = 0;
	}
	if (y > 0) {
		angle = 2*PI-angle;
	}
	angle = -(PI*(3.0/4.0)+angle);
	while (angle<0) {
		angle=angle+2*PI;
	}
	return angle;
}
void wxDial::SetValueFromAngle(double angle)
{
	m_Value = m_MinValue+(int)(((m_MaxValue-m_MinValue)*angle)/(PI/2+PI));
}
void wxDial::DrawBgCircle(wxDC *dc)
{
	dc->SetBrush(wxBrush(GetParent()->GetBackgroundColour(),wxSOLID));
	dc->SetPen(wxPen(*wxBLACK,1,wxTRANSPARENT));
	dc->DrawRectangle(0,0,GetSize().GetWidth(),GetSize().GetHeight());

	dc->SetBrush(wxBrush(wxColour(73,73,73),wxSOLID));
	int siz_1_8 =GetDialMinSize()/8;

	int bottom = GetSize().GetHeight()/2+4*siz_1_8;
	int center_x = GetSize().GetWidth()/2;
	int center_y = GetSize().GetHeight()/2;

	dc->DrawArc(GetSize().GetWidth()/2+2*siz_1_8,bottom,GetSize().GetWidth()/2-2*siz_1_8,bottom,center_x,center_y);
	dc->SetBrush(wxBrush(GetParent()->GetBackgroundColour(),wxSOLID));
	dc->DrawCircle(GetSize().GetWidth()/2,GetSize().GetHeight()/2,3*siz_1_8);
}
int wxDial::GetDialMinSize()
{
	if (GetSize().GetWidth()>GetSize().GetHeight()) {
		return GetSize().GetHeight();
	} else {
		return GetSize().GetWidth();
	}
}

