#include "NormalCopy.h"
#include <iostream>
#include <Eigen\Sparse>
#include <Eigen\SparseLU>

NormalCopy::NormalCopy(int width, int height, vector<Vector2i>& vertices,
	MatrixXd source_value, MatrixXd target_value) :
	PoissonSolver(width, height, source_value, target_value)
{
	ScanLine(vertices);

	cout << "count:\t" << point_count_ << endl;

	solution_.resize(point_count_);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (status_(i, j) != kExterior)
			{
				solution_(index_(i, j)) = source_value_(i, j);
			}
		}
	}

}

NormalCopy::~NormalCopy()
{

}
