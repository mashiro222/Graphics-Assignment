#include <Engine/MeshEdit/ASAP.h>
#include <Engine/MeshEdit/ARAP.h>
#include <Engine/Primitive/TriMesh.h>
#include <Eigen/Sparse>
#include <Engine/MeshEdit/Paramaterize.h>
#include <Engine/MeshEdit/IDT.h>
#include <cmath>

using namespace std;
using namespace Ubpa;
using namespace Eigen;

IDT::IDT(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void IDT::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool IDT::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::IDT::Init:\n"
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
		printf("ERROR::IDT::Init:\n"
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
	printf("IDT::Initialize:\n"
		"\t""Success\n");

	this->triMesh = triMesh;
	return true;
}

bool IDT::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::IDT::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Intrinsic_Delaunay_Triangulation();
	//还不止这个，还需要添加点来画出折线
	
	printf("IDT::intrinsic_delaunay_triangulation\n"
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

double IDT::Dist3(V* v1, V* v2) {
	double d = 0;
	for (int i = 0; i < 3; i++) {
		d += (v1->pos.at(i) - v2->pos.at(i)) * (v1->pos.at(i) - v2->pos.at(i));
	}
	return sqrt(d);
}

double IDT::Cot3(V* v1, V* v2, V* v3) {
	double cos, cot;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	cot = cos / sqrt(1 - cos * cos);
	return cot;
}

double IDT::Cos3(V* v1, V* v2, V* v3) {
	double cos;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	return cos;
}

double IDT::Angle(V* v1, V* v2, V* v3) {
	double cos;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	cos = (a * a + b * b - c * c) / (2 * a * b);
	double theta = acos(cos);
	return theta;
}

double IDT::Angle_update(int i1, int i2, int i3, P* triangle) {
	
}

//calculate the length of v1-v3
double IDT::Length(V* v1, V* v2, V* v3) {
	double l;
	double a = Dist3(v1, v2);
	double b = Dist3(v2, v3);
	double c = Dist3(v1, v3);
	
}

int IDT::Find_Index(V* vertice, P* triangle) {
	if (vertice == triangle->BoundaryVertice()[0])
		return 0;
	if (vertice == triangle->BoundaryVertice()[1])
		return 1;
	if (vertice == triangle->BoundaryVertice()[2])
		return 2;
}

int IDT::Check_Index(V* vertice, P* triangle) {
	int index = heMesh->Index(vertice);
	int index0 = heMesh->Index(triangle->BoundaryVertice()[0]);
	int index1 = heMesh->Index(triangle->BoundaryVertice()[1]);
	int index2 = heMesh->Index(triangle->BoundaryVertice()[1]);
	if (index == index0) return 0;
	if (index == index1) return 1;
	if (index == index2) return 2;
	return -1;
}

void IDT::Initialize() {
	size_t nT = heMesh->NumPolygons();
	size_t nV = heMesh->NumVertices();
	size_t nE = heMesh->NumEdges();
	auto triangle_list = heMesh->Polygons();
	auto vertice_list = heMesh->Vertices();
	auto Edge_list = heMesh->Edges();

	for (size_t i = 0; i < nT; i++) {
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
		double theta0 = Angle(v1, v0, v2);
		double theta1 = Angle(v0, v1, v2);
		double theta2 = Angle(v0, v2, v1);

		triangle_list[i]->x.push_back(x0);
		triangle_list[i]->x.push_back(x1);
		triangle_list[i]->x.push_back(x2);

		triangle_list[i]->theta.push_back(theta0);
		triangle_list[i]->theta.push_back(theta1);
		triangle_list[i]->theta.push_back(theta2);
	}

	for (size_t i = 0; i < nV; i++) {
		auto v = vertice_list[i];
		double cvt = 0;
		double pi = PI<double>;
		vertice_list[i]->degree = v->AdjPolygons().size();
		if (vertice_list[i]->IsBoundary()) {
			for (auto poly : v->AdjPolygons()) {
				int adj_index = Find_Index(v, poly);
				cvt -= poly->theta[adj_index];
			}
			cvt += pi;
		}
		else {
			for (auto poly : v->AdjPolygons()) {
				int adj_index = Find_Index(v, poly);
				cvt -= poly->theta[adj_index];
			}
			cvt += 2 * pi;
		}
		vertice_list[i]->cvt = cvt;
	}

	for (size_t i = 0; i < nE; i++) {
		auto he = Edge_list[i]->HalfEdge();
		V* v1 = he->Origin();
		V* v2 = he->End();
		Edge_list[i]->l = Dist3(v1, v2);
		Edge_list[i]->origin = true;
		Edge_list[i]->sect_edge_index.clear();
		Edge_list[i]->sect_point.clear();
	}
}

int IDT::check_if_delaunay(V* v1, V* v2) {
	double sum_theta = 0;
	double pi = PI<double>;
	for (auto poly : v1->AdjPolygons()) {
		int index2 = Check_Index(v2, poly);
		if (index2 == -1)
			continue;
		int index1 = Find_Index(v1, poly);
		sum_theta += poly->theta[3 - index1 - index2];
	}
	if (sum_theta < pi) return 1;
	return 0;
}

int IDT::check_can_delaunay(V* v1, V* v2) {
	double sum_theta1 = 0;
	double sum_theta2 = 0;
	double pi = PI<double>;
	//degree
	int degree1 = v1->AdjPolygons().size();
	int degree2 = v2->AdjPolygons().size();
	if (degree1 < 2 || degree2 < 2) return 0;
	//angle
	for (auto poly : v1->AdjPolygons()) {
		int index2 = Check_Index(v2, poly);
		if (index2 == -1)
			continue;
		int index1 = Find_Index(v1, poly);
		sum_theta1 += poly->theta[index1];
		sum_theta2 += poly->theta[index2];
	}
	if (sum_theta1 >= pi || sum_theta2 >= pi) return 0;
	return 1;
}

//we assume that we only need to operate on the origin edge
void IDT::flip(E* e) { 
	//find two polygon
	auto v1 = e->HalfEdge()->Origin();
	auto v2 = e->HalfEdge()->End();
	auto e_13 = e->HalfEdge()->Pre()->Edge();
	auto e_14 = e->HalfEdge()->Pair()->Next()->Edge();
	auto e_23 = e->HalfEdge()->Next()->Edge();
	auto e_24 = e->HalfEdge()->Pair()->Pre()->Edge();
	V* v3; V* v4;
	P* poly1; P* poly2;
	for (auto poly : v1->AdjPolygons()) {
		int index2 = Check_Index(v2, poly);
		if (index2 == -1)
			continue;
		int index1 = Find_Index(v1, poly);
		if (index2 > index1) {
			poly1 = poly;
			v3 = poly1->BoundaryVertice()[3 - index1 - index2];
		}
		else {
			poly2 = poly;
			v4 = poly2->BoundaryVertice()[3 - index1 - index2];
		}
	}

	//do the flip
	heMesh->FlipEdge(e);
	
	//new edge
	E* e_new;
	for (auto edge : v3->AdjEdges()) {
		if (v4 == edge->HalfEdge()->Origin() || v4 == edge->HalfEdge()->End()) {
			e_new = edge;
		}
	}
	//new polygon
	P* poly_new1;
	P* poly_new2;
	for (auto poly : v1->AdjPolygons()) {
		if (Check_Index(v3, poly) != -1 && Check_Index(v4, poly) != -1) poly_new1 = poly;
	}
	for (auto poly : v2->AdjPolygons()) {
		if (Check_Index(v3, poly) != -1 && Check_Index(v4, poly) != -1) poly_new2 = poly;
	}


	//update edge
	//origin
	e_new->origin = 0;
	//length
	double theta = poly1->theta[Find_Index(v1, poly1)] + poly2->theta[Find_Index(v2, poly2)];
	double l13 = e_13->l;
	double l14 = e_14->l;
	e_new->l = sqrt(l13 * l13 + l14 * l14 - 2 * l13 * l14 * cos(theta));
	//update polygon theta
	int index1 = Find_Index(v1, poly_new1);
	int index2 = Find_Index(v2, poly_new2);
	int index3 = Find_Index(v3, poly_new1);
	int index4 = Find_Index(v4, poly_new2);
	double l1 = poly_new1->BoundaryEdges()[index1]->l;
	double l2 = poly_new1->BoundaryEdges()[3 - index1 - index3]->l;
	double l3 = poly_new1->BoundaryEdges()[index3]->l;
	poly_new1->theta[index1] = acos((l1 * l1 + l3 * l3 - l2 * l2) / (2 * l1 * l3));
	poly_new1->theta[3 - index1 - index3] = acos((l1 * l1 + l2 * l2 - l3 * l3) / (2 * l1 * l2));
	poly_new1->theta[index3] = acos((l2 * l2 + l3 * l3 - l1 * l1) / (2 * l2 * l3));
	l1 = poly_new2->BoundaryEdges()[index2]->l;
	l2 = poly_new2->BoundaryEdges()[3 - index2 - index4]->l;
	l3 = poly_new2->BoundaryEdges()[index2]->l;
	poly_new2->theta[index2] = acos((l1 * l1 + l3 * l3 - l2 * l2) / (2 * l1 * l3));
	poly_new2->theta[3 - index2 - index4] = acos((l1 * l1 + l2 * l2 - l3 * l3) / (2 * l1 * l2));
	poly_new2->theta[index4] = acos((l2 * l2 + l3 * l3 - l1 * l1) / (2 * l2 * l3));

	//sect edge index
	for (int i : e_13->sect_edge_index) {
		if (find(e_23->sect_edge_index.begin(), e_23->sect_edge_index.end(), i) != e_23->sect_edge_index.end())
			e_new->sect_edge_index.push_back(i);
	}
	for (int i : e_13->sect_edge_index) {
		if (find(e_24->sect_edge_index.begin(), e_24->sect_edge_index.end(), i) != e_24->sect_edge_index.end())
			e_new->sect_edge_index.push_back(i);
	}
	for (int i : e_14->sect_edge_index) {
		if (find(e_23->sect_edge_index.begin(), e_23->sect_edge_index.end(), i) != e_23->sect_edge_index.end())
			e_new->sect_edge_index.push_back(i);
	}
	for (int i : e_14->sect_edge_index) {
		if (find(e_24->sect_edge_index.begin(), e_24->sect_edge_index.end(), i) != e_24->sect_edge_index.end())
			e_new->sect_edge_index.push_back(i);
	}
	//sect point
	for (int i = 0; i < e_new->sect_edge_index.size(); i++) {
		
	}
	

}

void IDT::Intrinsic_Delaunay_Triangulation() {
	//move through all edge, flip the bad edge, and then again and again
	int i = 0;
	int nE = heMesh->NumEdges();
	bool check = false;
	while (check == false) {
		for (i = 0; i < nE; i++) {
			auto v1 = heMesh->Edges()[i]->HalfEdge()->Origin();
			auto v2 = heMesh->Edges()[i]->HalfEdge()->End();
			if (!check_if_delaunay(v1, v2)) {
				flip(heMesh->Edges()[i]);
			}
		}
		if (i == nE) check = true;
	}
}

