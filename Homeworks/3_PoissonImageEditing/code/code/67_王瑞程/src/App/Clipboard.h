#pragma once

#include <opencv2/core/core.hpp>

class CClipboard
{
public:
	CClipboard()
	{
		isEmpty = true;
	}

	~CClipboard()
	{

	}

	bool isEmpty;

	cv::Mat clip_image;
	cv::Mat clip_mask;
};