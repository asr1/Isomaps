#include "stdafx.h"
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#include "Line.h"

Line::Line(int _x, int _y, int _width, int _height, Gdiplus::Color _color, int _thickness)
{
	Line::init(_x, _y, _width, _height, _color, _thickness);
}

Line::Line() {
	Line::init(0, 0, 0, 0, Gdiplus::Color(255, 255, 255), 0);
}

Line::Line(int _x, int _y, int _width, int _height, int _thickness)
{
	Line::init(_x, _y, _width, _height, Gdiplus::Color(0,0,0),_thickness);
}

void Line::init(int _x, int _y, int _width, int _height, Gdiplus::Color _color, int _thickness)
{
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	thickness = _thickness;
	color = _color;
}