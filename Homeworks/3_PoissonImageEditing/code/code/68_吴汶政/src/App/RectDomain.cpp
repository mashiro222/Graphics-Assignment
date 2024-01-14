#include "RectDomain.h"

RectDomain::RectDomain()
{

}

RectDomain::RectDomain(cv::Point start_point_, cv::Point end_point_)
{
	rect_domain_ = total_size_ = cv::Rect(start_point_, end_point_);
	getPointList();
	getNeighbourList();
}

RectDomain::~RectDomain()
{

}

void RectDomain::getPointList()
{
	int	width	=	rect_domain_.width - 2;
	int height	=	rect_domain_.height - 2;

	for (int i = 0; i < height; i++)
	{
		PointList tmp;
		for (int j = 0; j < width; j++)
		{
			tmp.push_back(cv::Point(j + total_size_.x + 1, i + total_size_.y + 1));
		}
		point_list_.push_back(tmp);
	}
}

void RectDomain::getNeighbourList()
{
	for (size_t i = 0; i < point_list_.size(); i++)
	{
		neighbour_list_.push_back(std::vector<Neighbour>());

		for (size_t j = 0; j < point_list_[i].size(); j++)
		{
			int	x = point_list_[i][j].x;
			int	y = point_list_[i][j].y;

			// Four neighbours of a point in the rectangle
			std::vector<cv::Point> tmp_list_ =
			{ cv::Point(x, y - 1), cv::Point(x, y + 1), cv::Point(x - 1, y), cv::Point(x + 1, y) };

			neighbour_list_[i].push_back(Neighbour());

			// Find the relationship between the neighbours and the domain
			for (size_t k = 0; k < 4; k++)
			{
				if (PointStatus(tmp_list_[k]) == 1)
					neighbour_list_[i][j].interior_point_.push_back(tmp_list_[k]);
				else if (PointStatus(tmp_list_[k]) == 2)
					neighbour_list_[i][j].boundary_point_.push_back(tmp_list_[k]);
			}
		}
	}
}

int RectDomain::PointStatus(cv::Point point_)
{
	int	x = point_.x;
	int	y = point_.y;

	// IN 
	if (x > rect_domain_.x && y > rect_domain_.y 
		&& x < total_size_.x + total_size_.width - 1 && y < total_size_.y + total_size_.height - 1)
		return 1;
	// ON
	else if (x == rect_domain_.x || y == rect_domain_.y
		|| x == total_size_.x + total_size_.width - 1 || y == total_size_.y + total_size_.height - 1)
		return 2;
	// OUT
	else
		return 0;
}