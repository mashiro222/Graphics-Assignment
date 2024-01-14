#ifndef _KD_TREE_H_
#define _KD_TREE_H_

#define lson (rt << 1)//左节点
#define rson (rt << 1 | 1)//右节点

#include <vector>
#include <algorithm>
#include <Eigen/Dense> 
#include <cmath>

using std::vector;
using Eigen::MatrixXf;

const int N = 50005;
const int k = 2; //2D-tree

struct Node {
	int feature[2];//feature[0] = x, feature[1] = y
	static int idx;
	Node(int x0, int y0) {
		feature[0] = x0;
		feature[1] = y0;
	}
	bool operator < (const Node &u) const {
		return feature[idx] < u.feature[idx];
	}
	//TOOD =hao
	Node() {
		feature[0] = 0;
		feature[0] = 0;
	}
};

class kdTree {
public:
	kdTree();
	~kdTree();
	void clean();
	void read_in(float* ary_x, float* ary_y, int len);
	void read_in(vector<MatrixXf> points_array);
	void build(int l, int r, int rt, int dept);
	int find_nearest_point(float x, float y, Node &res, int& dist, int boundary);
	int find_nearest_k_points(float x, float y, int k, vector<Node> &res, vector<int> &dist, int boundary);
	int distance(const Node& x, const Node& y);
private:
	void query(const Node& p, Node& res, int& dist, int rt, int dept);
	void query_k(const Node& p, int k, vector<int> &res, vector<int> &dist, int rt, int dept);
	vector<Node> _data;//用vector模拟数组
	vector<int> _flag;//判断是否存在 1, -1, 0
	int _idx;
	vector<Node> _find_nth;
};

#endif