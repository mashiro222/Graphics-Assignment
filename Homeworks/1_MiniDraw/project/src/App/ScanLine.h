#pragma once
#include <Eigen/Dense>
#include <vector>
using namespace Eigen;
using namespace std;

struct Edge
{
	double x;
	double dx;
	int ymax;
};

class ScanLine
{
public:
	ScanLine();
	~ScanLine();

	MatrixXd Scanner2Mat(int w, int h, vector<int> x_point, vector<int> y_point);			// scanner to get region
private:
	int min_int(int a, int b);
	vector<vector<int>> sortET(vector<vector<int>> ET);


};