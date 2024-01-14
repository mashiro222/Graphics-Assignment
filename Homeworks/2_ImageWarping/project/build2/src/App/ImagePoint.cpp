#include "ImagePoint.h"

ImagePoint::ImagePoint(){
}



int& ImagePoint::X() {
	return x;
}
int& ImagePoint::Y() {
	return y;
}
ImagePoint& ImagePoint::setX(const int& x0) {
	x = x0;
	return *this;
}
ImagePoint& ImagePoint::setY(const int& y0) {
	y = y0;
	return *this;
}