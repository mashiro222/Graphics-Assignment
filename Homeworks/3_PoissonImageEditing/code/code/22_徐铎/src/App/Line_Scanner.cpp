#include "Line_Scanner.h"
#include <iostream>

using namespace std;

Line_Scanner::Line_Scanner()
{
}

Line_Scanner::~Line_Scanner()
{
}

int Line_Scanner::min_int(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

vector<vector<int> > Line_Scanner::sortET(vector<vector<int> > ET)
{
	for (int k = 0; k < ET.size(); k++) 
	{
		if (!ET[k].empty())
		{
			for (int p = 1; p < ET[k].size(); p++)
			{
				int tmp = ET[k][p];
				for (int i = p - 1; i >= 0; i--)
				{
					if (ET[k][i] > tmp)
					{
						ET[k][i + 1] = ET[k][i];
					}
					else
					{
						ET[k][i + 1] = tmp;
						break;
					}
					if (i == 0) 
					{
						ET[k][0] = tmp;
					}
				}
			}
		}
	}
	return ET;
}

MatrixXd Line_Scanner::Scanner2Mat( int h,int w, vector<int> x_point, vector<int> y_point)
{
	MatrixXd selected=MatrixXd::Zero(h,w);
	//vector<vector<int> > point_list(h, vector<int>(0));
	vector<vector<tagEDGE> > NET(h, vector<tagEDGE>(0));

	//get NET from vertices
	for (size_t i = 0; i < y_point.size(); i++)
	{
		int front = (int)i - 1;
		if (front < 0)front += (int)y_point.size();
		if (y_point[front] > y_point[i])
		{
			tagEDGE tmp;
			tmp.x = x_point[i];
			tmp.dx = (x_point[front] - x_point[i]) / (double)(y_point[front] - y_point[i]);
			tmp.ymax = y_point[front];
			NET[y_point[i]].push_back(tmp);
		}
		if (y_point[front] < y_point[i])
		{
			tagEDGE tmp;
			tmp.x = x_point[front];
			tmp.dx = (x_point[front] - x_point[i]) / (double)(y_point[front] - y_point[i]);
			tmp.ymax = y_point[i];
			NET[y_point[front]].push_back(tmp);
		}
		if (y_point[front] == y_point[i])
		{
			int min = min_int(x_point[i], x_point[front]);
			int max = x_point[i] + x_point[front] - min;
			for (int j = min; j < max; j++)
			{
				selected(y_point[i], j) = 1;
			}
		}
	}																		// finish init NET
	// get ET from NET
	vector<vector<int> > ET(h, vector<int>(0));
	for (int i = 0; i < h; i++) 
	{
		if (!NET[i].empty())
		{
			for (int j = 0; j < NET[i].size(); j++)
			{
				double rx= NET[i][j].x;
				for (int k = i; k < NET[i][j].ymax; k++)
				{
					ET[k].push_back((int)round(rx));
					rx += NET[i][j].dx;
				}
			}
		}
	}

	ET = sortET(ET);
	// get region 
	for (int i = 0; i < h; i++)
	{
		if (!ET[i].empty())
		{
			for (int k = 0; k < ET[i].size(); k += 2)
			{
				for (int j = ET[i][k] + 1; j < ET[i][k + 1]; j++)
				{
					selected(i, j) = 1;
				}
			}
		}
	}
	// wipe off the region boundary
	for (size_t i = 0; i < y_point.size(); i++) 
	{
		int front = (int)i - 1;
		if (front < 0)front += (int)y_point.size();
		if (y_point[front] == y_point[i])
		{
			int min = min_int(x_point[i], x_point[front]);
			int max = x_point[i] + x_point[front] - min;
			for (int j = min; j < max; j++)
			{
				selected(y_point[i], j) = 0;
			}
		}
	}
	return selected;
}