#include "Cloning.h"
#include <iostream>

using std::cout;
using std::endl;

Cloning::Cloning()
{

}

Cloning::~Cloning()
{

}

void Cloning::getSparseMartrixA(Domain* source_domain)
{
	// Fill A with triplet
	int ord = 0;

	for (size_t i = 0; i < source_domain->point_list_.size(); i++)
	{
		for (size_t j = 0; j < source_domain->point_list_[i].size(); j++)
		{
			PointList q_interior = source_domain->neighbour_list_[i][j].interior_point_;
			
			A_triplet.push_back(Triplet<double>(ord, ord, 4));
			for (size_t k = 0; k < q_interior.size(); k++)
			{
				A_triplet.push_back(Triplet<double>(ord, source_domain->getOrd(q_interior[k]), -1));
			}
			ord++;
		}
	}

	SparseMatrix<double> A(ord, ord);
	A.setFromTriplets(A_triplet.begin(), A_triplet.end());

	solver.compute(A);
	if (solver.info() != Success)
	{
		cout << "Error:can't solve the equation" << endl;
		return;
	}
}




void Cloning::fillImage(Domain* source_domain, cv::Point destination,
	cv::Mat* image_destination, VectorXd& r, VectorXd& g, VectorXd& b)
{
	int ord = 0;

	for (size_t i = 0; i < source_domain->point_list_.size(); i++)
	{
		for (size_t j = 0; j < source_domain->point_list_[i].size(); j++)
		{
			cv::Point target = source_domain->Translate(source_domain->point_list_[i][j], destination);
			image_destination->at<cv::Vec3b>(target)[0] = fix(r(ord));
			image_destination->at<cv::Vec3b>(target)[1] = fix(g(ord));
			image_destination->at<cv::Vec3b>(target)[2] = fix(b(ord));
			//cout << int(r(ord)) << " " << int(g(ord)) << " " << int(b(ord)) << endl;
			ord++;
		}
	}
}

unsigned char Cloning::fix(double data)
{
	if (data > 255)
		return 255;
	else if (data < 0)
		return 0;
	else 
		return unsigned char(data);
}

bool Cloning::compare(cv::Vec3b p_s, cv::Vec3b q_s, cv::Vec3b p_d, cv::Vec3b q_d)
{
	double dist_s = pow(p_s[0] - q_s[0], 2) + pow(p_s[1] - q_s[1], 2) + pow(p_s[2] - q_s[2], 2);
	double dist_d = pow(p_d[0] - q_d[0], 2) + pow(p_d[1] - q_d[1], 2) + pow(p_d[2] - q_d[2], 2);
	
	if (dist_s > dist_d)
		return true;
	else
		return false;
}

bool Cloning::isAvailable(Domain* source_domain, cv::Point destination, cv::Mat* image_destination)
{
	if (destination.x + source_domain->getWidth() > image_destination->cols 
		|| destination.y + source_domain->getHeight() > image_destination->rows
		|| destination.x < 0 || destination.y < 0)
		return false;
	else
		return true;
}