#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run(WeightType type) {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	switch (type)
	{
	case Ubpa::kMean:
		Minimize();
		break;
	case Ubpa::kCot:
		cotMinimize();
		break;
	default:
		break;
	}

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	auto bound = heMesh->Boundaries()[0];
	cout << "Number of boundaries:" << bound.size() << endl;
	vector<int> bound_v_idx;

	for (auto p_v : bound)
	{
		bound_v_idx.push_back(heMesh->Index(p_v->End()));
	}

	std::sort(bound_v_idx.begin(), bound_v_idx.end());
	const auto& v = heMesh->Vertices();

	const auto mat_size = v.size() - bound_v_idx.size();
	SparseMatrix<double> mat(mat_size, mat_size);

	MatrixX3d right_ = Eigen::MatrixX3d::Zero(mat_size, 3);

	using std::cout;

	for (auto vert : v)
	{
		int index = idx_in_mat(bound_v_idx, heMesh->Index(vert));
		if (index != -1)
		{
			auto adj_v = vert->AdjVertices();
			double degree = adj_v.size();
			if (index >= mat_size)
			{
				cout << "index=" << index << "mat_size=" << mat_size << endl;
				system("pause");
			}
			else
			{
				mat.coeffRef(index, index) += 1;
			}
			for (auto v : adj_v)
			{
				int row_idx = idx_in_mat(bound_v_idx, heMesh->Index(v));
				if (row_idx == -1)
				{
					//cout<<"pos:" << v->pos << endl;
					for (int i = 0; i < 3; i++)
					{
						right_(index, i) += v->pos[i] / degree;
					}
					//cout << right_ << endl << endl;
				}
				else
				{
					if (index >= mat_size)
					{
						cout << "index:" << index << endl;
						system("pause");
					}
					else	if (row_idx >= mat_size)
					{
						cout << "row_index:" << row_idx << endl;
						system("pause");
					}
					else
					{
						mat.coeffRef(index, row_idx) -= 1 / degree;
					}
				}
			}
		}
	}
	//cout << mat << endl;

	//cout << right_ << endl;

	SparseLU<SparseMatrix<double>> solver;
	solver.compute(mat);

	if (solver.info() != Success)
	{
		cerr << "Unable to decompose the matrix" << endl;
	}

	auto result = solver.solve(right_);

	for (auto vert : v)
	{
		int idx = idx_in_mat(bound_v_idx, heMesh->Index(vert));
		if (idx != -1)
		{
			for (int i = 0; i < 3; i++)
			{
				vert->pos[i] = result(idx, i);
			}
		}
		else
		{
			cout << vert->pos << endl;
		}
	}
}

void Ubpa::MinSurf::cotMinimize()
{
	auto bound = heMesh->Boundaries()[0];
	cout << "Number of boundaries:" << bound.size() << endl;
	vector<int> bound_v_idx;

	for (auto p_v : bound)
	{
		bound_v_idx.push_back(heMesh->Index(p_v->End()));
	}

	std::sort(bound_v_idx.begin(), bound_v_idx.end());	
	const auto& v = heMesh->Vertices();

	const auto mat_size = v.size() - bound_v_idx.size();
	SparseMatrix<double> mat(mat_size, mat_size);

	MatrixX3d right_ = Eigen::MatrixX3d::Zero(mat_size, 3);

	using std::cout;

	for (auto vert : v)
	{
		int index = idx_in_mat(bound_v_idx, heMesh->Index(vert));
		if (index != -1)
		{
			auto ohes = vert->OutHEs();
			int degree = ohes.size();
			if (index >= mat_size)
			{
				cout << "index=" << index << "mat_size=" << mat_size << endl;
				system("pause");
			}
			else
			{
				mat.coeffRef(index, index) = 1;
			}

			vector<double> weight_vec;
			double sum = 0;

			vecf3 temp(0, 0, 0);

			for (auto ohe : ohes)
			{
				temp += ohe->End()->pos;
			}
			temp /= degree;

			for (auto ohe : ohes)
			{
				double weight = 0;
				auto e = ohe->Pair()->Pre();
				auto dir1 = e->Pre()->End()->pos - e->End()->pos;
				auto dir2 = e->Pre()->End()->pos - temp;

				double cos = dir1.dot(dir2) / dir2.norm() / dir1.norm();
				weight += cos / sqrt(1 - cos * cos);
				e = ohe->Next();
				dir1 = e->End()->pos - ohe->End()->pos;
				dir2 = e->End()->pos - temp;
				cos = dir1.dot(dir2) / dir2.norm() / dir1.norm();
				weight += cos / sqrt(1 - cos * cos);
				sum += weight;
				weight_vec.push_back(weight);
			}
			for (auto& c : weight_vec)
			{
				c /= sum;
			}

			for (int i = 0; i < ohes.size(); i++)
			{
				auto ohe = ohes[i];
				int row_idx = idx_in_mat(bound_v_idx, heMesh->Index(ohe->End()));

				auto v = ohe->End();

				if (row_idx == -1)
				{
					//cout<<"pos:" << v->pos << endl;
					for (int j = 0; j < 3; j++)
					{
						right_(index, j) += v->pos[j] * weight_vec[i];
					}
					//cout << right_ << endl << endl;
				}
				else
				{
					if (index >= mat_size)
					{
						cout << "index:" << index << endl;
						system("pause");
					}
					else	if (row_idx >= mat_size)
					{
						cout << "row_index:" << row_idx << endl;
						system("pause");
					}
					else
					{
						mat.coeffRef(index, row_idx) -= weight_vec[i];
					}
				}
			}
		}
	}
	//cout << mat << endl;

	//cout << right_ << endl;

	SparseLU<SparseMatrix<double>> solver;
	solver.compute(mat);

	if (solver.info() != Success)
	{
		cerr << "Unable to decompose the matrix" << endl;
	}

	auto result = solver.solve(right_);

	for (auto vert : v)
	{
		int idx = idx_in_mat(bound_v_idx, heMesh->Index(vert));
		if (idx != -1)
		{
			for (int i = 0; i < 3; i++)
			{
				vert->pos[i] = result(idx, i);
			}
		}
		else
		{
			cout << vert->pos << endl;
		}
	}
}

const int Ubpa::MinSurf::idx_in_mat(std::vector<int>& vec, int idx)
{
	int N = vec.size();
	int e = N - 1;
	int b = 0;

	if (idx > vec[e])
	{
		return idx - e - 1;
	}
	if (idx < vec[b])
	{
		return idx;
	}

	do
	{
		int mid = (b + e) / 2;
		if (idx == vec[mid])
		{
			return -1;//marks for boundary point, which means it's not in the matrix
		}
		else if (idx > vec[mid])
		{
			b = mid;
		}
		else
		{
			e = mid;
		}
	} while (e - b > 1);

	if (idx == vec[b])
	{
		return -1;
	}

	if (idx == vec[e])
	{
		return -1;
	}

	return idx - e;
}