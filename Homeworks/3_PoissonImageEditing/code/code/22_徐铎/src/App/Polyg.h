#pragma once
#include <vector>
#include "shape.h"

using namespace std;

class Polyg :public Shape
{
public:
	Polyg();
	~Polyg();

	void Draw(QPainter& painter);


};

