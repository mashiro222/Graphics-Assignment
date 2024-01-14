#include "Domain.h"

Domain::Domain()
{

}

Domain::~Domain()
{

}

int Domain::getHeight()
{
	return total_size_.height;
}

int Domain::getWidth()
{
	return total_size_.width;
}

size_t Domain::getSize()
{
	size_t size = 0;

	for (int i = 0; i < point_list_.size(); i++)
		size += point_list_[i].size();

	return size;
}

size_t Domain::getOrd(int i, int j)
{
	size_t ord = 0;

	for (int k = 0; k < i; k++)
		ord += point_list_[k].size();

	ord += j;
	return ord;
}

int Domain::getOrd(cv::Point point)
{
	int row = point.y - point_list_[0][0].y;
	int ord = 0;

	for (int i = 0; i < row; i++)
	{
		ord += point_list_[i].size();
	}

	for (int col = 0; col < point_list_[row].size(); col++)
	{
		if (point_list_[row][col].x == point.x)
		{
			return ord;
		}
		ord++;
	}

	return -1;
}

cv::Point Domain::Translate(cv::Point point, cv::Point target_point_)
{
	return cv::Point(point.x - total_size_.x + target_point_.x, 
		point.y - total_size_.y + target_point_.y);
}