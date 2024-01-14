#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>


#include <Eigen/SparseCholesky>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	is_boundary = nullptr;
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
	delete[] is_boundary;
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

	is_boundary = new bool[heMesh->NumVertices()];

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}
	GenConstraint(0);
	GenWeight();
	GenVecb();

	Minimize();

	// 转换 half-edge structure -> triangle mesh
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
	// TODO

	size_t nV = heMesh->NumVertices();

	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	Eigen::MatrixXf result;
	solver.compute(weight_mat);
	result = solver.solve(vec_b);


	for (int i = 0; i < nV; i++)
	{
		heMesh->Vertices()[i]->pos = Ubpa::vecf3(result(i, 0), result(i, 1), result(i, 2));
	}
}

void Ubpa::MinSurf::GenWeight()
{
	//获得系数矩阵.由于非对称，后面应当用LU方法求解的
	size_t nV = heMesh->NumVertices();
	weight_mat.resize(nV, nV);
	std::vector<Eigen::Triplet<float>> coef_elem_list;
	for (auto v : heMesh->Vertices())
	{
		size_t v_idx = heMesh->Index(v);
		if (is_boundary[v_idx])
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, 1.f));
		}
		else
		{
			std::vector<V*> adjVs = v->AdjVertices();
			coef_elem_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, (float)(adjVs.size())));
			for (auto u : adjVs)
			{
				coef_elem_list.push_back(Eigen::Triplet<float>(v_idx, heMesh->Index(u), -1.f));
			}
		}
	}
	weight_mat.setFromTriplets(coef_elem_list.begin(), coef_elem_list.end());
}

void Ubpa::MinSurf::GenVecb()
{
	//获得向量
	vec_b = Eigen::MatrixXf(heMesh->NumVertices(), 3);
	vec_b.fill(0.f);
	for (auto v : heMesh->Vertices())
	{
		size_t v_idx = heMesh->Index(v);
		if (is_boundary[v_idx])
		{
			vec_b(v_idx, 0) = v->pos[0];
			vec_b(v_idx, 1) = v->pos[1];
			vec_b(v_idx, 2) = v->pos[2];
		}
	}
}

void Ubpa::MinSurf::GenConstraint(const int atype)
{
	std::vector<Ubpa::THalfEdge<V, E, P>*> bd;
	memset(is_boundary, 0, sizeof(bool)*heMesh->NumVertices());
	switch (atype)
	{
	case 0:
		for (auto v : heMesh->Vertices())
		{
			if (v->IsBoundary())
				is_boundary[heMesh->Index(v)] = true;
		}
		break;
	case 1:
		bd = GetLargestBoundary();
		for (auto e : bd)
		{
			is_boundary[heMesh->Index(e->End())] = true;
		}
		break;
	default:
		break;
	}
}

std::vector<THalfEdge<MinSurf::V, MinSurf::E, MinSurf::P>*> Ubpa::MinSurf::GetLargestBoundary()
{
	auto bd = heMesh->Boundaries();
	size_t m = 0;
	for (size_t i = 0; i < bd.size(); i++)
	{
		if (bd[i].size() > bd[m].size())
			m = i;
	}
	return bd[m];
}
