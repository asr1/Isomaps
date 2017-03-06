#include "stdafx.h"
#include "Line.h"

Line::Line(int _x, int _y, int _width, int _height, int _thickness)
{
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	thickness = _thickness;
	//if (!_pen) {
	//	_pen = Pen(0, 255, 0);
	//}
	//pen = _pen;
}