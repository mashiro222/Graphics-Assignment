#include "PoissonSolver.h"
#include <iostream>
#include <Eigen\Sparse>

PoissonSolver::PoissonSolver()
{

}

PoissonSolver::PoissonSolver(int width, int height, MatrixXd source_value, MatrixXd target_value):
	width_(width), height_(height), source_value_(source_value), target_value_(target_value)
{
	status_.resize(height, width);
	index_.resize(height, width);

	// initialize status_, index_ matrix
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			status_(i, j) = kExterior;
			index_(i, j) = -1;
		}
	}

	point_count_ = 0;
}

PoissonSolver::~PoissonSolver()
{

}


double PoissonSolver::SolutionAt(int x, int y)
{
	int index = index_(y, x);
	if ((index >= 0) && (index < point_count_))
	{
		return solution_(index);
	}
	else
	{
		return target_value_(y, x);
	}
}


double	PoissonSolver::NonlinearTerm(int p_row, int p_column, int q_row, int q_column)
{
	return 0;
}

void	PoissonSolver::ScanLine(vector<Vector2i>& vertices)
{
	list<Edge> edge_list = list<Edge>();
	for (int i = 0; i < height_; i++)
	{
		int y = i;
		// update and delete edges
		list<Edge>::iterator itr = edge_list.begin();
		while (itr != edge_list.end())
		{
			if (y > itr->ymax)
			{
				itr = edge_list.erase(itr);
			}
			else
			{
				itr->x += itr->dx;
				itr++;
			}
		}

		// insert new edges
		for (int j = vertices.size() - 1, k = 0; k < vertices.size(); j = k, k++)
		{
			if (((vertices[j](1) == y) && (vertices[k](1) > y))
				|| ((vertices[j](1) > y) && (vertices[k](1) == y)))
			{
				Edge edge;
				edge.dx = ((double)(vertices[j](0) - vertices[k](0))) / (vertices[j](1) - vertices[k](1));
				if (vertices[j](1) == y)
				{
					edge.x = vertices[j](0);
					edge.ymax = vertices[k][1];

					// left or right vertex
					int l = ((k == vertices.size() - 1) ? 0 : k + 1);
					if (vertices[l](1) > vertices[k](1))
					{
						edge.ymax--;
					}
				}
				else
				{
					edge.x = vertices[k](0);
					edge.ymax = vertices[j][1];

					// left or right vertex
					int l = ((j == 0) ? vertices.size() - 1 : j - 1);
					if (vertices[l](1) > vertices[j](1))
					{
						edge.ymax--;
					}
				}

				list<Edge>::iterator itr = edge_list.begin();
				while (itr != edge_list.end())
				{
					if (itr->x < edge.x)
					{
						itr++;
					}
					else
					{
						break;
					}
				}
				edge_list.insert(itr, edge);
			}
		}

		list<Edge>::iterator itr1 = edge_list.begin();
		for (int k = 0; k < edge_list.size() / 2; k++)
		{
			list<Edge>::iterator itr2 = itr1;
			itr2++;
			for (int j = (int)itr1->x; j <= (int)itr2->x; j++)
			{
				index_(i, j) = point_count_;
				point_count_++;
				if ((j == (int)itr1->x) || (j == (int)itr2->x))
				{
					status_(i, j) = kBoundary;
				}
				else
				{
					status_(i, j) = kInterior;
				}
			}
			itr1++;
			itr1++;
		}
	}

	// flat edge correction
	for (int i = vertices.size() - 1, j = 0; j < vertices.size(); i = j, j++)
	{
		if (vertices[i](1) == vertices[j](1))
		{
			for (int k = min(vertices[i](0), vertices[j](0)); k <= max(vertices[i](0), vertices[j](0)); k++)
			{
				if (index_(vertices[i](1), k) != -1)
				{
					status_(vertices[i](1), k) = kBoundary;
				}
			}
		}
	}
}