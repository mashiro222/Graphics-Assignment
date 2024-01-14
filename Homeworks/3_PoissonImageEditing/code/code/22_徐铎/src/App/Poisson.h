#pragma once
#include <Eigen\Sparse>
#include <Eigen\Dense>
using namespace Eigen;
typedef Eigen::SparseMatrix<double> SparseMatrixType;
typedef Eigen::Triplet<double> T;
typedef Eigen::SimplicialCholesky<SparseMatrixType> Solver;
 
class Poisson
{
public:
	Poisson();
	~Poisson();
	VectorXd Poisson_solve(MatrixXd Data_f, MatrixXd Data_g, MatrixXd selected);			// finish Poisson method
	Solver* Get_Poisson_solver( MatrixXd selected);											// get Poisson solver 
	VectorXd Use_Poisson_solver(MatrixXd Data_f, MatrixXd Data_g, MatrixXd selected, Solver* p_A, bool mixed);		// use Poisson solver to slove the equation
};
