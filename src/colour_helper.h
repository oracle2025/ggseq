#ifndef _COLOUR_HELPER_H_
#define _COLOUR_HELPER_H_

float MAX(float a, float b, float c);
float MIN(float a, float b, float c);
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
wxColour GetDarkColour(wxColour colour);
wxColour GetLightColour(wxColour colour);

#endif /*_COLOUR_HELPER_H_*/
