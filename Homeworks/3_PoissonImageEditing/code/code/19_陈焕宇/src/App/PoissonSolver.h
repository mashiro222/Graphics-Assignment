#pragma once

#include <vector>
#include <Eigen\Dense>

using namespace std;
using namespace Eigen;

enum PointStatus
{
	kExterior,
	kBoundary,
	kInterior
};

typedef Matrix<PointStatus, Dynamic, Dynamic> MatrixXs;

class PoissonSolver
{
public:
	PoissonSolver();
	PoissonSolver(int width, int height, 
		MatrixXd source_value, MatrixXd target_value);
	virtual ~PoissonSolver();

public:
	double SolutionAt(int x, int y);			// solution at given row and column
	
protected:
	void	ScanLine(vector<Vector2i>& vertices);										// the scan line algorithm for polygon rasterization
	virtual double NonlinearTerm(int p_row, int p_column, int q_row, int q_column);		// the nonlinear term of the guidance vector field

protected:
	int			width_ = 0;						// the width of the envelope rectangle
	int			height_ = 0;					// the height of the envelope rectngle
	int			point_count_ = 0;				// the number of pixels in the region
	MatrixXs	status_ = MatrixXs();			// the ststus matrix of the pixels
	MatrixXi	index_ = MatrixXi();			// the index matrix of the pixels
	MatrixXd	source_value_ = MatrixXd();		// the value in the source image
	MatrixXd	target_value_ = MatrixXd();		// the value in the target image
	VectorXd	solution_ = VectorXd();			// the vector for dolution

protected:
	struct Edge
	{
		double x = 0;
		double dx = 0;
		int ymax = 0;
	};
};

