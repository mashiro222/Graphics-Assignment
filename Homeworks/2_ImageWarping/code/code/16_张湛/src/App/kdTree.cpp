#include "kdTree.h"

int Node::idx = 0;

kdTree::kdTree() 
{
	_data.reserve(N * 4);
	_flag.reserve(N * 4);//TODO init
}

kdTree::~kdTree() 
{
}

void kdTree::read_in(float* ary_x, float* ary_y, int len) 
{
	_find_nth.reserve(N * 4);
	for (int i = 0; i < len; i++) {
		_find_nth.push_back(Node(ary_x[i], ary_y[i]));
	}
	for (int i = 0; i < N * 4; i++) {
		Node tmp;
		_data.push_back(tmp);
		_flag.push_back(0);
	}
	build(0, len - 1, 1, 0);
}

void kdTree::read_in(vector<MatrixXf> points_array)
{
	int len = points_array.size();
	_find_nth.reserve(N * 4);
	for (int i = 0; i < len; i++) {
		_find_nth.push_back(Node(int(points_array[i](0, 0)), int(points_array[i](1, 0))));
	}
	for (int i = 0; i < N * 4; i++) {
		Node tmp;
		_data.push_back(tmp);
		_flag.push_back(0);
	}
	build(0, len - 1, 1, 0);
}

void kdTree::clean() 
{
	_find_nth.clear();
	_data.clear();
	_flag.clear();
}

//����kd-tree
void kdTree::build(int l, int r, int rt, int dept) 
{
	if (l > r) return;
	_flag[rt] = 1;                  //��ʾ���Ϊrt�Ľڵ����
	_flag[lson] = _flag[rson] = -1; //��ǰ�ڵ�ĺ�����ʱ��ǲ����� 
	int mid = (l + r + 1) >> 1;
	Node::idx = dept % k;           //���ձ��Ϊidx�����Խ��л���
	std::nth_element(_find_nth.begin() + l, _find_nth.begin() + mid, _find_nth.begin() + r + 1);
	_data[rt] = _find_nth[mid];
	build(l, mid - 1, lson, dept + 1); //�ݹ�������
	build(mid + 1, r, rson, dept + 1);
}

int kdTree::find_nearest_point(float x, float y, Node &res, int& dist, int boundary) 
{
	Node p(x, y);
	dist = boundary * boundary;
	query(p, res, dist, 1, 0);
	return 1;
}

int kdTree::find_nearest_k_points(float x, float y, int k, vector<Node>& res, vector<int>& dist, int boundary)
{
	vector<int> idx;
	idx.clear();
	res.clear();
	dist.clear();

	Node p(x, y);
	for (int i = 0; i < k; i++) {
		idx.push_back(-1);
		dist.push_back(boundary * boundary);
		query_k(p, i, idx, dist, 1, 0);
		if (idx[i] < 0) break;
		_flag[idx[i]] = 0;
		res.push_back(_data[idx[i]]);
	}

	int kfind = res.size();

	for (int i = 0; i < kfind; i++) {
		_flag[idx[i]] = 1;
	}

	return kfind;
}

//����kd-tree����p����ĵ�
void kdTree::query(const Node& p, Node& res, int& dist, int rt, int dept) 
{
	if (_flag[rt] == -1) return; //�����ڵĽڵ㲻����
	int tmp_dist = distance(_data[rt], p);
	bool fg = false; //���ڱ���Ƿ���Ҫ����������
	int dim = dept % k; //�ͽ���һ��, ��֤��ͬ�ڵ��dimֵ����
	int x = lson;
	int y = rson;

	if (p.feature[dim] >= _data[rt].feature[dim]) std::swap(x, y);  //����p�ĵ�dim������ֵ���ڵ��ڵ�ǰ�����ݣ�����Ҫ����������
	if (x < _flag.size() && _flag[x] != -1) query(p, res, dist, x, dept + 1); //�ڵ�x����, �����������������
	if (tmp_dist < dist) { //����ҵ���С�ľ���, ���滻Ŀǰ�Ľ��dist
		res = _data[rt];
		dist = tmp_dist;
	}
	tmp_dist = (p.feature[dim] - _data[rt].feature[dim]) * (p.feature[dim] - _data[rt].feature[dim]);
	if (tmp_dist < dist) fg = true; //����Ҫ��������
	if (y < _flag.size() && _flag[y] != -1 && fg) query(p, res, dist, y, dept + 1);
}

void kdTree::query_k(const Node & p, int i, vector<int>& res, vector<int>& dist, int rt, int dept)
{
	if (_flag[rt] == -1) return; //�����ڵĽڵ㲻����
	int tmp_dist = distance(_data[rt], p);
	bool fg = false; //���ڱ���Ƿ���Ҫ����������
	int dim = dept % k; //�ͽ���һ��, ��֤��ͬ�ڵ��dimֵ����
	int x = lson;
	int y = rson;

	if (p.feature[dim] >= _data[rt].feature[dim]) std::swap(x, y);  //����p�ĵ�dim������ֵ���ڵ��ڵ�ǰ�����ݣ�����Ҫ����������
	if (x<_flag.size() && _flag[x]==1) query_k(p, i, res, dist, x, dept + 1); //�ڵ�x����, �����������������
	if (tmp_dist < dist[i] && _flag[rt] == 1) { //����ҵ���С�ľ���, ���滻Ŀǰ�Ľ��dist
		res[i] = rt;
		dist[i] = tmp_dist;
	}
	tmp_dist = (p.feature[dim] - _data[rt].feature[dim]) * (p.feature[dim] - _data[rt].feature[dim]);
	if (tmp_dist < dist[i]) fg = true; //����Ҫ��������
	if (y < _flag.size() && _flag[y] == 1 && fg) query_k(p, i, res, dist, y, dept + 1);
}

//���������ľ����ƽ��
int kdTree::distance(const Node& x, const Node& y) 
{
	int res = 0;
	for (int i = 0; i < k; i++) 
	{
		res += (x.feature[i] - y.feature[i]) * (x.feature[i] - y.feature[i]);
	}
	return res;
}
