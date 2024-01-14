#pragma once

#include "Cloning.h"

class NormalCloning : public Cloning
{
public:
	NormalCloning();
	~NormalCloning();

	NormalCloning(Domain* source_domain);

	void Clone(Domain* source_domain, cv::Point destination,
		cv::Mat* image_source, cv::Mat* image_destination);

private:
};