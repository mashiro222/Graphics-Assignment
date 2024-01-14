#pragma once

#include <opencv2/core/core.hpp>

#include <Eigen/Core>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseQR>

#include "ImageGrad.h"
/*
Poissonͼ���ں�
*/
class CPoissonFusion
{
public:
	CPoissonFusion();
	~CPoissonFusion();


	/*
	��source�Ĳ���ȡmask�Ĳ����ںϵ�target�У�source (0, 0)��λ�ö��뵽 target�� (x_pos, y_pos)λ�á�
	* source��target��channels��Ӧ�����
	* maskӦ��ֻ��1��channel
	*/
	cv::Mat GetFusion();


	/*����source��mask��������ʽ�Ƿ�Ϸ���������׳��쳣*/
	void SetSourceMask(const cv::Mat asource, const cv::Mat amask);

	/*����target*/
	void SetTarget(const cv::Mat atarget) { target = atarget; }

	/*����ƫ�ƾ��롣ͬʱ���¼�����Ч�ص�����*/
	void SetShiftPos(const cv::Point shift_pos) { pos = shift_pos; GenFeasibleRect(); }

	/*�����Ƿ����Mixed����*/
	void SetMixedGrad(bool abool) { mixed_grad = abool; }
	
private:
	/*
	����ϵ������
	*/
	void GenCoefMatA();
	
	/*
	�����ڵ������
	*/
	void GenIndex();

	/*
	����b
	*/
	void GenVecb(bool Mixed);


	/*
	Ԥ�ֽ�ϵ������
	*/
	void PreFactorizeA();

	void GenFeasibleRect();

public:
	/*
	�Ծ����е�irowy��,��jcolx��/����(jcolx, irowy)��ch��uchar��ַ�ķ��ʡ���ʹ��at<>����
	*/
	static uchar* AddrMatAt(const cv::Mat& mat, const int irowy,const int jcolx, const int ch);
	
	/*
	�Ծ����е�irowy��,��jcolx��/����(jcolx, irowy)Ԫ���׵�ַ�ķ��ʡ���ʹ��at<>��������������֪����������
	*/
	static uchar* AddrMatAt(const cv::Mat& mat, const int irowy, const int jcolx);

	/*
	�Ծ��������� p ��ch��uchar��ַ�ķ��ʡ���ʹ��at<>��������������֪����������
	*/
	static uchar* AddrMatAt(const cv::Mat& mat, cv::Point p, const int ch);
	
	/*
	�Ծ��������� p Ԫ���׵�ַ�ķ��ʡ���ʹ��at<>��������������֪����������
	*/
	static uchar* AddrMatAt(const cv::Mat& mat, cv::Point p);

	static int CoorInRect(const int x, const int y, const cv::Rect& rect);
	static int CoorInRect(cv::Point p, const cv::Rect& rect);

	/*
	����b�ľ�̬����
	*/
	static void GenVecb(const Eigen::MatrixXd& laplace_map, const cv::Mat target, const cv::Point shift_pos, std::vector<cv::Point>& id_to_coor, const int* coor_to_id, const cv::Rect& frect, Eigen::MatrixXd& b);
	
	static void GenLaplaceMap(const cv::Mat& img, const std::vector<cv::Point>& id_to_coor, Eigen::MatrixXd& laplacemap);
	static void GenLaplaceMapMixed(const cv::Mat& sourceimg, const cv::Mat& targetimg, const cv::Point shift_pos, std::vector<cv::Point>& id_to_coor, Eigen::MatrixXd& laplacemap);


private:

	bool mixed_grad;

	bool coef_a_uptodate;

	cv::Mat mask;
	cv::Mat source;
	cv::Mat target;
	cv::Point pos;
	cv::Rect feasible_rect;

	int* coor_mapto_index;						//�ڵ������ӳ�䵽���
	std::vector<cv::Point> index_mapto_coor;	//�ڵ�ı��ӳ�䵽����
	Eigen::SparseMatrix<double>		coef_mat_a;
	Eigen::MatrixXd						vec_b;
	Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;

};