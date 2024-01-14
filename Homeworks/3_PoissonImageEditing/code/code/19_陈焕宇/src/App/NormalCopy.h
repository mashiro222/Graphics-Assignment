#pragma once

#include "PoissonSolver.h"

class NormalCopy :
	public PoissonSolver
{
public:
	NormalCopy(int width, int height, vector<Vector2i>& vertices, 
		MatrixXd source_value, MatrixXd target_value);
	~NormalCopy();

};

