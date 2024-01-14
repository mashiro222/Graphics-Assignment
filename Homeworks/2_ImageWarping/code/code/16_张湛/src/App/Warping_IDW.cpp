#include "Warping_IDW.h"
#include <Eigen/Dense> 
#include <iostream>

using Eigen::MatrixXf;
using std::vector;
using std::cout;
using std::endl;


Warping_IDW::Warping_IDW()
{
}

Warping_IDW::Warping_IDW(Warping IW)
{
	vector<Line *> *ptr = IW.GetControlpoints();
	vector<bool> *aff = IW.GetAffineset();
	int cpn = (*ptr).size();
	for (int i = 0; i < cpn; i++) {
		if ((*aff)[i]) continue;
		controlpoints.push_back((*ptr)[i]);
	}
	cpn = controlpoints.size();

	InitWarp();
}


Warping_IDW::~Warping_IDW()
{
}

void Warping_IDW::DrawCP(QPainter & painter, int dw, int dh)
{
	QColor green(0, 255, 0);
	QColor blue(0, 0, 255);
	QColor greenblue(0, 191, 191, 191);
	QColor red(191, 0, 0, 191);
	QPen pen1(green);
	QPen pen2(blue);
	QPen pen3(greenblue);
	QPen pen4(red);
	pen4.setWidth(2);
	pen3.setWidth(2);
	pen2.setWidth(5);
	pen1.setWidth(5);

	int cpn = controlpoints.size();
	for (size_t i = 0; i < cpn; i++)
	{
		painter.setPen(pen3);
		controlpoints[i]->Draw(painter, dw, dh);
		QPoint startp(controlpoints[i]->get_start_point_x_() + dw, controlpoints[i]->get_start_point_y_() + dh);
		QPoint endp(controlpoints[i]->get_end_point_x_() + dw, controlpoints[i]->get_end_point_y_() + dh);

		painter.setPen(pen2);
		painter.drawPoint(startp);
		painter.setPen(pen1);
		painter.drawPoint(endp);
	}
}

void Warping_IDW::InitWarp()
{
	int cpn = controlpoints.size();
	if (cpn < 3) return;

	for (int i = 0; i < cpn; i++) {
		MatrixXf cpPi = MatrixXf::Zero(2, 1);
		MatrixXf cpQi = MatrixXf::Zero(2, 1);
		MatrixXf Ti = MatrixXf::Zero(2, 2);
		cpPi << controlpoints[i]->get_start_point_x_(), controlpoints[i]->get_start_point_y_();
		cpQi << controlpoints[i]->get_end_point_x_(), controlpoints[i]->get_end_point_y_();
		cpP.push_back(cpPi);
		cpQ.push_back(cpQi);
		T.push_back(Ti);
	}
	vector<MatrixXf> dPp;
	vector<MatrixXf> dQq;
	for (int i = 0; i < cpn; i++) {
		dPp.clear();
		dQq.clear();
		MatrixXf sig1 = MatrixXf::Zero(2, 2);
		MatrixXf sig2 = MatrixXf::Zero(2, 2);
		for (int j = 0; j < cpn; j++) {
			MatrixXf dPji = MatrixXf::Zero(2, 1);
			MatrixXf dQji = MatrixXf::Zero(2, 1);
			dPji << cpP[j] - cpP[i];
			dQji << cpQ[j] - cpQ[i];
			dPp.push_back(dPji);
			dQq.push_back(dQji);
			if (i == j) continue;
			sig1 += 1 / (((dPp[j].transpose() * dPp[j]) * (dPp[j].transpose() * dPp[j]))(0, 0)) * (dPp[j] * dPp[j].transpose());
			sig2 += 1 / (((dPp[j].transpose() * dPp[j]) * (dPp[j].transpose() * dPp[j]))(0, 0)) * (dQq[j] * dPp[j].transpose());
		}
		T[i] = sig2 * sig1.inverse();
		//cout << "T[" << i << "] = " << T[i] << endl;
	}

}

MatrixXf Warping_IDW::WarpingIJ(int i, int j)
{
	int cpn = controlpoints.size();

	bool at_cp = false;
	MatrixXf ij = MatrixXf::Zero(2, 1);
	MatrixXf new_ij = MatrixXf::Zero(2, 1);
	ij << i, j;
	if (cpn < 3) {
		return ij;
	}

	float sig = 0;
	float* sigk = new float[cpn];
	int k;
	for (k = 0; k < cpn; k++) {
		sigk[k] = (((ij - cpP[k]).transpose() * (ij - cpP[k]))(0, 0));
		if (sigk[k] < 1E-6) {
			at_cp = true;
			break;
		}
		sig += 1 / sigk[k];
	}
	if (at_cp) {
		new_ij<<cpQ[k](0, 0), cpQ[k](1, 0);
		return new_ij;
	}
	for (k = 0; k < cpn; k++) {
		new_ij += 1 / sigk[k] / sig * (cpQ[k] + T[k] * (ij - cpP[k]));
	}
	SetChangedIJ(new_ij);
	return new_ij;
}