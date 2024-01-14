#include <Engine/MeshEdit/ASAP.h>
#include <Engine/MeshEdit/MinSurf.h>
#include <Engine/MeshEdit/Paramaterize.h>
#include <Engine/Primitive/TriMesh.h>
#include <Eigen/Sparse>

using namespace Ubpa;
using namespace std;
using namespace Eigen;

ASAP::ASAP(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void ASAP::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool ASAP::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::ASAP::Init:\n"
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
		printf("ERROR::ASAP::Init:\n"
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

	this->triMesh = triMesh;
	return true;
}

bool ASAP::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	ASAP_para();
	printf("ASAP::ASAP_para\n"
		"\t""Success\n");

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();

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

void ASAP::Set_Display() {
	displaytype = kon;
}

double ASAP::Dist3(V* v1, V* v2) {
	double d = 0;
	for (int i = 0; i < 3; i++) {
		d += (v1->pos.at(i) - v2->pos.at(i)) * (v1->pos.at(i) - v2->pos.at(i));
	}
	return sqrt(d);
}

double ASAP::Cot3(V* v1, V* v2, V* v3) {
	double cos, cot;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	cot = cos / sqrt(1 - cos * cos);
	return cot;
}

double ASAP::Cos3(V* v1, V* v2, V* v3) {
	double cos;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	return cos;
}

int ASAP::Find_Index(int vector_index, P* triangle) {
	if (heMesh->Index(triangle->BoundaryVertice()[0]) == vector_index)
		return 0;
	if (heMesh->Index(triangle->BoundaryVertice()[1]) == vector_index)
		return 1;
	if (heMesh->Index(triangle->BoundaryVertice()[2]) == vector_index)
		return 2;
}

int ASAP::Find_back(int index) {
	if (index == 0)
		return 2;
	else
		return index - 1;
}

int ASAP::Find_next(int index){
	if (index == 2)
		return 0;
	else return index + 1;
}


void ASAP::ASAP_para() {
	Anchor_Index();
	Set_Matrix_u();
	printf("ASAP::Set_Matrix_u\n"
		"\t""Success\n");
	Set_Matrix_ab();
	printf("ASAP::Set_Matrix_ab\n"
		"\t""Success\n");
	Solve();
	printf("ASAP::Solve\n"
		"\t""Success\n");
}

//Initialize the diff and cot
void ASAP::Initialize() {
	size_t nT = heMesh->NumPolygons();
	size_t nV = heMesh->NumVertices();
	auto triangle_list = heMesh->Polygons();
	size_t n = 2 * (nV + nT);
	coM.resize(n, n); coM.setZero();
	b.resize(n); b.setZero();
	for (size_t i = 0; i < nT; i++) {
		V* v0 = triangle_list[i]->BoundaryVertice()[0];
		V* v1 = triangle_list[i]->BoundaryVertice()[1];
		V* v2 = triangle_list[i]->BoundaryVertice()[2];
		double edge01 = Dist3(v0, v1);
		double edge02 = Dist3(v0, v2);
		double edge12 = Dist3(v1, v2);
		double cos = Cos3(v1, v0, v2);
		pointf2 x0 = pointf2(0., 0.);
		pointf2 x1 = pointf2(edge01, 0.);
		pointf2 x2 = pointf2(edge02 * cos, edge02 * sqrt(1 - cos * cos));
		double cot0 = Cot3(v1, v2, v0);
		double cot1 = Cot3(v1, v0, v2);
		double cot2 = Cot3(v0, v1, v2);
		//isometric_list.push_back({ x0,x1,x2 });
		diff_x.push_back({ (x0 - x1)[0],(x1 - x2)[0],(x2 - x0)[0] });
		diff_y.push_back({ (x0 - x1)[1],(x1 - x2)[1],(x2 - x0)[1] });
		cot_list.push_back({ cot0,cot1,cot2 });
	}

}

//choose two point as the anchor point
void ASAP::Anchor_Index() {
	V* anchor_point_1 = heMesh->Boundaries()[0][0]->Origin();
	size_t nB2 = heMesh->Boundaries()[0].size() / 2;
	V* anchor_point_2 = heMesh->Boundaries()[0][nB2]->End();
	anchor_index_1 = heMesh->Index(anchor_point_1);
	anchor_index_2 = heMesh->Index(anchor_point_2);
}

//construct the linear equations coefficient Matrix
void ASAP::Set_Matrix_u() {
	//initialize
	auto triangle_list = heMesh->Polygons();
	size_t nV = heMesh->NumVertices();

	//\partial E/ \partial u = 0
	for (size_t i = 0; i < nV; i++) {
		auto adj_edges = heMesh->Vertices()[i]->OutHEs();
		size_t degree = adj_edges.size();

		//fix two anchor points
		if (i == anchor_index_1 || i == anchor_index_2)
		{
			tripletlist.push_back(Triplet<double>(2 * i, 2 * i, 1));
			tripletlist.push_back(Triplet<double>(2 * i + 1, 2 * i + 1, 1));
			b(2 * i) = (i == anchor_index_1) ? 0 : 1;
			b(2 * i + 1) = (i == anchor_index_1) ? 0 : 1;
			continue;
		}

		double tmp = 0;
		size_t t = 0;
		int index = 0;
		for (int j = 0; j < degree; j++) {
			double cot_ij = 0;
			//two adjacent Polygon of u
			P* triangle = adj_edges[j]->Polygon();
			if (triangle != nullptr) {
				//find the corresponding (i,t) for u
				t = heMesh->Index(triangle);
				index = Find_Index(i, triangle);
				cot_ij += cot_list[t][index];
			}
			triangle = adj_edges[j]->Pair()->Polygon();
			if (triangle != nullptr) {
				t = heMesh->Index(triangle);
				index = Find_Index(i, triangle);
				cot_ij += cot_list[t][Find_back(index)];
			}

			// construct Matrix
			tmp += cot_ij;
			int v_index = heMesh->Index(adj_edges[j]->End());
			if (v_index != anchor_index_1 && v_index != anchor_index_2)
			{
				tripletlist.push_back(Triplet<double>(i * 2, 2 * v_index, -cot_ij));
				tripletlist.push_back(Triplet<double>(i * 2 + 1, 2 * v_index + 1, -cot_ij));
			}
			else if (v_index == anchor_index_2)
			{
				b(i * 2) -= -cot_ij;
				b(i * 2 + 1) -= -cot_ij;
			}
		}

		tripletlist.push_back(Triplet<double>(i * 2, i * 2, tmp));
		tripletlist.push_back(Triplet<double>(i * 2 + 1, i * 2 + 1, tmp));
	}
}

void ASAP::Set_Matrix_ab() {
	//initialize
	auto triangle_list = heMesh->Polygons();
	size_t nT = heMesh->NumPolygons();
	size_t nV = heMesh->NumVertices();

	//\partial E/ \partial a(b) = 0
	for (int t = 0; t < nT; t++) {
		double tmp = 0;

		for (int i = 0; i < 3; i++) {
			int v_index = heMesh->Index(triangle_list[t]->BoundaryVertice()[i]);
			tmp += cot_list[t][i] * (diff_x[t][i] * diff_x[t][i] + diff_y[t][i] * diff_y[t][i]);

			if (v_index != anchor_index_1 && v_index != anchor_index_2) {
				//two edges
				tripletlist.push_back(Triplet<double>(2 * nV + 2 * t, 2 * v_index, -cot_list[t][i] * diff_x[t][i] + cot_list[t][Find_back(i)] * diff_x[t][Find_back(i)]));
				tripletlist.push_back(Triplet<double>(2 * nV + 2 * t, 2 * v_index + 1, -cot_list[t][i] * diff_y[t][i] + cot_list[t][Find_back(i)] * diff_y[t][Find_back(i)]));
				tripletlist.push_back(Triplet<double>(2 * nV + 2 * t + 1, 2 * v_index, -cot_list[t][i] * diff_y[t][i] + cot_list[t][Find_back(i)] * diff_y[t][Find_back(i)]));
				tripletlist.push_back(Triplet<double>(2 * nV + 2 * t + 1, 2 * v_index + 1, cot_list[t][i] * diff_x[t][i] - cot_list[t][Find_back(i)] * diff_x[t][Find_back(i)]));

				tripletlist.push_back(Triplet<double>(2 * v_index, 2 * nV + 2 * t, -cot_list[t][i] * diff_x[t][i] + cot_list[t][Find_back(i)] * diff_x[t][Find_back(i)]));
				tripletlist.push_back(Triplet<double>(2 * v_index + 1, 2 * nV + 2 * t, -cot_list[t][i] * diff_y[t][i] + cot_list[t][Find_back(i)] * diff_y[t][Find_back(i)]));
				tripletlist.push_back(Triplet<double>(2 * v_index, 2 * nV + 2 * t + 1, -cot_list[t][i] * diff_y[t][i] + cot_list[t][Find_back(i)] * diff_y[t][Find_back(i)]));
				tripletlist.push_back(Triplet<double>(2 * v_index + 1, 2 * nV + 2 * t + 1, cot_list[t][i] * diff_x[t][i] - cot_list[t][Find_back(i)] * diff_x[t][Find_back(i)]));
			}

			//eliminate extra item
			else if (v_index == anchor_index_2) {
				b(2 * nV + 2 * t) -= -cot_list[t][i] * diff_x[t][i] + cot_list[t][Find_back(i)] * diff_x[t][Find_back(i)];
				b(2 * nV + 2 * t) -= -cot_list[t][i] * diff_y[t][i] + cot_list[t][Find_back(i)] * diff_y[t][Find_back(i)];
				b(2 * nV + 2 * t + 1) -= -cot_list[t][i] * diff_y[t][i] + cot_list[t][Find_back(i)] * diff_y[t][Find_back(i)];
				b(2 * nV + 2 * t + 1) -= cot_list[t][i] * diff_x[t][i] - cot_list[t][Find_back(i)] * diff_x[t][Find_back(i)];
			}
		}
		tripletlist.push_back(Triplet<double>(2 * nV + 2 * t, 2 * nV + 2 * t, tmp));
		tripletlist.push_back(Triplet<double>(2 * nV + 2 * t + 1, 2 * nV + 2 * t + 1, tmp));
	}
}

void ASAP::Solve() {
	coM.setFromTriplets(tripletlist.begin(), tripletlist.end());
	solver.compute(coM);
	//result
	VectorXd X = solver.solve(b);

	//evaluate
	for (auto v : heMesh->Vertices()) {
		size_t i = heMesh->Index(v);
		if (i != anchor_index_1 && i != anchor_index_2) {
			pointf2 tmp = { X(static_cast<Index>(2 * i)), X(static_cast<Index>(2 * i + 1)) };
			v->coord = tmp.cast_to<vecf2>();
		}
		else if (i == anchor_index_1) {
			pointf2 tmp = { 0, 0 };
			v->coord = tmp.cast_to<vecf2>();
		}
		else {
			pointf2 tmp = { 1, 1 };
			v->coord = tmp.cast_to<vecf2>();
		}
	}
}
