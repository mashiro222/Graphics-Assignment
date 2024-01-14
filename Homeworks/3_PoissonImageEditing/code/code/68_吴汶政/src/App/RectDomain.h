#pragma once

#include "Domain.h"

class RectDomain : public Domain
{
public:
	RectDomain();
	RectDomain(cv::Point start_point_, cv::Point end_point_);		// Create a Rectangle with two vertices
	~RectDomain();

	int PointStatus(cv::Point point_);								// Relationship between a point and the domain
private:
	void getPointList();
	void getNeighbourList();

	cv::Rect rect_domain_;
};