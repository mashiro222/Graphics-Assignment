#include "PoissonEditing.h"
#include <iostream>
#include <vector>
using namespace Eigen;

PoissonEditing::PoissonEditing() {
}
PoissonEditing::~PoissonEditing() {

}

Solver* PoissonEditing::Get_coeff_Matrix(MatrixXd selected_area) {
	int h = (int)selected_area.rows();
	int w = (int)selected_area.cols();
	int n = h * w;
	SparseMatrix<double> A(n, n);
	std::vector<Triplet<double>> tripletlist;// a faster way to give number to a sparse Matrix


	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int center = i * w + j;
			//if the point doesn't be in the area
			if (selected_area(i, j) == 0) {
				tripletlist.push_back(Triplet<double>(center, center, 1));
			}
			else if (selected_area(i, j) == 1) {
				int sum;
				if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
					sum = 0;
				}
				else {
					sum = selected_area(i - 1, j) + selected_area(i + 1, j) + selected_area(i, j + 1) + selected_area(i, j - 1);
				}
				if (sum == 4) {
					tripletlist.push_back(Triplet<double>(center, center, 4));
					tripletlist.push_back(Triplet<double>(center, center - w, -1));
					tripletlist.push_back(Triplet<double>(center, center + w, -1));
					tripletlist.push_back(Triplet<double>(center, center - 1, -1));
					tripletlist.push_back(Triplet<double>(center, center + 1, -1));
				}
				else {
					tripletlist.push_back(Triplet<double>(center, center, 1));
				}
			}
		}
	}

	A.setFromTriplets(tripletlist.begin(), tripletlist.end());
	A.makeCompressed();//Eigen的压缩列存储格式，能提速
	
	// Output the coefficient matrix A
	//std::cout << "Coefficient Matrix A:\n" << A << std::endl;
	Solver* coeff_Matrix = new Solver(A);
	return coeff_Matrix;
}

VectorXd PoissonEditing::Solve_Poisson(MatrixXd f, MatrixXd g, MatrixXd selected_area, Solver* coeff_Matrix, bool mixed) {
	//f is the origin image,g is the source image
	//right hand side vector
	int h = (int)f.rows();
	int w = (int)f.cols();
	int n = h * w;
	VectorXd b = VectorXd::Zero(n);

	//set b
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int center = i * w + j;
			if (selected_area(i, j) == 0) {
				b(center) = f(i, j);
			}
			else {
				int sum;
				if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
					sum = 0;
				}
				else {
					sum = selected_area(i - 1, j) + selected_area(i + 1, j) + selected_area(i, j + 1) + selected_area(i, j - 1);
				}
				if (sum == 4) {
					if (mixed) {
						std::vector<double> temp(4);
						temp[0] = g(i, j) - g(i + 1, j);
						temp[1] = g(i, j) - g(i - 1, j);
						temp[2] = g(i, j) - g(i, j + 1);
						temp[3] = g(i, j) - g(i, j - 1);
						if (fabs(temp[0]) < fabs(f(i, j) - f(i + 1, j)))
							temp[0] = f(i, j) - f(i + 1, j);
						if (fabs(temp[1]) < fabs(f(i, j) - f(i - 1, j)))
							temp[1] = f(i, j) - f(i - 1, j);
						if (fabs(temp[2]) < fabs(f(i, j) - f(i, j + 1)))
							temp[2] = f(i, j) - f(i, j + 1);
						if (fabs(temp[3]) < fabs(f(i, j) - f(i, j - 1)))
							temp[3] = f(i, j) - f(i, j - 1);
						b(center) = temp[0] + temp[1] + temp[2] + temp[3];
					}
					else {
						b(center) = 4 * g(i, j) - g(i + 1, j) - g(i - 1, j) - g(i, j + 1) - g(i, j - 1);
					}	
				}
				else {
					b(center) = f(i, j);
				}
			}
		}
	}			
	//std::cout << "b: " << b << std::endl;

	//solve the problem
	VectorXd x;
	x = coeff_Matrix->solve(b);
	//std::cout << "x: " << x << std::endl;
	return x;
}

VectorXd PoissonEditing::Poisson_solve(MatrixXd f, MatrixXd g, MatrixXd selected_area) {
	int h = (int)selected_area.rows();
	int w = (int)selected_area.cols();
	int n = h * w;

	SparseMatrix<double> A(n, n);
	std::vector<Triplet<double>> tripletlist;// a faster way to give number to a sparse Matrix

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int center = i * w + j;
			//if the point doesn't be in the area
			if (selected_area(i, j) == 0) {
				tripletlist.push_back(Triplet<double>(center, center, 1));
			}
			else if (selected_area(i, j) == 1) {
				int sum;
				if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
					sum = 0;
				}
				else {
					sum = selected_area(i - 1, j) + selected_area(i + 1, j) + selected_area(i, j + 1) + selected_area(i, j - 1);
				}
				if (sum == 4) {
					tripletlist.push_back(Triplet<double>(center, center, 4));
					tripletlist.push_back(Triplet<double>(center, center - w, -1));
					tripletlist.push_back(Triplet<double>(center, center + w, -1));
					tripletlist.push_back(Triplet<double>(center, center - 1, -1));
					tripletlist.push_back(Triplet<double>(center, center + 1, -1));
				}
				else {
					tripletlist.push_back(Triplet<double>(center, center, 1));
				}
			}
		}
	}
	A.setFromTriplets(tripletlist.begin(), tripletlist.end());
	A.makeCompressed();//Eigen的压缩列存储格式，能提速
	// Output the coefficient matrix A
	//std::cout << "Coefficient Matrix A:\n" << A << std::endl;
	//Solver *coeff_Matrix = new Solver(A);
	Solver coeff_Matrix;
	coeff_Matrix.compute(A);

	VectorXd b = VectorXd::Zero(n);
	//set b
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int center = i * w + j;
			if (selected_area(i, j) == 0) {
				b(center) = f(i, j);
			}
			else {
				int sum;
				if (i == 0 || j == 0 || i == h - 1 || j == w - 1) {
					sum = 0;
				}
				else {
					sum = selected_area(i - 1, j) + selected_area(i + 1, j) + selected_area(i, j + 1) + selected_area(i, j - 1);
				}
				if (sum == 4) {
					b(center) = 4 * g(i, j) - g(i + 1, j) - g(i - 1, j) - g(i, j + 1) - g(i, j - 1);
				}
				else {
					b(center) = f(i, j);
				}
			}
		}
	}
	//std::cout << "b: " << b << std::endl;

	//solve the problem
	VectorXd x = VectorXd::Zero(n);
	x = coeff_Matrix.solve(b);
	//x = coeff_Matrix->solve(b);
	//std::cout << "x: " << x << std::endl;
	return x;
}