#include "Poisson.h"
#include <iostream>
#include <Eigen\Sparse>
#include <Eigen\Dense>

using namespace std;
typedef Eigen::SparseMatrix<double> SparseMatrixType;
typedef Eigen::Triplet<double> T;
typedef Eigen::SimplicialCholesky<SparseMatrixType> Solver;

Poisson::Poisson()
{
}

Poisson::~Poisson()
{
}

Solver* Poisson::Get_Poisson_solver( MatrixXd selected)
{
	int height = (int)selected.rows();
	int width = (int)selected.cols();
	int N = height * width;
	SparseMatrix<double> A(N, N);
	vector<T> tripletlist;

	// compute the coefficient matrix
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int center = i * width + j;
			if (selected(i, j) == 0)
			{
				tripletlist.push_back(T(center, center, 1));
			}
			else
			{
				tripletlist.push_back(T(center, center, 4));
				if (selected(i - 1, j) == 1)
					tripletlist.push_back(T(center, center - width, -1));

				if (selected(i + 1, j) == 1)
					tripletlist.push_back(T(center, center + width, -1));

				if (selected(i, j - 1) == 1)
					tripletlist.push_back(T(center, center - 1, -1));

				if (selected(i, j + 1) == 1)
					tripletlist.push_back(T(center, center + 1, -1));
			}
		}
	}
	A.setFromTriplets(tripletlist.begin(), tripletlist.end());
	A.makeCompressed();

	// get the solver for A
	Solver* p_A = new Solver(A);
	return p_A;
}

VectorXd Poisson::Use_Poisson_solver(MatrixXd Data_f, MatrixXd Data_g, MatrixXd selected , Solver* p_A,bool mixed)
{
	int height = (int)Data_f.rows();
	int width = (int)Data_f.cols();
	int N = height * width;
	VectorXd b = VectorXd::Zero(N);

	// compute the gradient
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int center = i * width + j;
			if (selected(i, j) == 0)
			{
				b(center) = Data_f(i, j);
			}
			else
			{
				if (selected(i - 1, j) != 1)
					b(center) += Data_f(i - 1, j);

				if (selected(i + 1, j) != 1)
					b(center) += Data_f(i + 1, j);

				if (selected(i, j - 1) != 1)
					b(center) += Data_f(i, j - 1);

				if (selected(i, j + 1) != 1)
					b(center) += Data_f(i, j + 1);

				if (mixed)				// is mixed?
				{
					vector<double> tmp(4);
					tmp[0] = Data_g(i, j) - Data_g(i + 1, j);
					tmp[1] = Data_g(i, j) - Data_g(i - 1, j);
					tmp[2] = Data_g(i, j) - Data_g(i, j + 1);
					tmp[3] = Data_g(i, j) - Data_g(i, j - 1);
					if (fabs(tmp[0]) < fabs(Data_f(i, j) - Data_f(i + 1, j)))tmp[0] = Data_f(i, j) - Data_f(i + 1, j);
					if (fabs(tmp[1]) < fabs(Data_f(i, j) - Data_f(i - 1, j)))tmp[1] = Data_f(i, j) - Data_f(i - 1, j);
					if (fabs(tmp[2]) < fabs(Data_f(i, j) - Data_f(i, j + 1)))tmp[2] = Data_f(i, j) - Data_f(i, j + 1);
					if (fabs(tmp[3]) < fabs(Data_f(i, j) - Data_f(i, j - 1)))tmp[3] = Data_f(i, j) - Data_f(i, j - 1);
					b(center) += tmp[0] + tmp[1] + tmp[2] + tmp[3];
				}
				else
				{
					b(center) += 4 * Data_g(i, j) - Data_g(i + 1, j)
						- Data_g(i - 1, j) - Data_g(i, j + 1) - Data_g(i, j - 1);
				}
			}
		}
	}
	VectorXd x;
	x = p_A->solve(b);
	return x;
}

// combine two functions before (no mixed)
VectorXd Poisson::Poisson_solve(MatrixXd Data_f, MatrixXd Data_g, MatrixXd selected)
{
	int height = (int)Data_g.rows();
	int width = (int)Data_g.cols();
	int N = height * width;
	SparseMatrix<double> A(N, N);
	VectorXd b=VectorXd::Zero(N);
	vector<T> tripletlist;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int center = i * width + j;
			if (selected(i, j) == 0)
			{
				b(center) = Data_f(i, j);
				tripletlist.push_back(T(center, center, 1));
			}
			else
			{
				tripletlist.push_back(T(center, center, 4));
				if (selected(i - 1, j) == 1)
					tripletlist.push_back(T(center, center - width, -1));
				else
					b(center) += Data_f(i - 1, j);

				if (selected(i + 1, j) == 1)
					tripletlist.push_back(T(center, center + width, -1));
				else
					b(center) += Data_f(i + 1, j);

				if (selected(i, j - 1) == 1)
					tripletlist.push_back(T(center, center - 1, -1));
				else
					b(center) += Data_f(i, j - 1);

				if (selected(i, j + 1) == 1)
					tripletlist.push_back(T(center, center + 1, -1));
				else
					b(center) += Data_f(i, j + 1);

				b(center) += 4 * Data_g(i, j) - Data_g(i + 1, j) 
					- Data_g(i - 1, j) - Data_g(i, j + 1) - Data_g(i, j - 1);
			}
		}
	}
	A.setFromTriplets(tripletlist.begin(), tripletlist.end());
	A.makeCompressed();
	VectorXd x;
	Solver* p_A = new Solver(A);
	x = p_A->solve(b);
	delete p_A;
	return x;
}
