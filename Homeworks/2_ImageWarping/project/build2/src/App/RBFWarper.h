#pragma once
#include "ImagePoint.h"
#include <Eigen/Dense>
#include <vector>

class RBFWarper 
{
public:

    const IPoint& func(IPoint& p);
    void initialize(std::vector<IMap> points);

    static RBFWarper* getInstance();

protected:
    std::vector<Eigen::Vector2d> alpha;
    std::vector<IMap> point_;
    size_t pairs;

    Eigen::Matrix2d A;
    Eigen::Vector2d b;

    static RBFWarper inst;
};