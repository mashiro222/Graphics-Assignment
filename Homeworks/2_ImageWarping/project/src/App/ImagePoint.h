#pragma once
#include<map>
#include<vector>

class ImagePoint {
public:
	ImagePoint();
	ImagePoint(const int x0, const int y0) : x(x0), y(y0) {}

	int& X();
	int& Y();
	ImagePoint& setX(const int& x0);
	ImagePoint& setY(const int& y0);


//����point��x,y����
private:
	int x;
	int y;
};

//����������
using IPoint = ImagePoint;
using IMap = std::pair<IPoint, IPoint>;