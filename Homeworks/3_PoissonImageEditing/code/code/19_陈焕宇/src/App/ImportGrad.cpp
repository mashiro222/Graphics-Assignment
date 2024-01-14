#include "ImportGrad.h"
#include <iostream>
#include <Eigen\Sparse>
#include <Eigen\SparseLU>

ImportGrad::ImportGrad(int width, int height, vector<Vector2i>& vertices,
	MatrixXd source_value, MatrixXd target_value) :
	PoissonSolver(width, height, source_value, target_value)
{
	ScanLine(vertices);

	Solve();
}

ImportGrad::~ImportGrad()
{

}

void	ImportGrad::Solve()
{
	SparseMatrix<double, RowMajor> A(point_count_, point_count_);
	VectorXd b(point_count_);
	for (int i = 0; i < height_; i++)
	{
		for (int j = 0; j < width_; j++)
		{
			if (status_(i, j) != kExterior)
			{
				A.insert(index_(i, j), index_(i, j)) = 4;
				if ((i == 0) || (i == height_ - 1))
				{
					A.coeffRef(index_(i, j), index_(i, j))--;
				}
				if ((j == 0) || (j == width_ - 1))
				{
					A.coeffRef(index_(i, j), index_(i, j))--;
				}
				b(index_(i, j)) = 0;

				// neighborhood
				int row_array[4] = { 0, 0, -1, 1 };
				int column_array[4] = { -1, 1, 0, 0 };
				for (int n = 0; n < 4; n++)
				{
					int row = i + row_array[n];
					int column = j + column_array[n];
					if ((row < 0) || (row > height_ - 1))
					{
						continue;
					}
					if ((column < 0) || (column > width_ - 1))
					{
						continue;
					}

					b(index_(i, j)) += NonlinearTerm(i, j, row, column);
					if (status_(row, column) == kInterior)
					{
						A.insert(index_(i, j), index_(row, column)) = -1;
					}
					else
					{
						b(index_(i, j)) += target_value_(row, column);
					}
				}
			}
		}
	}
	SparseLU<SparseMatrix<double, RowMajor>> solver;
	solver.compute(A);
	if (solver.info() != Success)
	{
		cout << "Compute Matrix is error" << endl;
		return;
	}
	solution_ = solver.solve(b);
}

double	ImportGrad::NonlinearTerm(int p_row, int p_column, int q_row, int q_column)
{
	return source_value_(p_row, p_column) - source_value_(q_row, q_column);
}