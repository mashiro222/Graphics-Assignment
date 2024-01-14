#include "IDWWarper.h"
#include <vector>
#include <map>
#define DIST(x1, y1, x2, y2) (((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)))
IDWWarper IDWWarper::inst;

IDWWarper* IDWWarper::getInstance() {
	return &(inst);
}

void IDWWarper::initialize(std::vector<IMap> points) {
	double E, E0; E = 0; E0 = 0;
	//初始化赋值
	pairs = points.size();
	point_ = points;
	D.clear();
	D = std::vector<DMatrix>(pairs);
	//算D
	double d11, d12, d21, d22;
	double a, b, c, d, e, f, g, h;//方程的系数
	std::vector<double> S(pairs);
	for (int i = 0; i < pairs; i++) {
		a = 0; b = 0; c = 0; d = 0; e = 0; f = 0; g = 0; h = 0;
		d11 = 0; d12 = 0; d21 = 0; d22 = 0;
		for (int j = 0; j < pairs ; j++) {
			if (j == i) {
				continue;
			}
			double pix, piy, pjx, pjy, qix, qiy, qjx, qjy;
			pix = point_[i].first.X();
			piy = point_[i].first.Y();
			pjx = point_[j].first.X();
			pjy = point_[j].first.Y();
			qix = point_[i].second.X();
			qiy = point_[i].second.Y();
			qjx = point_[j].second.X();
			qjy = point_[j].second.Y();
			
			//求d11,d12

			//double sigma = 1 / ((pix - pjx) * (pix - pjx) + (piy - pjy) * (piy - pjy));
			double sigma = 1.0 / DIST(pix, piy, pjx, pjy);
			S[j] = sigma;
			a += sigma * (pjx - pix) * (pjx - pix);
			b += sigma * (pjx - pix) * (pjy - pjy);
			c += sigma * (pjx - pix) * (pjy - pjy);
			d += sigma * (pjy - piy) * (pjy - piy);
			e += sigma * (pjx - pix) * (qjx - qix);
			f += sigma * (pjy - piy) * (qjx - qix);
			g += sigma * (pjx - pix) * (qjy - qiy);
			h += sigma * (pjy - piy) * (qjy - qiy);
		
		}
		//计算D
		d11 = (e * d - b * f) / (a * d - b * c);
		d12 = (a * f - c * e) / (a * d - b * c);
		d21 = (g * d - h * b) / (a * d - b * c);
		d22 = (a * h - c * g) / (a * d - b * c);
		//赋值
		DMatrix temp;
		temp.d11 = d11; 
		temp.d12 = d12;
		temp.d21 = d21;
		temp.d22 = d22;

		printf("i=%d, t11=%lf, t12=%lf, t21=%lf, t22=%lf\n", i, d11, d12, d21, d22);
		// validate
		printf("1: %lf = %lf\n", a * d11 + b * d12, e);
		printf("2: %lf = %lf\n", c * d11 + d * d12, f);
		printf("3: %lf = %lf\n", a * d21 + b * d22, g);
		printf("4: %lf = %lf\n", c * d21 + d * d22, h);

		//算一下E

		for (int i = 0; i < pairs; i++) {
			for (int j = 0; j < pairs; j++) {
				if (j == i) {
					continue;
				}
				double temp1, temp2;
				temp1 = d11 * (point_[j].first.X() - point_[i].first.X()) + d12 * (point_[j].first.Y() - point_[i].first.Y()) + point_[j].second.X() - point_[i].second.X();
				temp2 = d21 * (point_[j].first.X() - point_[i].first.X()) + d22 * (point_[j].first.Y() - point_[i].first.Y()) + point_[j].second.Y() - point_[i].second.Y();
				E += S[j] * (temp1 * temp1 + temp2 * temp2);
				temp1 = 1 * (point_[j].first.X() - point_[i].first.X()) + 0 * (point_[j].first.Y() - point_[i].first.Y()) + point_[j].second.X() - point_[i].second.X();
				temp2 = 0 * (point_[j].first.X() - point_[i].first.X()) + 1 * (point_[j].first.Y() - point_[i].first.Y()) + point_[j].second.Y() - point_[i].second.Y();
				E0 += S[j] * (temp1 * temp1 + temp2 * temp2);
			}
		}
		printf("D %lf && I %lf\n", E, E0);

		D[i] = temp;
	}
}

const IPoint& IDWWarper::func(IPoint& p) {
	double fx, fy;
	double sum = 0;
	double px = (double)p.X();
	double py = (double)p.Y();

	std::vector<double> sigma(pairs);
	for (int i = 0; i < pairs; i++) {
		double pix = point_[i].first.X();
		double piy = point_[i].second.Y();
		sigma[i] = 1.0 / DIST(px, py, pix, piy);
		sum += sigma[i];
	}
	std::vector<double> fix(pairs);
	std::vector<double> fiy(pairs);
	std::vector<double> wi(pairs);
	for (int i = 0; i < pairs; i++) {
		wi[i] = sigma[i] / sum;
		double qix, qiy, pix, piy, d11, d12, d21, d22;
		qix = point_[i].second.X();
		qiy = point_[i].second.Y();
		pix = point_[i].first.X();
		piy = point_[i].first.Y();
		d11 = D[i].d11;
		d12 = D[i].d12;
		d21 = D[i].d21;
		d22 = D[i].d22;

		fix[i] = qix + d11 * (px - pix) + d12 * (py - piy);
		fiy[i] = qiy + d21 * (px - pix) + d22 * (py - piy);
	}
	fx = 0; fy = 0;
	for (int i = 0; i < pairs; i++) {
		fx += wi[i] * fix[i];
		fy += wi[i] * fiy[i];
	}

	IPoint result;
	result.setX((int)fx).setY((int)fy);
	return result;
}

//aa
