#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) 
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
	cout << "Paramaterize::Paramaterize:" << endl
		<< "\t" << "Success" << endl;
}

void Paramaterize::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
	cout << "Paramaterize::Clear:" << endl
		<< "\t" << "Success" << endl;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Paramaterize::Init:\n"
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
		printf("ERROR::Parameterize::Init:\n"
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

bool Paramaterize::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Paramaterization();
	printf("Paramaterize::Paramaterization\n"
		"\t""Success\n");

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
	triMesh->Update(texture_coordinate);
	return true;
}

void Paramaterize::Paramaterization() {
	cout << "boundarytype = " << boundarytype << " barycentrictype = " << barycentrictype << " displaytype = " << displaytype << endl;
	Boundary();
	printf("Boundary Success\n");
	Laplace();
	printf("Laplace Success\n");
	Solve();
	printf("Solve Success\n");
}

void Paramaterize::Set_Boundary_Circle() {
	boundarytype = kCircle;
}

void Paramaterize::Set_Boundary_Square() {
	boundarytype = kSquare;
}

void Paramaterize::Set_Barycentric_Uniform() {
	barycentrictype = kUniform;
}

void Paramaterize::Set_Barycentric_Cot() {
	barycentrictype = kCot;
}

void Paramaterize::Set_Display() {
	displaytype = kon;
}

void Paramaterize::Boundary() {
	size_t nB = heMesh->Boundaries()[0].size();
	//record Index
	for (size_t i = 0; i < nB; i++) {
		Boundary_Index.push_back(heMesh->Index(heMesh->Boundaries()[0][i]->Origin()));
	}
	//reflect the Boundary to specified shape
	if (boundarytype == kCircle)
		Boundary_Circle();
	if (boundarytype == kSquare)
		Boundary_Square();
}

//we let Boundary in [0,1]*[0,1]
void Paramaterize::Boundary_Circle() {
	// we use the first Boundary in the Half-edge constructure
	size_t nB = heMesh->Boundaries()[0].size();
	//PI is from basic.h()
	double pi = PI<double>;
	double theta = 2 * pi / (double)nB;
	for (size_t i = 0; i < nB; i++) {
		Boundary_list.push_back(pointf2(0.5 * (1 + cos(i * theta)), 0.5 * (1 + sin(i * theta))));
	}
}

void Paramaterize::Boundary_Square() {
	// we use the first Boundary in the Half-edge constructure
	size_t nB = heMesh->Boundaries()[0].size();
	//same lull between every points
	double lull = (double)4 / (double)nB;
	for (size_t i = 0; i < nB; i++) {
		double temp = i * lull;
		if (temp < 1) {
			Boundary_list.push_back(pointf2(0, temp));
		}
		else if (temp >= 1 && temp < 2) {
			Boundary_list.push_back(pointf2(temp - 1, 1));
		}
		else if (temp >= 2 && temp < 3) {
			Boundary_list.push_back(pointf2(1, 3 - temp));
		}
		else {
			Boundary_list.push_back(pointf2(4 - temp, 0));
		}
	}
}

void Paramaterize::Laplace() {
	if (barycentrictype == kUniform)
		Laplace_Uniform();
	if (barycentrictype == kCot)
		Laplace_Cot();
}

void Paramaterize::Laplace_Uniform() {
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
}

double Paramaterize::Dist3(V* v1,V* v2) {
	double d = 0;
	for (int i = 0; i < 3; i++) {
		d += (v1->pos.at(i) - v2->pos.at(i)) * (v1->pos.at(i) - v2->pos.at(i));
	}
	return sqrt(d);
}

double Paramaterize::Cot3(V* v1, V* v2, V* v3) {
	double cos, cot;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	cot = cos / sqrt(1 - cos * cos);
	return cot;
}

void Paramaterize::Laplace_Cot() {
	size_t nV = heMesh->NumVertices();
	vector<Eigen::Triplet<double>> tripletlist;
	double last, next;

	for (size_t i = 0; i < nV; i++) {
		V* vertex = heMesh->Vertices()[i];
		tripletlist.push_back(Eigen::Triplet<double>(i, i, 1));
		if (!vertex->IsBoundary()) {
			double degree = vertex->AdjVertices().size();
			std::vector<double> w_list;
			double w_sum = 0;

			for (size_t j = 0; j < degree; j++) {
				last = (j != 0) ? (j - 1) : (degree - 1);
				next = (j != degree - 1) ? (j + 1) : (0);
				V* vertex_last = vertex->AdjVertices()[last];
				V* vertex_present = vertex->AdjVertices()[j];
				V* vertex_next = vertex->AdjVertices()[next];
				w_list.push_back(Cot3(vertex, vertex_last, vertex_present) + Cot3(vertex, vertex_next, vertex_present));
				w_sum += w_list.back();
			}
			//evaluate
			for (size_t j = 0; j < degree; j++) {
				tripletlist.push_back(Eigen::Triplet<double>(i, heMesh->Index(vertex->AdjVertices()[j]), -w_list[j] / w_sum));
			}
		}
	}

	L.resize(nV, nV);
	L.setZero();
	L.setFromTriplets(tripletlist.begin(), tripletlist.end());
}

void Paramaterize::Solve() {
	//first do decomposition
	solver.compute(L);

	//solve the problem
	size_t nV = heMesh->NumVertices();
	Eigen::VectorXd x(nV), bx(nV), y(nV), by(nV);
	bx.setZero(); by.setZero();
	for (size_t i = 0; i < Boundary_Index.size(); i++) {
		bx[Boundary_Index[i]] = Boundary_list[i][0];
		by[Boundary_Index[i]] = Boundary_list[i][1];
	}
	x = solver.solve(bx);
	y = solver.solve(by);

	//evaluate
	for (size_t i = 0; i < nV; i++) {
		if (displaytype == kon) {
			heMesh->Vertices()[i]->pos.at(0) = x(i);
			heMesh->Vertices()[i]->pos.at(1) = y(i);
			heMesh->Vertices()[i]->pos.at(2) = 0;
		}
		//record
		texture_coordinate.push_back(pointf2(x(i), y(i)));
	}
}