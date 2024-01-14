#include<PolyDomain.h>
#include <iostream>
using std::cout;
using std::endl;

bool EdgeType::operator < (EdgeType& e)
{
	return x < e.x;
}

PolyDomain::PolyDomain()
{

}

PolyDomain::PolyDomain(PointList choosed_points)
{
	// Set the vertice list
	vertice_list_ = choosed_points;
	
	if (vertice_list_.empty())
		return;

	// Set the total size rectangle
	int x_min, y_min, x_max, y_max;
	cv::Point tmp_point_;

	x_min = x_max = vertice_list_[0].x;
	y_min = y_max = vertice_list_[0].y;

	for (size_t i = 0; i < vertice_list_.size(); i++)
	{
		tmp_point_ = vertice_list_[i];
		if (tmp_point_.x < x_min)
			x_min = tmp_point_.x;
		if (tmp_point_.x > x_max)
			x_max = tmp_point_.x;
		if (tmp_point_.y < y_min)
			y_min = tmp_point_.y;
		if (tmp_point_.y > y_max)
			y_max = tmp_point_.y;
	}

	total_size_ = cv::Rect(x_min, y_min, x_max - x_min + 1, y_max - y_min + 1);
	
	// Init pointlist and neighbourlist
	getPointList();
	getNeighbourList();
}

PolyDomain::~PolyDomain()
{

}

void PolyDomain::getPointList()
{
	InitNET();
	// Init active edge list
	std::list<Edge> ActiveEdgeList;

	// Scan the polygon row by row
	for (int i = 0; i < NewEdgeList.size(); i++)
	{
		InsertNewEdge(ActiveEdgeList, i);
		UpdateEdge(ActiveEdgeList, i);

		// Find the points in the polygon
		bool fillon = false;
		std::list<Edge>::iterator tmp_iter;
		point_list_.push_back(PointList());

		for (auto iter_AET = ActiveEdgeList.begin(); iter_AET != ActiveEdgeList.end(); iter_AET++)
		{
			if (!fillon)
			{
				tmp_iter = iter_AET;
				fillon = true;
			}
			else
			{
				// Make sure the point is IN the polygon
				for (int j = floor(tmp_iter->x + 1); j < floor(iter_AET->x); j++)
				{
					point_list_[i].push_back(cv::Point(j, i + total_size_.y));
				}
				fillon = false;
			}
		}
	}
}

void PolyDomain::getNeighbourList()
{
	for (size_t i = 0; i < point_list_.size(); i++)
	{
		neighbour_list_.push_back(std::vector<Neighbour>());

		for (size_t j = 0; j < point_list_[i].size(); j++)
		{
			// Store the point and find its neighbour
			cv::Point point = point_list_[i][j];

			neighbour_list_[i].push_back(Neighbour());

			// Get left neighbour
			if (j > 0 && point_list_[i][j - 1].x == point.x - 1)
				neighbour_list_[i][j].interior_point_.push_back(point_list_[i][j - 1]);
			else
				neighbour_list_[i][j].boundary_point_.push_back(cv::Point(point.x - 1, point.y));
			// Get right neighbour
			if (j < point_list_[i].size() - 1 && point_list_[i][j + 1].x == point.x + 1)
				neighbour_list_[i][j].interior_point_.push_back(point_list_[i][j + 1]);
			else
				neighbour_list_[i][j].boundary_point_.push_back(cv::Point(point.x + 1, point.y));
			
			size_t k = 0;
			// Get up neighbour
			for (k = 0; i > 0 && k < point_list_[i - 1].size(); k++)
			{
				if (point_list_[i - 1][k].x == point.x)
				{
					neighbour_list_[i][j].interior_point_.push_back(point_list_[i - 1][k]);
					break;
				}
			}
			if (i == 0 || k == point_list_[i - 1].size())
				neighbour_list_[i][j].boundary_point_.push_back(cv::Point(point.x, point.y - 1));

			// Get down neighbour
			for (k = 0; i < point_list_.size() - 1 && k < point_list_[i + 1].size(); k++)
			{
				if (point_list_[i + 1][k].x == point.x)
				{
					neighbour_list_[i][j].interior_point_.push_back(point_list_[i + 1][k]);
					break;
				}
			}
			if (i == point_list_.size() - 1 || k == point_list_[i + 1].size())
				neighbour_list_[i][j].boundary_point_.push_back(cv::Point(point.x, point.y + 1));
		}
	}
}

void PolyDomain::InitNET()
{
	for (size_t i = 0; i < vertice_list_.size(); i++)
	{
		cv::Point p1, p2;
		Edge newEdge;

		// Set the size of the edge list
		NewEdgeList.resize(total_size_.height);

		// Find two vertices of the edge
		if (i < vertice_list_.size() - 1)
		{
			p1 = vertice_list_[i];
			p2 = vertice_list_[i + 1];
		}
		else
		{
			p1 = vertice_list_[i];
			p2 = vertice_list_[0];
		}

		// Find the HIGHER vertice and set the edge
		if (p1.y > p2.y)
		{
			newEdge.y_max = p1.y - total_size_.y;
			newEdge.x = p2.x;
			newEdge.dx = (float)(p1.x - p2.x) / (float)(p1.y - p2.y);
			NewEdgeList[p2.y - total_size_.y].push_back(newEdge);
		}
		else if (p1.y < p2.y)
		{
			newEdge.y_max = p2.y - total_size_.y;
			newEdge.x = p1.x;
			newEdge.dx = (float)(p2.x - p1.x) / (float)(p2.y - p1.y);
			NewEdgeList[p1.y - total_size_.y].push_back(newEdge);
		}
	}
}

void PolyDomain::InsertNewEdge(std::list<Edge> &ActiveEdgeList, int i)
{
	// Insert new edge 
	for (auto iter_NET = NewEdgeList[i].begin(); iter_NET != NewEdgeList[i].end(); iter_NET++)
	{
		std::list<Edge>::iterator iter_AET;
		for (iter_AET = ActiveEdgeList.begin(); 
			iter_AET != ActiveEdgeList.end() && iter_AET->x < iter_NET->x; iter_AET++);
		ActiveEdgeList.insert(iter_AET, *iter_NET);
	}
}

void PolyDomain::UpdateEdge(std::list<Edge> &ActiveEdgeList, int i)
{
	for (auto iter_AET = ActiveEdgeList.begin(); iter_AET != ActiveEdgeList.end();)
	{
		// Delete old edge
		if (iter_AET->y_max == i)
		{
			iter_AET = ActiveEdgeList.erase(iter_AET);
		}
		// Updete intersections
		else if (iter_AET->y_max > i)
		{
			iter_AET->x += iter_AET->dx;
			iter_AET++;
		}
	}

	// Make sure the order of the intersections
	ActiveEdgeList.sort();
}