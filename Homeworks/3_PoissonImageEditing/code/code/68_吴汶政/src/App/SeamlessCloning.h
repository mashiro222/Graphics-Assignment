#pragma once

#include "Cloning.h"

class SeamlessCloning : public Cloning
{
public:
	SeamlessCloning();
	~SeamlessCloning();

	SeamlessCloning(Domain* source_domain);

	void Clone(Domain* source_domain, cv::Point destination,
		cv::Mat* image_source, cv::Mat* image_destination);

private:
};