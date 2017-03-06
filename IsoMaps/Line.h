#pragma once
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")

class Line
{


public:

	int width, height, thickness, x, y;
	bool isSelected;
	Gdiplus::Color color;

	Line();
	Line(int _x, int _y, int _width, int _height, int _thickness = 1 );
	Line(int _x, int _y, int _width, int _height, Gdiplus::Color _color, int _thickness = 1);
private:
	void init(int _x, int _y, int _width, int _height, Gdiplus::Color _color, int _thickness);

};