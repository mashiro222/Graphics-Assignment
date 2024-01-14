#include "Warping.h"
#include "kdTree.h"
#include <Eigen/Dense> 
#include <iostream>

using Eigen::MatrixXf;
using std::cout;
using std::endl;

Warping::Warping()
{
	Init();
}

Warping::~Warping()
{
}

void Warping::CpPushBack(Line * line, bool in)
{
	controlpoints.push_back(line);
	in_affineset.push_back(in);
}

void Warping::Init()
{
	controlpoints.clear();
	in_affineset.clear();
	is_changed.clear();
}

vector<Line*>* Warping::GetControlpoints()
{
	return &controlpoints;
}

vector<bool>* Warping::GetAffineset()
{
	return &in_affineset;
}

void Warping::DrawCP(QPainter &painter, int dw, int dh)
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
		if (in_affineset[i]) painter.setPen(pen4);
		else painter.setPen(pen3);
		controlpoints[i]->Draw(painter, dw, dh);
		QPoint startp(controlpoints[i]->get_start_point_x_()+ dw, controlpoints[i]->get_start_point_y_()+ dh);
		QPoint endp(controlpoints[i]->get_end_point_x_()+ dw, controlpoints[i]->get_end_point_y_() + dh);

		painter.setPen(pen2);
		painter.drawPoint(startp);
		painter.setPen(pen1);
		painter.drawPoint(endp);
	}
}

void Warping::InitWarp()
{
}


MatrixXf Warping::WarpingIJ(int i, int j)
{
	MatrixXf new_ij = MatrixXf::Zero(2, 1);
	new_ij << i, j;
	SetChangedIJ(new_ij);
	return new_ij;
}

void Warping::SetChangedIJ(MatrixXf ij)
{
	is_changed.push_back(ij);
}

void Warping::FillHole_1(QImage *img)
{
	int w = img->width();
	int h = img->height();

	int num = is_changed.size();
	if (num == 0) return;

	int cpn = controlpoints.size();
	vector<MatrixXf> cpP;
	vector<MatrixXf> cpQ;
	for (int i = 0; i < cpn; i++) {
		MatrixXf cpPi = MatrixXf::Zero(2, 1);
		MatrixXf cpQi = MatrixXf::Zero(2, 1);
		cpPi << controlpoints[i]->get_start_point_x_(), controlpoints[i]->get_start_point_y_();
		cpQi << controlpoints[i]->get_end_point_x_(), controlpoints[i]->get_end_point_y_();
		cpP.push_back(cpPi);
		cpQ.push_back(cpQi);
	}

	float *r = new float[cpn];
	float** d1 = new float*[cpn];
	float** d2 = new float*[cpn];
	for (int j = 0; j < cpn; j++) {
		d1[j] = new float[cpn];
		d2[j] = new float[cpn];
		int k = (j + 1) % cpn;
		r[j] = 0;
		for (int i = 0; i < cpn; i++) {
			d1[j][i] = ((cpQ[i] - cpQ[j]).transpose() * (cpQ[i] - cpQ[j]))(0, 0);
			d2[j][i] = ((cpP[i] - cpP[j]).transpose() * (cpP[i] - cpP[j]))(0, 0);
			float rij = d1[j][i] / d2[j][i];
			if (i != j && rij > r[j]) r[j] = rij;
		}
	}
	int bound = 0;
	for (int j = 0; j < cpn; j++) {
		if (bound < r[j]) bound = r[j];
	}
	bound = int(sqrt(bound)) + 1;

	bool **set;
	set = new bool*[w];
	for (int k = 0; k < w; k++) {
		set[k] = new bool[h];
		for (int l = 0; l < h; l++) {
			set[k][l] = false;
		}
	}

	int max_i, min_i, max_j, min_j;
	max_i = min_i = is_changed[0](0, 0);
	max_j = min_j = is_changed[0](1, 0);
	for (int k = 0; k < num; k++) {
		int new_i = is_changed[k](0, 0);
		int new_j = is_changed[k](1, 0);

		if (new_i < 0) {
			min_i = 0;
			continue;
		}
		else if (new_i > w - 1) {
			max_i = w - 1;
			continue;
		}
		else if (new_i > max_i) max_i = new_i;
		else if (new_i < min_i) min_i = new_i;

		if (new_j < 0) {
			min_j = 0;
			continue;
		}
		else if (new_j > h - 1) {
			max_j = h - 1;
			continue;
		}
		else if (new_j > max_j) max_j = new_j;
		else if (new_j < min_j) min_j = new_j;

		set[new_i][new_j] = true;
	}

	for (int i = min_i; i <= max_i; i++) {
		for (int j = min_j; j <= max_j; j++) {
			int k, r, g, b;
			r = g = b = 250;
			//r = 200; g = 125; b = 50; //测试用
			if (set[i][j]) continue;
			for (k = 1; k <= bound; k++) {
				int x1 = i + k <= max_i ? i + k : max_i;
				int y1 = j + k <= max_j ? j + k : max_j;
				int x2 = i - k >= min_i ? i - k : min_i;
				int y2 = j - k >= min_j ? j - k : min_j;
				if (set[x2][y2]) {
					img->setPixel(i, j, img->pixel(x2, y2));
					break;
				}
				if (set[x2][j]) {
					img->setPixel(i, j, img->pixel(x2, j));
					break;
				}
				if (set[x2][y1]) {
					img->setPixel(i, j, img->pixel(x2, y1));
					break;
				}
				if (set[i][y2]) {
					img->setPixel(i, j, img->pixel(i, y2));
					break;
				}
				if (set[i][y1]) {
					img->setPixel(i, j, img->pixel(i, y1));
					break;
				}
				if (set[x1][y2]) {
					img->setPixel(i, j, img->pixel(x1, y2));
					break;
				}
				if (set[x1][j]) {
					img->setPixel(i, j, img->pixel(x1, j));
					break;
				}
				if (set[x1][y1]) {
					img->setPixel(i, j, img->pixel(x1, y1));
					break;
				}
			}
		}
	}
}

void Warping::FillHole_2(QImage *img)
{
	if (is_changed.size() == 0) return;

	int cpn = controlpoints.size();
	vector<MatrixXf> cpP;
	vector<MatrixXf> cpQ;
	for (int i = 0; i < cpn; i++) {
		MatrixXf cpPi = MatrixXf::Zero(2, 1);
		MatrixXf cpQi = MatrixXf::Zero(2, 1);
		cpPi << controlpoints[i]->get_start_point_x_(), controlpoints[i]->get_start_point_y_();
		cpQi << controlpoints[i]->get_end_point_x_(), controlpoints[i]->get_end_point_y_();
		cpP.push_back(cpPi);
		cpQ.push_back(cpQi);
	}

	float *r = new float[cpn];
	float** d1 = new float*[cpn];
	float** d2 = new float*[cpn];
	for (int j = 0; j < cpn; j++) {
		d1[j] = new float[cpn];
		d2[j] = new float[cpn];
		int k = (j + 1) % cpn;
		r[j] = 0;
		for (int i = 0; i < cpn; i++) {
			d1[j][i] = ((cpQ[i] - cpQ[j]).transpose() * (cpQ[i] - cpQ[j]))(0, 0);
			d2[j][i] = ((cpP[i] - cpP[j]).transpose() * (cpP[i] - cpP[j]))(0, 0);
			float rij = d1[j][i] / d2[j][i];
			if (i != j && rij > r[j]) r[j] = rij;
		}
	}
	int bound = 0;
	for (int j = 0; j < cpn; j++) {
		if (bound < r[j]) bound = r[j];
	}
	bound = int(sqrt(bound)) + 1;

	kdTree T;
	T.read_in(is_changed);

	int K = 5; //K临近点
	int w = img->width();
	int h = img->height();
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			vector<Node> res;
			vector<int> dist;
			int find_k = T.find_nearest_k_points(i, j, K, res, dist, bound);
			//cout << "total: " << find_k << endl;
			if (find_k == 0) continue;
			if (res[0].feature[0] == i && res[0].feature[1] == j) {
				continue;
			}

			int r, g, b;
			r = g = b = 250;
			//r = 200; g = 125; b = 50; //测试用
			vector<int> kr;
			vector<int> kg;
			vector<int> kb;
			vector<int> knum;
			for (int k = 0; k < find_k; k++) {
				int x = res[k].feature[0];
				int y = res[k].feature[1];
				if (x < 0 || y < 0 || x >= w || y >= h) continue;
				
				b = img->pixelColor(x, y).blue();
				g = img->pixelColor(x, y).green();
				r = img->pixelColor(x, y).red();
				int l;
				for (l = 0; l < knum.size(); l++) {
					if (kr[l] == r && kg[l] == g && kb[l] == b) {
						knum[l]++;
						break;
					}
				}
				if (l == knum.size()) {
					kr.push_back(r);
					kg.push_back(g);
					kb.push_back(b);
					knum.push_back(1);
				}
			}

			if (knum.size() == 0) {
				continue;
			}
			int max = knum[0];
			int m = 0;
			for (int k = 1; k < knum.size(); k++) {
				if (knum[k] > max) {
					max = knum[k];
					m = k;
				}
			}

			img->setPixelColor(i, j, QColor(kr[m], kg[m], kb[m]));
			
		}
	}
}
