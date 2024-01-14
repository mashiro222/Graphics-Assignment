#pragma once

#include "PoissonSolver.h"

class ImportGrad :
	public PoissonSolver
{
public:
	ImportGrad(int width, int height, vector<Vector2i>& vertices, 
		MatrixXd source_value, MatrixXd target_value);
	~ImportGrad();

private:
	void Solve();

protected:
	double NonlinearTerm(int p_row, int p_column, int q_row, int q_column);
};

