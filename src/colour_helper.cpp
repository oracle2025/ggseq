#include <math.h>
#include <wx/colour.h>
#include "colour_helper.h"

float MAX(float a, float b, float c)
{
	if (a>=b && a>=c)
		return a;
	if (b>=a && b>=c)
		return b;
	return c;
}
float MIN(float a, float b, float c)
{
	if (a<=b && a<=c)
		return a;
	if (b<=a && b<=c)
		return b;
	return c;
}
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = MIN( r, g, b );
	max = MAX( r, g, b );
	*v = max; // v
	delta = max - min;
	if( max != 0 )
		*s = delta / max; // s
	else {
		// r = g = b = 0 // s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max ) {
		*h = ( g - b ) / delta; // between yellow & magenta
	} else if( g == max ) {
		*h = 2 + ( b - r ) / delta; // between cyan & yellow
	} else {
		*h = 4 + ( r - g ) / delta; // between magenta & cyan
	}
	*h *= 60; // degrees
	if( *h < 0 )
		*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60; // sector 0 to 5
	i = (int)floor( h );
	f = h - i; // factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default: // case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}


wxColour GetDarkColour(wxColour colour)
{
	float H, S, V;
	float R, G, B;
	R=((float)colour.Red())/256.0;
	G=((float)colour.Green())/256.0;
	B=((float)colour.Blue())/256.0;
	RGBtoHSV(R,G,B,&H,&S,&V);
	V-=0.2;
	if (V<0)
		V=0;
	HSVtoRGB(&R,&G,&B,H,S,V);
	wxColour result((unsigned char)(R*255),(unsigned char)(G*255),(unsigned char)(B*255));
	return result;
}
wxColour GetLightColour(wxColour colour)
{
	float H, S, V;
	RGBtoHSV(((float)colour.Red())/256.0,((float)colour.Green())/256.0,((float)colour.Blue())/256.0,&H,&S,&V);
	S-=0.3;
	if (S<0)
		S=0;
	float R,G,B;
	HSVtoRGB(&R,&G,&B,H,S,V);
	wxColour result((unsigned char)(R*255),(unsigned char)(G*255),(unsigned char)(B*255));
	return result;
}

