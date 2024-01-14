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
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}
	// triangle mesh's positions ->  half-edge structure's positions


	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	//Minimize();

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
	cout << "test" << endl;
	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	Laplace();
	Solve();

	cout << "INFO::MinSurf::Minimize:" << endl
		<< "\t" << "Transform Success!" << endl;
}

void MinSurf::Laplace() {
	size_t nV = heMesh->NumVertices();
	//Sparse Matrix use triplet to evaluate
	vector<Eigen::Triplet<double>> tripletlist;

	for (size_t i = 0; i < nV; i++) {
		V* vertex = heMesh->Vertices()[i];
		tripletlist.push_back(Eigen::Triplet<double>(i, i, 1));
		if (!vertex->IsBoundary()) {
			double degree = vertex->AdjVertices().size();
			for (size_t j = 0; j < degree; j++) {
				tripletlist.push_back(Eigen::Triplet<double>(i, heMesh->Index(vertex->AdjVertices()[j]), -1 / degree));
			}
		}
	}
	
	L.resize(nV, nV);
	L.setZero();
	L.setFromTriplets(tripletlist.begin(), tripletlist.end());
	//L.makeCompressed();
}

void MinSurf::Solve() {
	//first do decomposition
	solver.compute(L);

	//solve the problem
	size_t nV = heMesh->NumVertices();
	VectorXd x(nV), bx(nV), y(nV), by(nV), z(nV), bz(nV);
	bx.setZero();
	by.setZero();
	bz.setZero();
	for (size_t i = 0; i < nV; i++) {
		V* vertex = heMesh->Vertices()[i];
		if (vertex->IsBoundary()) {
			bx(i) = vertex->pos.at(0);
			by(i) = vertex->pos.at(1);
			bz(i) = vertex->pos.at(2);
		}
	}
	x = solver.solve(bx);
	y = solver.solve(by);
	z = solver.solve(bz);

	//evaluate
	//for (size_t i = 0; i < nV; i++) {
	//	heMesh->Vertices()[i]->pos.at(0) = x(i);
	//	heMesh->Vertices()[i]->pos.at(1) = y(i);
	//	heMesh->Vertices()[i]->pos.at(2) = z(i);
	//}
}
