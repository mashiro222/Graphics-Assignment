#include "PoissonFusion.h"
#include <iostream>
#include <Eigen/sparse>
#include <ctime>

CPoissonFusion::CPoissonFusion()
{
	coor_mapto_index = nullptr;
	coef_a_uptodate = false;
}

CPoissonFusion::~CPoissonFusion()
{
	if (coor_mapto_index != nullptr)
		delete[] coor_mapto_index;
}

cv::Mat CPoissonFusion::GetFusion()
{
	clock_t starttime = clock();
	cv::Mat result = target.clone();
	if (!coef_a_uptodate)
	{
		GenIndex();
		GenCoefMatA();
		PreFactorizeA();
		coef_a_uptodate = true;
	}
	GenVecb(mixed_grad);
	Eigen::MatrixXd X;
	X = solver.solve(vec_b);

	int inner_points_size = index_mapto_coor.size();

	uchar colorv;
	for (int i = 0; i < inner_points_size; i++)
	{
		for (int chn = 0; chn < target.channels(); chn++)
		{
			if (X(i, chn) > 255.)
				colorv = 255;
			else if (X(i, chn) < 0.)
				colorv = 0;
			else
				colorv = (uchar)X(i, chn);
			(*AddrMatAt(result, index_mapto_coor[i] + pos, chn)) = colorv;
		}
	}
	clock_t endtime = clock();
	std::cout << "Time consumed : " << (endtime - starttime)<< "ms" <<std::endl;
	return result;
}

void CPoissonFusion::PreFactorizeA()
{
	solver.compute(coef_mat_a);
}

void CPoissonFusion::SetSourceMask(const cv::Mat asource, const cv::Mat amask)
{
	source = asource;  mask = amask;
	if (source.size().width != mask.size().width ||source.size().height != mask.size().height)
	{
		std::cout << "Error: source image does not correspond with mask image" << std::endl;
		throw 99;
	}
	if (mask.elemSize() != 1)
	{
		std::cout << "Error: mask element type should be CV_8UC1" << std::endl;
		throw 99;
	}
	coef_a_uptodate = false;
}

void CPoissonFusion::GenFeasibleRect()
{
	//ȡ��Ч�ص��Ĳ���
	int rectx, recty, rectwidth, rectheight;
	rectx = std::min(std::max(1 - pos.x, 0), mask.size().width);
	recty = std::min(std::max(1 - pos.y, 0), mask.size().height);
	rectwidth = std::min(target.size().width - 1 - pos.x - rectx, mask.size().width - 1 - rectx);
	rectheight = std::min(target.size().height - 1 - pos.y - recty, mask.size().height - 1 - recty);

	if (cv::Rect(rectx, recty, rectwidth, rectheight) != feasible_rect)
	{
		feasible_rect = cv::Rect(rectx, recty, rectwidth, rectheight);
		coef_a_uptodate = false;
	}
}

void CPoissonFusion::GenIndex()
{

	index_mapto_coor.clear();
	index_mapto_coor.reserve(feasible_rect.width*feasible_rect.height);

	delete[] coor_mapto_index;
	coor_mapto_index = new int[feasible_rect.width*feasible_rect.height];
	memset(coor_mapto_index, 255, sizeof(int)*feasible_rect.width*feasible_rect.height);

	int cnt = 0;

	//��Mask�в�Ϊ0��Ԫ�ؿ�����Ҫ�ںϵ��ڵ㣬�����š��������ڵ�ģ����Ϊ-1��
	for (int i = feasible_rect.y; i < feasible_rect.y + feasible_rect.height; i++)
	{
		for (int j = feasible_rect.x; j < feasible_rect.x+feasible_rect.width; j++)
		{
			if (*(AddrMatAt(mask, i, j)) > 0)
			{
				index_mapto_coor.push_back(cv::Point(j, i));
				coor_mapto_index[CoorInRect(j, i, feasible_rect)] = cnt++;
			}
		}
	}
	index_mapto_coor.shrink_to_fit();
}

void CPoissonFusion::GenCoefMatA()
{
	int inner_points_size = index_mapto_coor.size();

	std::vector<Eigen::Triplet<double>> coefs;
	coefs.reserve(inner_points_size * 4);

	coef_mat_a = Eigen::SparseMatrix<double>();

	coef_mat_a.resize(inner_points_size, inner_points_size);

	//����ÿ���㣬����������������ң�����ϵ��
	int x_i, y_i, id;
	for (int i = 0; i < index_mapto_coor.size(); i++)
	{
		x_i = index_mapto_coor[i].x;
		y_i = index_mapto_coor[i].y;
		coefs.push_back(Eigen::Triplet<double>(i, i, 4.));
		//��
		if (cv::Point(x_i, y_i - 1).inside(feasible_rect))
		{
			id = coor_mapto_index[CoorInRect(x_i, y_i - 1, feasible_rect)];
			if (id >= 0)
				coefs.push_back(Eigen::Triplet<double>(i, id, -1.));
		}
		//��
		if (cv::Point(x_i, y_i + 1).inside(feasible_rect))
		{
			id = coor_mapto_index[CoorInRect(x_i, y_i + 1, feasible_rect)];
			if (id >= 0)
				coefs.push_back(Eigen::Triplet<double>(i, id, -1.));
		}
		//��
		if (cv::Point(x_i - 1, y_i).inside(feasible_rect))
		{
			id = coor_mapto_index[CoorInRect(x_i - 1, y_i, feasible_rect)];
			if (id >= 0)
				coefs.push_back(Eigen::Triplet<double>(i, id, -1.));
		}
		//��
		if (cv::Point(x_i + 1, y_i).inside(feasible_rect))
		{
			id = coor_mapto_index[CoorInRect(x_i + 1, y_i, feasible_rect)];
			if (id >= 0)
				coefs.push_back(Eigen::Triplet<double>(i, id, -1.));
		}
	}
	coef_mat_a.setFromTriplets(coefs.begin(), coefs.end());
}

void CPoissonFusion::GenVecb(bool amixed)
{
	int inner_points_size = index_mapto_coor.size();
	Eigen::MatrixXd laplacemap;
	if (amixed)
	{
		GenLaplaceMapMixed(source, target, pos, index_mapto_coor, laplacemap);
	}
	else
	{
		GenLaplaceMap(source, index_mapto_coor, laplacemap);
	}
	GenVecb(laplacemap, target, pos, index_mapto_coor, coor_mapto_index, feasible_rect, vec_b);
}

void CPoissonFusion::GenVecb(const Eigen::MatrixXd& laplace_map, const cv::Mat targetimg, const cv::Point shift_pos, std::vector<cv::Point>& id_to_coor, const int* coor_to_id, const cv::Rect& frect, Eigen::MatrixXd& b)
{
	cv::Point direction[4] = { cv::Point(0,-1), cv::Point(0,1) ,cv::Point(-1,0) ,cv::Point(1, 0) };

	int inner_points_size = id_to_coor.size();
	b.resize(inner_points_size, targetimg.channels());
	b = laplace_map;

	cv::Point ptemp;
	for (int i = 0; i < inner_points_size; i++)		//���е�
	{
		for (int k = 0; k < 4; k++)					//�ĸ�����
		{
			ptemp = id_to_coor[i] + direction[k];
			if ((!ptemp.inside(frect)) || coor_to_id[CoorInRect(ptemp, frect)] < 0)
			{
				for (int ch = 0; ch < targetimg.channels(); ch++)	//ÿ��channel
				{
					b(i, ch) += (double)(*AddrMatAt(targetimg, ptemp + shift_pos, ch));
				}
			}
		}
	}
}

void CPoissonFusion::GenLaplaceMap(const cv::Mat& img, const std::vector<cv::Point>& id_to_coor, Eigen::MatrixXd& laplacemap)
{
	int inner_points_size = id_to_coor.size();
	laplacemap = Eigen::MatrixXd(inner_points_size, img.channels());
	for (int i = 0; i < inner_points_size; i++)
	{
		laplacemap.row(i) = CImageGrad::DivGrad(img, id_to_coor[i]);
	}
}
void CPoissonFusion::GenLaplaceMapMixed(const cv::Mat& sourceimg, const cv::Mat& targetimg, const cv::Point shift_pos, std::vector<cv::Point>& id_to_coor, Eigen::MatrixXd& laplacemap)
{
	int inner_points_size = id_to_coor.size();
	laplacemap = Eigen::MatrixXd(inner_points_size, sourceimg.channels());
	for (int i = 0; i < inner_points_size; i++)
	{
		laplacemap.row(i) = CImageGrad::DivMixGrad(sourceimg, targetimg, shift_pos, id_to_coor[i]);
	}
}

inline uchar* CPoissonFusion::AddrMatAt(const cv::Mat& mat, const int irowy, const int jcolx, const int ch)
{
	return mat.data + mat.step[0] * irowy + mat.step[1] * jcolx + ch;
}

inline uchar* CPoissonFusion::AddrMatAt(const cv::Mat& mat, const int irowy, const int jcolx)
{
	return mat.data + mat.step[0] * irowy + mat.step[1] * jcolx;
}

inline int CPoissonFusion::CoorInRect(const int x, const int y, const cv::Rect& rect)
{
	return (y - rect.y)*rect.width + x - rect.x;
}
inline int CPoissonFusion::CoorInRect(cv::Point p, const cv::Rect& rect)
{
	return (p.y - rect.y)*rect.width + p.x - rect.x;
}

inline uchar* CPoissonFusion::AddrMatAt(const cv::Mat& mat, cv::Point p, const int ch)
{
	return mat.data + mat.step[0] * p.y + mat.step[1] * p.x + ch;
}
inline uchar* CPoissonFusion::AddrMatAt(const cv::Mat& mat, cv::Point p)
{
	return mat.data + mat.step[0] * p.y + mat.step[1] * p.x;
}