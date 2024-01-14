#pragma once
#include "ImagePoint.h"
#include <vector>

//不知道怎么继承啊
class IDWWarper 
{
public:
	
	const IPoint& func(IPoint& p);
	void initialize(std::vector<IMap> points);

	using DMatrix = struct {
		double d11;
		double d12;
		double d21;
		double d22;
	};

	static IDWWarper* getInstance();
	
private:
	std::vector<DMatrix> D; 
	size_t pairs;
	std::vector<IMap> point_;

	static IDWWarper inst;
};