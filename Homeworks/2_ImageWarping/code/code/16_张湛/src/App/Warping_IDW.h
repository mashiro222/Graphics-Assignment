#pragma once
#ifndef _WARP_IDW_H_
#define _WARP_IDW_H_

#include "Warping.h"
#include <Eigen/Dense> 

using Eigen::MatrixXf;
using std::vector;

class Warping_IDW :
	public Warping
{
public:
	Warping_IDW();
	Warping_IDW(Warping IW);
	~Warping_IDW();

	void DrawCP(QPainter &painter, int dw = 0, int dh = 0);
	void InitWarp();
	MatrixXf WarpingIJ(int i, int j);

private:
	vector<MatrixXf> cpP;
	vector<MatrixXf> cpQ;
	vector<MatrixXf> T;
};

#endif