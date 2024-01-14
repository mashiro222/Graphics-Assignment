#pragma once
#include <Eigen\Sparse>
#include <Eigen\Dense>
#include <Eigen\SparseCholesky>
#include<Eigen\SparseLU>

using namespace Eigen;
//typedef SimplicialCholesky<SparseMatrix<double>> Solver;
//typedef SimplicialLLT<SparseMatrix<double>> Solver;
typedef SparseLU<SparseMatrix<double>> Solver;

class PoissonEditing {
public:
	PoissonEditing();
	~PoissonEditing();
	//get the coefficient Matrix
	Solver* Get_coeff_Matrix(MatrixXd selected_area);
	//solve the Poisson Editing problem
	VectorXd Solve_Poisson(MatrixXd f, MatrixXd g, MatrixXd selected_area, Solver* coeff_Matrix, bool mixed);

	VectorXd Poisson_solve(MatrixXd f, MatrixXd g, MatrixXd selected_area);
};