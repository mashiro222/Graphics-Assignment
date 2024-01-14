#include "RBFWarper.h"
#include <math.h>
#include <vector>
#include <iostream>

#define DIST_1(x, y, pix, piy) (( sqrt( (x-pix)*(x-pix) + (y-piy)*(y-piy)) ))

RBFWarper RBFWarper::inst;

RBFWarper* RBFWarper::getInstance() {
    return &(inst);
}

//注：在RBF中我实现了两种方法

//void RBFWarper::initialize(std::vector<IMap> points) {
//	//初始化赋值
//	pairs = points.size();
//	point_ = points;
//	alpha.clear();
//	alpha = std::vector<Vector2d>(pairs);
//	//std::vector<std::vector<double>> R(pairs, std::vector<double>(pairs));
//	MatrixXd R(pairs, pairs);
//	//std::vector<double> px(pairs); std::vector<double> py(pairs);
//	//std::vector<double> qx(pairs); std::vector<double> qy(pairs);
//	//std::vector<Vector2d> p(pairs);
//	//std::vector<Vector2d> q(pairs);
//
//	//计入pq
//	//for (int i = 0; i < pairs; i++) {
//	//	p[i][0] = point_[i].first.X(); p[i][1] = point_[i].first.Y();
//	//	q[i][0] = point_[i].second.X(); q[i][1] = point_[i].second.Y();
//	//}
//	//算R
//	for (int i = 0; i < pairs; i++) {
//		for (int j = 0; j < pairs; j++) {
//			R(i, j) = sqrt((point_[i].first.X() - point_[j].first.X()) * (point_[i].first.X() - point_[j].first.X()) +
//				(point_[i].first.Y() - point_[j].first.Y()) * (point_[i].first.Y() - point_[j].first.Y()));
//		}
//	}
//	//设计方程
//	MatrixXd M(2 * pairs + 6, 2 * pairs + 6);
//	M.setZero();
//	//VectorXd X(2 * pairs + 6);
//	VectorXd B(2 * pairs + 6);
//	B.setZero();
//	//构造系数矩阵
//	for (int i = 0; i < pairs; i++) {
//		for (int j = 0; j < pairs; j++) {
//			M(i, j) = R(i, j);
//			M(i + pairs, j + pairs) = R(i, j);
//		}
//		M(i, 2 * pairs) = point_[i].first.X();
//		M(i, 2 * pairs + 1) = point_[i].first.Y();
//		M(i, 2 * pairs + 4) = 1;
//		M(i + pairs, 2 * pairs + 2) = point_[i].first.X();
//		M(i + pairs, 2 * pairs + 3) = point_[i].first.Y();
//		M(i + pairs, 2 * pairs + 5) = 1;
//
//		B(i) = point_[i].second.X();
//		B(i + pairs) = point_[i].second.Y();
//	}
//	////构造B
//	//for (int i = 0; i < pairs; i++) {
//	//	B(i) = q[i][0];
//	//	B(i + pairs) = q[i][1];
//	//}
//	//求解出X
//	VectorXd X = M.colPivHouseholderQr().solve(B);
//	
//	//赋值
//	for (int i = 0; i < pairs; i++) {
//		alpha[i][0] = X(i);
//		alpha[i][1] = X(i + pairs);
//	}
//	A(0, 0) = X(2 * pairs);
//	A(0, 1) = X(2 * pairs + 1);
//	A(1, 0) = X(2 * pairs + 2);
//	A(1, 1) = X(2 * pairs + 3);
//	b(0) = X(2 * pairs + 4);
//	b(1) = X(2 * pairs + 5);	
//}

void RBFWarper::initialize(std::vector<IMap> points) {
    //初始化
    pairs = points.size();
    A = Eigen::Matrix2d::Identity();
    b = Eigen::Vector2d::Zero();

    //求出A和b的方程
    Eigen::MatrixXd M(6, 6);
    Eigen::VectorXd B(6);
    M = Eigen::MatrixXd::Zero(6, 6);
    B = Eigen::VectorXd::Zero(6);

    for (int i = 0; i < pairs; i++) {
        double pix = points[i].first.X();
        double piy = points[i].first.Y();
        double qix = points[i].second.X();
        double qiy = points[i].second.Y();

        M(0, 0) += 2 * pix * pix;
        M(0, 1) += 2 * pix * piy;
        M(0, 4) += 2 * pix;
        B(0) += 2 * pix * qix;

        M(1, 0) += 2 * pix * piy;
        M(1, 1) += 2 * piy * piy;
        M(1, 4) += 2 * piy;
        B(1) += 2 * qix * piy;

        M(2, 2) += 2 * pix * pix;
        M(2, 3) += 2 * pix * piy;
        M(2, 5) += 2 * pix;
        B(2) += 2 * pix * qiy;

        M(3, 2) += 2 * pix * piy;
        M(3, 3) += 2 * piy * piy;
        M(3, 5) += 2 * piy;
        B(3) += 2 * piy * qiy;

        M(4, 0) += 2 * pix;
        M(4, 1) += 2 * piy;
        M(4, 4) += 2;
        B(4) += 2 * qix;

        M(5, 2) += 2 * pix;
        M(5, 3) += 2 * piy;
        M(5, 5) += 2;
        B(5) += 2 * qiy;
    }

    Eigen::VectorXd result1 = M.colPivHouseholderQr().solve(B);
    A(0, 0) = result1(0);
    A(0, 1) = result1(1);
    A(1, 0) = result1(2);
    A(1, 1) = result1(3);
    b(0) = result1(4);
    b(1) = result1(5);

    //求出alpha
    Eigen::MatrixXd Alpha(2 * pairs, 2 * pairs);
    Alpha = Eigen::MatrixXd::Zero(2 * pairs, 2 * pairs);
    for (int i = 0; i < 2 * pairs; i = i + 2) {
        for (int j = 0; j < 2 * pairs; j = j + 2) {
            double pix = points[i / 2].first.X();
            double piy = points[i / 2].first.Y();
            double pjx = points[j / 2].first.X();
            double pjy = points[j / 2].first.Y();
            Alpha(i, j) = DIST_1(pjx, pjy, pix, piy);
            Alpha(i + 1, j + 1) = Alpha(i, j);
        }
    }

    Eigen::VectorXd b_alpha(2 * pairs);
    for (int i = 0; i < 2 * pairs; i = i + 2) {
        Eigen::Vector2d p_vec;
        p_vec(0) = points[i / 2].first.X();
        p_vec(1) = points[i / 2].first.Y();

        Eigen::Vector2d offset = A * p_vec + b;

        b_alpha(i) = points[i / 2].second.X() - offset(0);
        b_alpha(i + 1) = points[i / 2].second.Y() - offset(1);
    }

    Eigen::VectorXd result2 = Alpha.colPivHouseholderQr().solve(b_alpha);
    alpha = std::vector<Eigen::Vector2d>(pairs);

    for (int i = 0; i < 2 * pairs; i = i + 2) {
        alpha[i / 2](0) = result2(i);
        alpha[i / 2](1) = result2(i + 1);
    }

    point_ = points;

    //// 检测
    //for (int i = 0; i < pairs; i++) {
    //    printf("i = %d, (%d, %d) ->", i, point_[i].first.X(), point_[i].first.Y());
    //    IPoint pnt = func(point_[i].first);
    //    printf("(%d, %d) [expect (%d, %d)]\n",
    //        pnt.X(), pnt.Y(), point_[i].second.X(), point_[i].second.Y());
    //}
}

//const IPoint& RBFWarper::func(IPoint& p) {
//	double px = (double)p.X();
//	double py = (double)p.Y();
//	double fx = 0; double fy = 0;
//	double R;
//	for (int i = 0; i < pairs; i++) {
//		R = sqrt(((px - point_[i].first.X()) * (px - point_[i].first.X()) + (py - point_[i].first.Y()) * (py - point_[i].first.Y())));
//		fx += alpha[i][0] * R;
//		fy += alpha[i][1] * R;
//	}
//	fx += A(0, 0) * px + A(0, 1) * py + b(0);
//	fx += A(1, 0) * px + A(1, 1) * py + b(1);
//
//	IPoint result;
//	result.setX((int)fx).setY((int)fy);
//	return result;
//}

const IPoint& RBFWarper::func(IPoint& p) {
    double px = p.X();
    double py = p.Y();

    Eigen::Vector2d fp = Eigen::Vector2d::Zero();
    for (int i = 0; i < pairs; i++) {
        double pix = point_[i].first.X();
        double piy = point_[i].first.Y();
        fp += alpha[i] * DIST_1(px, py, pix, piy);
    }

    Eigen::Vector2d p_vec;
    p_vec(0) = px;
    p_vec(1) = py;
    fp += A * p_vec + b;

    IPoint ret;
    ret.setX((int)fp(0)).setY((int)fp(1));

    return ret;
}