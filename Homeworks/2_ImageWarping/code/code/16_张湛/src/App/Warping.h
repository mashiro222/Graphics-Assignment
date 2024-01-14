#pragma once
#ifndef _WARPING_H_
#define _WARPING_H_

#include "Line.h"
#include <vector>
#include <Eigen/Dense> 
#include <QImage>

using Eigen::MatrixXf;
using std::vector;

class Warping
{
public:
	Warping();
	~Warping();

	void CpPushBack(Line * line, bool in = false);
	void Init();
	vector<Line *>* GetControlpoints();
	vector<bool>* GetAffineset();

	virtual void DrawCP(QPainter &painter, int dw = 0, int dh = 0);
	virtual void InitWarp();
	virtual MatrixXf WarpingIJ(int i, int j);
	void SetChangedIJ(MatrixXf ij);

	void FillHole_1(QImage *img);
	void FillHole_2(QImage *img);

protected:
	vector<Line *> controlpoints;
	vector<bool> in_affineset;
	vector<MatrixXf> is_changed;
};

#endif