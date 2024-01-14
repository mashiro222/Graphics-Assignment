#pragma once

#include "PoissonSolver.h"

class MixGrad :
	public PoissonSolver
{
public:
	MixGrad(int width, int height, vector<Vector2i>& vertices, 
		MatrixXd source_value, MatrixXd target_value);
	~MixGrad();

private:
	void Solve();

protected:
	double NonlinearTerm(int p_row, int p_column, int q_row, int q_column);
};

