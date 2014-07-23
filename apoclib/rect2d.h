#pragma once


namespace ApocRes {

template <typename t>
class Point2d
{
public:
	t x,y;
	Point2d(t x, t y) : x(x),y(y){};
};

template <typename t>
class Rect2d
{
public:
	Point2d<t> p0, p1;
	Rect2d(Point2d<t> p0, Point2d<t> p1) : p0(p0), p1(p1){};
};

}; //namespace ApocRes
