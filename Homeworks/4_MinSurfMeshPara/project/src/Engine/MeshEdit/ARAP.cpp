#include <Engine/MeshEdit/ASAP.h>

#include <Engine/MeshEdit/ARAP.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

#include <Engine/MeshEdit/Paramaterize.h>

using namespace std;
using namespace Ubpa;
using namespace Eigen;

ARAP::ARAP(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void ARAP::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool ARAP::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::ARAP::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}
	cout << "test" << endl;
	auto asap = ASAP::New(triMesh);
	asap->Run();
	//auto para = Paramaterize::New(triMesh);
	//para->Set_Boundary_Square();
	//para->Set_Barycentric_Cot();
	//para->Run();

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::ARAP::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's texcoords and positions ->  half-edge structure's texcoords and positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->coord = triMesh->GetTexcoords()[i].cast_to<vecf2>();
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	// first do the Initialization
	Initialize();
	printf("ASAP::Initialize:\n"
		"\t""Success\n");

	//second set the anchor point, in ARAP we only need one
	V* v1 = heMesh->Boundaries()[0][0]->Origin();
	anchor_index = heMesh->Index(v1);
	anchor_coordinate = vecf2(0, 0);

	this->triMesh = triMesh;
	return true;
}

bool ARAP::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	ARAP_Solve();
	printf("ARAP::ARAP_para\n"
		"\t""Success\n");

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();

	vector<pointf2> texture_coordinate;
	for (auto v : heMesh->Vertices())
		texture_coordinate.push_back(v->coord.cast_to<pointf2>());
	triMesh->Update(texture_coordinate);

	if (displaytype == kon) {
		vector<pointf3> positions;
		vector<unsigned> indice;
		positions.reserve(nV);
		indice.reserve(3 * nF);
		printf("test1\n");
		for (auto v : heMesh->Vertices())
			positions.push_back(v->coord.cast_to<pointf3>());
		printf("test2\n");
		for (auto f : heMesh->Polygons()) { // f is triangle
			for (auto v : f->BoundaryVertice()) // vertices of the triangle
				indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
		}
		printf("test3\n");
		triMesh->Init(indice, positions);
		printf("test\n");
	}

	printf("test4\n");
	return true;
}

void ARAP::ARAP_Solve() {
	size_t nV = heMesh->NumVertices();
	
	//decomposition
	A.resize(nV, nV);
	A.setZero();
	GlobalMatrixA(A);
	solver.compute(A);
	cout << "decomposition Success" << endl;

	for (int itr = 0; itr < iteration_times; itr++) {
		cout << "iteration time: " << itr << endl;
		LocalSetL();
		cout << " LocalSetL Success " << endl;
		GlobalSolveU(solver);
		cout << " GlobalSolveU Success " << endl;
	}
}

double ARAP::Dist3(V* v1, V* v2) {
	double d = 0;
	for (int i = 0; i < 3; i++) {
		d += (v1->pos.at(i) - v2->pos.at(i)) * (v1->pos.at(i) - v2->pos.at(i));
	}
	return sqrt(d);
}

double ARAP::Cot3(V* v1, V* v2, V* v3) {
	double cos, cot;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	cot = cos / sqrt(1 - cos * cos);
	return cot;
}

double ARAP::Cos3(V* v1, V* v2, V* v3) {
	double cos;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	return cos;
}

void ARAP::Initialize() {
	size_t nT = heMesh->NumPolygons();
	auto triangle_list = heMesh->Polygons();
	
	//put x and cot in every single Polygon, very nice
	for (int i = 0; i < nT; i++) {
		V* v0 = triangle_list[i]->BoundaryVertice()[0];
		V* v1 = triangle_list[i]->BoundaryVertice()[1];
		V* v2 = triangle_list[i]->BoundaryVertice()[2];
		double edge01 = Dist3(v0, v1);
		double edge02 = Dist3(v0, v2);
		double edge12 = Dist3(v1, v2);
		double cos = Cos3(v1, v0, v2);
		vecf2 x0 = vecf2(0., 0.);
		vecf2 x1 = vecf2(edge01, 0.);
		vecf2 x2 = vecf2(edge02 * cos, edge02 * sqrt(1 - cos * cos));
		double cot0 = Cot3(v1, v2, v0);
		double cot1 = Cot3(v1, v0, v2);
		double cot2 = Cot3(v2, v1, v0);

		triangle_list[i]->x.push_back(x0);
		triangle_list[i]->x.push_back(x1);
		triangle_list[i]->x.push_back(x2);
		triangle_list[i]->cot.push_back(cot0);
		triangle_list[i]->cot.push_back(cot1);
		triangle_list[i]->cot.push_back(cot2);
	}
}

void ARAP::LocalSetL() {
	//solve SVD and get L
	for (auto triangle : heMesh->Polygons()) {
		auto vertices = triangle->BoundaryVertice();
		Matrix2d J0,J1,J2;
		//initialize
		J0 << (triangle->x[0] - triangle->x[1])[0] * (vertices[0]->coord - vertices[1]->coord)[0],
			(triangle->x[0] - triangle->x[1])[1] * (vertices[0]->coord - vertices[1]->coord)[0],
			(triangle->x[0] - triangle->x[1])[0] * (vertices[0]->coord - vertices[1]->coord)[1],
			(triangle->x[0] - triangle->x[1])[1] * (vertices[0]->coord - vertices[1]->coord)[1];
		J1 << (triangle->x[1] - triangle->x[2])[0] * (vertices[1]->coord - vertices[2]->coord)[0],
			(triangle->x[1] - triangle->x[2])[1] * (vertices[1]->coord - vertices[2]->coord)[0],
			(triangle->x[1] - triangle->x[2])[0] * (vertices[1]->coord - vertices[2]->coord)[1],
			(triangle->x[1] - triangle->x[2])[1] * (vertices[1]->coord - vertices[2]->coord)[1];
		J2 << (triangle->x[2] - triangle->x[0])[0] * (vertices[2]->coord - vertices[0]->coord)[0],
			(triangle->x[2] - triangle->x[0])[1] * (vertices[2]->coord - vertices[0]->coord)[0],
			(triangle->x[2] - triangle->x[0])[0] * (vertices[2]->coord - vertices[0]->coord)[1],
			(triangle->x[2] - triangle->x[0])[1] * (vertices[2]->coord - vertices[0]->coord)[1];
		Matrix2d J;
		J = triangle->cot[0] * J0 + triangle->cot[1] * J1 + triangle->cot[2] * J2;
		JacobiSVD<Matrix2d> svd(J, ComputeFullU | ComputeFullV);
		if (J.determinant() > 0)
			triangle->L = svd.matrixU() * svd.matrixV().transpose();
		else
		{
			Matrix2d D;
			D(0, 0) = 1; D(0, 1) = 0; D(1, 0) = 0; D(1, 1) = -1;
			triangle->L = svd.matrixU() * D * svd.matrixV().transpose();
		}
		//cout << triangle->L << endl;
	}
}

void ARAP::GlobalSolveU(SimplicialLLT<SparseMatrix<double>>& solver) {
	size_t nV = heMesh->NumVertices();
	bx.resize(nV); bx.setZero();
	by.resize(nV); by.setZero();
	auto vertice_list = heMesh->Vertices();

	//set b
	for (int i = 0; i < nV; i++) {
		if (i == anchor_index) {
			bx(i) += anchor_coordinate[0];
			by(i) += anchor_coordinate[1];
			continue;
		}
		Vector2d sum;
		sum.setZero();

		// traverse all the out half-edge
		size_t t = 0;
		int index = 0;
		double coe_ij = 0;
		for (auto he : vertice_list[i]->OutHEs()) {
			// Left Triangle
			auto triangle = he->Polygon();
			if (triangle != nullptr) {
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				auto diffx = triangle->x[index] - triangle->x[index == 2 ? 0 : index + 1];
				coe_ij += triangle->cot[index];
				sum += triangle->cot[index] * triangle->L * Vector2d(diffx[0], diffx[1]);
			}
			// Right Triangle
			triangle = he->Pair()->Polygon();
			if (triangle != nullptr)
			{
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				auto diffx = triangle->x[index] - triangle->x[index == 0 ? 2 : index - 1];
				coe_ij += triangle->cot[index == 0 ? 2 : index - 1];
				sum += triangle->cot[index == 0 ? 2 : index - 1] * triangle->L * Vector2d(diffx[0], diffx[1]);
			}
			// Set
			int j = static_cast<int>(heMesh->Index(he->End()));
			if (j == anchor_index)
			{
				bx(i) -= (-coe_ij) * anchor_coordinate[0];
				by(i) -= (-coe_ij) * anchor_coordinate[1];
			}
		}

		bx(i) += sum(0);
		by(i) += sum(1);
	}
	//solve the global phase
	VectorXd u_x = solver.solve(bx);
	VectorXd u_y = solver.solve(by);
	//cout << u_x << endl;
	//cout << u_y << endl;
	//update
	for (int i = 0; i < nV; i++) {
		vertice_list[i]->coord = vecf2(u_x(i), u_y(i));
	}
}

void ARAP::GlobalMatrixA(SparseMatrix<double>& A) {
	vector<Triplet<double>> tripletlist;
	size_t nV = heMesh->NumVertices();
	auto vertice_list = heMesh->Vertices();

	for (int i = 0; i < nV; i++) {
		if (i == anchor_index) {
			tripletlist.push_back(Triplet<double>(i, i, 1));
			continue;
		}

		double sum = 0;
		for (auto he : vertice_list[i]->OutHEs()) {
			double coe_ij = 0;
			// Left Triangle
			auto triangle = he->Polygon();
			size_t t = 0;
			int index = 0;
			if (triangle != nullptr) {
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				coe_ij += triangle->cot[index];
			}
			// Right Triangle
			triangle = he->Pair()->Polygon();
			if (triangle != nullptr) {
				for (index = 0; heMesh->Index(triangle->BoundaryVertice()[index]) != i; index++);
				coe_ij += triangle->cot[index == 0 ? 2 : index - 1];
			}
			// Set
			sum += coe_ij;
			int j = static_cast<int>(heMesh->Index(he->End()));
			//if (j != fixed_vertices_[0] && j != fixed_vertices_[1])
			if (j != anchor_index)
				tripletlist.push_back(Triplet<double>(i, j, -coe_ij));
		}

		tripletlist.push_back(Triplet<double>(i, i, sum));
	}

	A.setFromTriplets(tripletlist.begin(), tripletlist.end());
	//cout << A << endl;
}

void ARAP::setTimes(int time) {
	iteration_times = time;
}

void ARAP::Set_Display() {
	displaytype = kon;
}