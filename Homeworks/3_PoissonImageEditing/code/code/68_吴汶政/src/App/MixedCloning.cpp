#include "MixedCloning.h"

#include <iostream>

using std::cout;
using std::endl;

MixedCloning::MixedCloning()
{

}

MixedCloning::~MixedCloning()
{

}

MixedCloning::MixedCloning(Domain* source_domain)
{
	getSparseMartrixA(source_domain);
}

void MixedCloning::Clone(Domain* source_domain, cv::Point destination,
	cv::Mat* image_source, cv::Mat* image_destination)
{
	if (!isAvailable(source_domain, destination, image_destination))
		return;

	// Get three bs for the equation
	size_t dim = source_domain->getSize();

	VectorXd br = VectorXd::Zero(dim);
	VectorXd bg = VectorXd::Zero(dim);
	VectorXd bb = VectorXd::Zero(dim);

	int ord = 0;

	for (size_t i = 0; i < source_domain->point_list_.size(); i++)
	{
		for (size_t j = 0; j < source_domain->point_list_[i].size(); j++)
		{
			// Get neighbour
			PointList q_interior = source_domain->neighbour_list_[i][j].interior_point_;
			PointList q_boundary = source_domain->neighbour_list_[i][j].boundary_point_;

			for (size_t k = 0; k < q_interior.size(); k++)
			{
				cv::Vec3b p_source_data = image_source->at<cv::Vec3b>(source_domain->point_list_[i][j]);
				cv::Vec3b q_source_data = image_source->at<cv::Vec3b>(q_interior[k]);
				cv::Vec3b p_destination_data = image_destination->at<cv::Vec3b>(source_domain->Translate(source_domain->point_list_[i][j], destination));
				cv::Vec3b q_destination_data = image_destination->at<cv::Vec3b>(source_domain->Translate(q_interior[k], destination));
				if (compare(p_source_data, q_source_data, p_destination_data, q_destination_data))
				{
					br(ord) += p_source_data[0] - q_source_data[0];
					bg(ord) += p_source_data[1] - q_source_data[1];
					bb(ord) += p_source_data[2] - q_source_data[2];
				}
				else
				{
					br(ord) += p_destination_data[0] - q_destination_data[0];
					bg(ord) += p_destination_data[1] - q_destination_data[1];
					bb(ord) += p_destination_data[2] - q_destination_data[2];
				}
			}
			for (size_t k = 0; k < q_boundary.size(); k++)
			{
				cv::Vec3b p_source_data = image_source->at<cv::Vec3b>(source_domain->point_list_[i][j]);
				cv::Vec3b q_source_data = image_source->at<cv::Vec3b>(q_boundary[k]);
				cv::Vec3b p_destination_data = image_destination->at<cv::Vec3b>(source_domain->Translate(source_domain->point_list_[i][j], destination));
				cv::Vec3b q_destination_data = image_destination->at<cv::Vec3b>(source_domain->Translate(q_boundary[k], destination));
				br(ord) += q_destination_data[0];
				bg(ord) += q_destination_data[1];
				bb(ord) += q_destination_data[2];
				if (compare(p_source_data, q_source_data, p_destination_data, q_destination_data))
				{
					br(ord) += p_source_data[0] - q_source_data[0];
					bg(ord) += p_source_data[1] - q_source_data[1];
					bb(ord) += p_source_data[2] - q_source_data[2];
				}
				else
				{
					br(ord) += p_destination_data[0] - q_destination_data[0];
					bg(ord) += p_destination_data[1] - q_destination_data[1];
					bb(ord) += p_destination_data[2] - q_destination_data[2];
				}
			}
			ord++;
		}
	}

	if (solver.info() != Success)
	{
		cout << "Error:can't solve the equation" << endl;
		return;
	}
	VectorXd r = solver.solve(br);
	VectorXd g = solver.solve(bg);
	VectorXd b = solver.solve(bb);

	fillImage(source_domain, destination, image_destination, r, g, b);
}
