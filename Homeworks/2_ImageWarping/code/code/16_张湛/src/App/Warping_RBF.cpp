#include "Warping_RBF.h"
#include <Eigen/Dense> 
#include <iostream>

using Eigen::MatrixXf;
using std::cout;
using std::endl;


Warping_RBF::Warping_RBF()
{
}

Warping_RBF::Warping_RBF(Warping IW)
{
	vector<Line *> *ptr = IW.GetControlpoints();
	vector<bool> *aff = IW.GetAffineset();
	int cpn = (*ptr).size();
	for (int i = 0; i < cpn; i++) {
		controlpoints.push_back((*ptr)[i]);
		in_affineset.push_back((*aff)[i]);
	}

	InitWarp();
}


Warping_RBF::~Warping_RBF()
{
}

void Warping_RBF::InitWarp()
{
	int cpn = controlpoints.size();
	int anum = 0;
	int rnum = 0;
	for (int i = 0; i < cpn; i++) {
		MatrixXf cpPi = MatrixXf::Zero(2, 1);
		MatrixXf cpQi = MatrixXf::Zero(2, 1);
		cpPi << controlpoints[i]->get_start_point_x_(), controlpoints[i]->get_start_point_y_();
		cpQi << controlpoints[i]->get_end_point_x_(), controlpoints[i]->get_end_point_y_();
		if (in_affineset[i]) {
			cpAP.push_back(cpPi);
			cpAQ.push_back(cpQi);
			anum++;
		}
		else {
			cpRP.push_back(cpPi);
			cpRQ.push_back(cpQi);
			rnum++;
		}
	}

	M = MatrixXf::Zero(2, 2);
	b = MatrixXf::Zero(2, 1);
	switch (anum) {
	case 0:
		M << 1, 0,
			0, 1;
		break;
	case 1:
		M << 1, 0,
			0, 1;
		b = cpAQ[0] - cpAP[0];
		break;
	case 2:
		M << 0, -1,
			1, 0;
		float s, a1, a2;
		s = ((cpAP[0] - cpAP[1]).transpose() * (cpAP[0] - cpAP[1]))(0, 0);
		a1 = ((cpAP[0] - cpAP[1]).transpose() * (cpAQ[0] - cpAQ[1]))(0, 0);
		a2 = ((cpAP[0] - cpAP[1]).transpose() * (M * (cpAQ[0] - cpAQ[1])))(0, 0);
		M << a1 / s, a2 / s,
			-a2 / s, a1 / s;
		b = cpAQ[0] - M * cpAP[0];
		break;
	default:	//anum >= 3
		MatrixXf atemp1 = MatrixXf::Zero(6, 6);
		MatrixXf atemp2 = MatrixXf::Zero(6, 6);
		MatrixXf aQ = MatrixXf::Zero(6, 1);
		float sigpix, sigpiy, sigpix2, sigpixiy, sigpiy2, sigqix, sigqiy, sigpxiqx, sigpxiqy, sigpyiqx, sigpyiqy;
		sigpix = sigpiy = sigpix2 = sigpixiy = sigpiy2 = sigqix = sigqiy = sigpxiqx = sigpxiqy = sigpyiqx = sigpyiqy = 0;
		for (int i = 0; i < anum; i++) {
			sigpix += cpAP[i](0, 0);
			sigpiy += cpAP[i](1, 0);
			sigpix2 += cpAP[i](0, 0) * cpAP[i](0, 0);
			sigpiy2 += cpAP[i](1, 0) * cpAP[i](1, 0);
			sigpixiy += cpAP[i](0, 0) * cpAP[i](1, 0);
			sigqix += cpAQ[i](0, 0);
			sigqiy += cpAQ[i](1, 0);
			sigpxiqx += cpAP[i](0, 0) * cpAQ[i](0, 0);
			sigpxiqy += cpAP[i](0, 0) * cpAQ[i](1, 0);
			sigpyiqx += cpAP[i](1, 0) * cpAQ[i](0, 0);
			sigpyiqy += cpAP[i](1, 0) * cpAQ[i](1, 0);
		}
		atemp1 << 1, 0, sigpix, sigpiy, 0, 0,
			0, 1, 0, 0, sigpix, sigpiy,
			sigpix, 0, sigpix2, sigpixiy, 0, 0,
			sigpiy, 0, sigpixiy, sigpiy2, 0, 0,
			0, sigpix, 0, 0, sigpix2, sigpixiy,
			0, sigpiy, 0, 0, sigpixiy, sigpiy2;
		aQ << sigqix, sigqiy, sigpxiqx, sigpyiqx, sigpxiqy, sigpyiqy;
		atemp2 = atemp1.inverse() * aQ;
		b << atemp2(0, 0), atemp2(1, 0);
		M << atemp2(2, 0), atemp2(3, 0),
			atemp2(4, 0), atemp2(5, 0);
	}
	//cout << "b = " << b << endl;
	//cout << "M = " << M << endl;

	if (rnum < 2) {
	}
	else {
		Ax = MatrixXf::Zero(rnum, 1);
		Ay = MatrixXf::Zero(rnum, 1);
		MatrixXf T = MatrixXf::Zero(rnum, rnum);
		MatrixXf Qx = MatrixXf::Zero(rnum, 1);
		MatrixXf Qy = MatrixXf::Zero(rnum, 1);
		r2 = new float[rnum];
		float** d2 = new float*[rnum];
		for (int j = 0; j < rnum; j++) {
			MatrixXf x = MatrixXf::Zero(2, 1);
			x = cpRQ[j] - M * cpRP[j] - b;
			Qx(j, 0) = x(0, 0);
			Qy(j, 0) = x(1, 0);
			d2[j] = new float[rnum];
			int k = (j + 1) % rnum;
			r2[j] = ((cpRP[k] - cpRP[j]).transpose() * (cpRP[k] - cpRP[j]))(0, 0);
			for (int i = 0; i < rnum; i++) {
				d2[j][i] = ((cpRP[i] - cpRP[j]).transpose() * (cpRP[i] - cpRP[j]))(0, 0);
				if (i != j && d2[j][i] < r2[j]) r2[j] = d2[j][i];
			}
		}
		for (int j = 0; j < rnum; j++) {
			for (int i = 0; i < rnum; i++) {
				T(j, i) = sqrt(d2[j][i] + r2[i]);
			}
		}
		T = T.inverse();
		Ax = T * Qx;
		Ay = T * Qy;
		//cout << "T = " << T << endl;
	}
}

MatrixXf Warping_RBF::WarpingIJ(int i, int j)
{
	int rnum = cpRP.size();
	MatrixXf ij = MatrixXf::Zero(2, 1);
	ij << i, j;
	MatrixXf new_ij = MatrixXf::Zero(2, 1);
	if (rnum < 2) {
		new_ij += M * ij + b;
		return new_ij;
	}
	for (int k = 0; k < rnum; k++) {
		float tempx = sqrt(((cpRP[k] - ij).transpose() * (cpRP[k] - ij))(0, 0) + r2[k]);;
		new_ij(0, 0) += tempx * Ax(k, 0);
		new_ij(1, 0) += tempx * Ay(k, 0);
	}
	new_ij += M * ij + b;
	SetChangedIJ(new_ij);
	return new_ij;
}
