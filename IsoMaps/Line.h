#pragma once
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")

#define DEFAULT_THICKNESS 1

enum LINE_TYPE {
	NONE,
	HORZ,
	VERT
};

class Line
{


public:

	int width, height, thickness, x, y;
	bool isSelected;
	Gdiplus::Color color;
	LINE_TYPE tpye;

	Line();
	Line(int _x, int _y, int _width, int _height, int _thickness = DEFAULT_THICKNESS, LINE_TYPE type = NONE);
	Line(int _x, int _y, int _width, int _height, Gdiplus::Color _color, int _thickness = DEFAULT_THICKNESS, LINE_TYPE type = NONE);
private:
	void init(int _x, int _y, int _width, int _height, Gdiplus::Color _color, int _thickness, LINE_TYPE _type = NONE);

};