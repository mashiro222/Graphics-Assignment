#include "NormalCloning.h"

NormalCloning::NormalCloning()
{

}

NormalCloning::~NormalCloning()
{

}

NormalCloning::NormalCloning(Domain* source_domain)
{
	getSparseMartrixA(source_domain);
}

void NormalCloning::Clone(Domain* source_domain, cv::Point destination,
	cv::Mat* image_source, cv::Mat* image_destination)
{
	if (!isAvailable(source_domain, destination, image_destination))
		return;

	cv::Point tmp_point_;

	for (size_t i = 0; i < source_domain->point_list_.size(); i++)
	{
		for (size_t j = 0; j < source_domain->point_list_[i].size(); j++)
		{
			tmp_point_ = source_domain->point_list_[i][j];
			image_destination->at<cv::Vec3b>(source_domain->Translate(tmp_point_, destination))
				= image_source->at<cv::Vec3b>(tmp_point_);
		}
	}
}