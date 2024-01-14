#pragma once 

#include <vector>
#include <qpainter.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

typedef std::vector<cv::Point> PointList;

class Domain
{
public:
	Domain();
	virtual ~Domain();

	int getWidth();														// Width of the rigion
	int getHeight();													// Height of the rigion
	size_t getSize();													// Number of the points in the domain
	size_t getOrd(int i, int j);										// Order of the point
	int getOrd(cv::Point);												// Order of the point
	cv::Point Translate(cv::Point point, cv::Point target_point_);		// Find the absolute position in the desitination image

	typedef struct {
		PointList interior_point_;
		PointList boundary_point_;
	}Neighbour;															// Neighbour of a point
	std::vector<PointList>	point_list_;
	std::vector<std::vector<Neighbour>> neighbour_list_;

protected:
	virtual void getPointList() = 0;
	virtual void getNeighbourList() = 0;

	cv::Rect	total_size_;
};