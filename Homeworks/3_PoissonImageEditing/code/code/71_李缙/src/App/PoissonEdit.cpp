#include "PoissonEdit.h"
#include <iostream>

CPoissonEdit::CPoissonEdit(void)
	:imagewidget_s(NULL),imagewidget_(NULL),source_rect_region_(NULL),in_points_s(NULL),
	in_points_num(0)
{
}


CPoissonEdit::~CPoissonEdit(void)
{
}

void CPoissonEdit::init(ImageWidget *src_imgwdg, ImageWidget *imgwdg)
{
	in_points_num = 0;
	imagewidget_s = src_imgwdg;
	imagewidget_ = imgwdg;

	source_rect_region_ = &(imagewidget_s->rect_or_ellipse_);
	in_points_s = &(imagewidget_s->in_points);

	poissoned_rgb_.resize(width_s(),height_s());
	coor_to_index_.resize(width_s(),height_s());
	index_to_coor_.clear();
	
	for (int i=0;i<width_s();i++)
		for (int j=0;j<height_s();j++)
			if (1 == (*in_points_s)(i,j))
			{
				coor_to_index_(i,j) = in_points_num++;
				index_to_coor_.push_back(QPoint(i,j));
			}
	if (in_points_num)
		buildAMat();
}

int CPoissonEdit::width_s()
{
	return imagewidget_s->width();
}

int CPoissonEdit::height_s()
{
	return imagewidget_s->height();
}

void CPoissonEdit::buildAMat()
{

	A_.resize(in_points_num,in_points_num);

	std::vector<Eigen::Triplet<double> > coefficients;
	for (int i=0;i<in_points_num;i++)
	{
		QPoint coor = index_to_coor_[i];
		int x = coor.x();
		int y = coor.y();
		int np = 4;
		if (0 == x || width_s()-1 == x)
			np--;
		if (0 == y || height_s()-1 == y)
			np--;
		coefficients.push_back(Eigen::Triplet<double>(i,i,np));

		if (x>0 && 1==(*in_points_s)(x-1,y))
			coefficients.push_back(Eigen::Triplet<double>(i,coor_to_index_(x-1,y),-1));
		if (x<width_s()-1 && 1==(*in_points_s)(x+1,y))
			coefficients.push_back(Eigen::Triplet<double>(i,coor_to_index_(x+1,y),-1));
		if (y>0 && 1==(*in_points_s)(x,y-1))
			coefficients.push_back(Eigen::Triplet<double>(i,coor_to_index_(x,y-1),-1));
		if (y<height_s()-1 && 1==(*in_points_s)(x,y+1))
			coefficients.push_back(Eigen::Triplet<double>(i,coor_to_index_(x,y+1),-1));
	}

	A_.setFromTriplets(coefficients.begin(),coefficients.end());

	LLT_.compute(A_);

}

Eigen::MatrixXi CPoissonEdit::GetPoissonedImage(int xpos, int ypos)
{
    clock_t start,end1,end2,end3;
    start=clock();
	if (!in_points_num)
		return poissoned_rgb_;
	buildbMat(xpos,ypos);
    end1=clock();
	Eigen::MatrixX3d X;
	X = LLT_.solve(b_);
    end2=clock();
	for (int i=0;i<in_points_num;i++)
	{
		QPoint coor = index_to_coor_[i];
		int r,g,b;
		r = (int)X(i,0);
		r = (r<0)?0:r;
		r = (r>255)?255:r;
		g = (int)X(i,1);
		g = (g<0)?0:g;
		g = (g>255)?255:g;
		b = (int)X(i,2);
		b = (b<0)?0:b;
		b = (b>255)?255:b;
		poissoned_rgb_(coor.x(),coor.y()) = qRgb(r,g,b);
	}
    end3=clock();
	return poissoned_rgb_;
}

void CPoissonEdit::buildbMat(int xpos, int ypos)
{
    clock_t start,end1,end2,end3;
    start=clock();
	QPoint delta[4] = {QPoint(-1,0),QPoint(1,0),QPoint(0,-1),QPoint(0,1)};
	b_ = Eigen::MatrixX3d::Zero(in_points_num,3);
    end1=clock();
	for (int i=0;i<in_points_num;i++)
	{
        end2=clock();
		QPoint coor = index_to_coor_[i];
		int x = coor.x();
		int y = coor.y();

		for (int j=0;j<4;j++)
		{
			QPoint q = coor + delta[j];
			if (q.x()>=0 && q.x()<width_s() && q.y()>=0 && q.y()<height_s())
			{
				b_(i,0) += v(coor,q,0,QPoint(xpos,ypos));
				b_(i,1) += v(coor,q,1,QPoint(xpos,ypos));
				b_(i,2) += v(coor,q,2,QPoint(xpos,ypos));
				if ( 2== (*in_points_s)(q.x(),q.y()))
				{
                    b_(i,0) += qRed(imagewidget_->image().pixel(q-source_rect_region_->topLeft()+QPoint(xpos,ypos)));
                    b_(i,1) += qGreen(imagewidget_->image().pixel(q-source_rect_region_->topLeft()+QPoint(xpos,ypos)));
                    b_(i,2) += qBlue(imagewidget_->image().pixel(q-source_rect_region_->topLeft()+QPoint(xpos,ypos)));
				}
			}
		}
	}
}
