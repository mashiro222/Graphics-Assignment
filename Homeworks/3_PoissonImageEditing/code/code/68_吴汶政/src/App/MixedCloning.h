#pragma once

#include "Cloning.h"

class MixedCloning : public Cloning
{
public:
	MixedCloning();
	~MixedCloning();

	MixedCloning(Domain* source_domain);

	void Clone(Domain* source_domain, cv::Point destination,
		cv::Mat* image_source, cv::Mat* image_destination);

private:
};