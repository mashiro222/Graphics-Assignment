#pragma once
#include <Eigen/dense>
#include <vector>
using namespace Eigen;
using namespace std;

struct tagEDGE
{
	double x;
	double dx;
	int ymax;
};

class Line_Scanner
{
public:
	Line_Scanner();
	~Line_Scanner();

	MatrixXd Scanner2Mat(int w,int h,vector<int> x_point,vector<int> y_point);			// scanner to get region
private:
	int min_int(int a, int b);	
	vector<vector<int> > sortET(vector<vector<int> > ET);

	
};