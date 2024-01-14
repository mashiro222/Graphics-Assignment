#pragma once
#ifndef _WARP_RBF_H_
#define _WARP_RBF_H_

#include "Warping.h"
#include <Eigen/Dense> 

using Eigen::MatrixXf; 
using std::vector;

class Warping_RBF :
	public Warping
{
public:
	Warping_RBF();
	Warping_RBF(Warping IW);
	~Warping_RBF();

	void InitWarp();
	MatrixXf WarpingIJ(int i, int j);

private:
	vector<MatrixXf> cpAP;
	vector<MatrixXf> cpAQ;
	vector<MatrixXf> cpRP;
	vector<MatrixXf> cpRQ;
	MatrixXf Ax;
	MatrixXf Ay;
	MatrixXf M;
	MatrixXf b ;
	float* r2;
};

#endif;