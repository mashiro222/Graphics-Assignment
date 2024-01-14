#pragma once
#include <QWidget>
#include <vector>
#include "ImageWidget.h"
#include <Eigen/Core>
#include <Eigen/SparseCholesky>

class QPoint;
class QRect;
class ImageWidget;

class CPoissonEdit
{
protected:
    ImageWidget		*imagewidget_s;
    ImageWidget		*imagewidget_;
    QRect			*source_rect_region_;
    Eigen::MatrixXi *in_points_s;
    Eigen::MatrixXi poissoned_rgb_;

    int in_points_num;
    std::vector<QPoint> index_to_coor_;
    Eigen::MatrixXi coor_to_index_;

    Eigen::SparseMatrix<double> A_;
    Eigen::MatrixX3d b_;
	Eigen::SimplicialLLT<Eigen::SparseMatrix<double> > LLT_;

	int width_s();
	int height_s();
    void buildAMat();
    void buildbMat(int, int);
    virtual int v(QPoint, QPoint, int, QPoint) = 0;

public:
	CPoissonEdit(void);
	virtual ~CPoissonEdit(void);

	void init(ImageWidget *, ImageWidget *);
    Eigen::MatrixXi GetPoissonedImage(int, int);
};

