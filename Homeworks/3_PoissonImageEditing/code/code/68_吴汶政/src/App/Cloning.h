#pragma once

#include "Domain.h"
#include <Eigen/Dense>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <vector>

using namespace Eigen;

class Cloning
{
public:
	Cloning();
	~Cloning();

	virtual void Clone(Domain *source_domain, cv::Point destination, 
		cv::Mat *image_source, cv::Mat *image_destination) = 0;					// Clone 

	void getSparseMartrixA(Domain *source_domain);								// Compute A
	void fillImage(Domain* source_domain, cv::Point destination,
		cv::Mat* image_destination, VectorXd &r, VectorXd &g, VectorXd &b);		

protected:
	unsigned char fix (double data);											// Value in [0, 255]
	bool compare(cv::Vec3b p_s, cv::Vec3b q_s, cv::Vec3b p_d, cv::Vec3b q_d);	// Compare two distances of vectors
	bool isAvailable(Domain* source_domain, cv::Point destination, cv::Mat* image_destination);
																				// Check the destination is in the image

	std::vector<Triplet<double>> A_triplet;
	SimplicialLDLT<SparseMatrix<double>> solver;
};