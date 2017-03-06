#pragma once
//#include <gdiplus.h>

class Line
{
	int width, height, thickness, x, y;
	bool isSelected;
	//Gdiplus::Pen pen;

public:
	Line(int _x, int _y, int _width, int _height, int _thickness = 1);
};