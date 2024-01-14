#include <Engine/MeshEdit/LocalGlobalPara.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/SparseCholesky>
#include <Eigen/SVD>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

LocalGlobalPara::LocalGlobalPara(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void LocalGlobalPara::Clear() {
	heMesh->Clear();
	isometric_dx.clear();
	isometric_x.clear();
	cot_theta.clear();
	adjTri.clear();
	triMesh = nullptr;
}

bool LocalGlobalPara::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::LocalGlobalPara::Init:\n"
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
		printf("ERROR::LocalGlobalPara::Init:\n"
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

bool LocalGlobalPara::Run(int itenum) {
	cout << "LocalGlobalPara::Run() : You should not use the base class. Instead, You are supposed to use the derived class, which rewrites this function. " << std::endl;
	/*
	**重写的模板**
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::LocalGlobalPara::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	size_t nV = heMesh->NumVertices();

	InitTexCoords();
	SetFixed();
	GenIsometric();
	GenWeight();
	solver.compute(weight_mat);
	for (int j = 0; j < 4; j++)
	{
		//Local
		GenL();
		//Global
		GenVecb();
		texcoords = solver.solve(vec_b);
	}

	std::vector<Ubpa::pointf2> tritexcoords;
	tritexcoords.reserve(nV);
	for (int j = 0; j < nV; j++)
	{
		tritexcoords.push_back(Ubpa::pointf2(texcoords(j, 0), texcoords(j, 1)));
	}
	triMesh->Update(tritexcoords);
	Flatten();
	*/
	return false;
}

void Ubpa::LocalGlobalPara::InitTexCoords()
{
	Ptr<Paramaterize> parameterize = Ubpa::New<Paramaterize>(triMesh);
	parameterize->GetTexcoods(Paramaterize::EnumMethod::SHAPE_PRESERVING, Paramaterize::EnumMethod::ROUND_BOUNDARY, texcoords);
}

void Ubpa::LocalGlobalPara::SetFixed()
{

	auto bd = heMesh->Boundaries();
	size_t m = 0;
	for (size_t i = 0; i < bd.size(); i++)
	{
		if (bd[i].size() > bd[m].size())
			m = i;
	}
	fixed[0] = heMesh->Index(bd[m][0]->End());
	fixed[1] = -1;
}

void Ubpa::LocalGlobalPara::GenIsometric()
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	const std::vector<Ubpa::pointf3>& positions = triMesh->GetPositions();

	size_t nT = triangles.size();
	size_t nV = triMesh->GetPositions().size();


	isometric_x.resize(nT);
	isometric_dx.resize(nT);
	cot_theta.resize(nT);

	adjTri.resize(nV);

	Ubpa::vecf3 p[3];
	for (size_t t = 0; t < nT; t++)
	{
		isometric_x[t].resize(3);
		isometric_dx[t].resize(3);
		cot_theta[t].resize(3);

		for (size_t i = 0; i < 3; i++)
		{
			size_t k = triangles[t]->idx[i];
			p[i] = Ubpa::vecf3(positions[k][0], positions[k][1], positions[k][2]);	//从网格上取出三角形的vecf3坐标
			adjTri[k].push_back(CAdjTri(t, i));		//记录顶点k所属的三角形t及内部编号和i
		}

		//计算Isometric变换到平面的坐标
		isometric_x[t][0] = Ubpa::vecf2(0.f, 0.f);
		isometric_x[t][1] = Ubpa::vecf2((p[1] - p[0]).norm(), 0.f);
		float costh = Ubpa::vecf3::cos_theta(p[1] - p[0], p[2] - p[0]);
		float len = (p[2] - p[0]).norm();
		isometric_x[t][2] = Ubpa::vecf2(costh*len, sqrt(1.0001f - costh * costh)*len);

		//直接的三角形坐标x并不常用，dx更常用
		for (size_t i = 0; i < 3; i++)
			isometric_dx[t][i] = isometric_x[t][i] - isometric_x[t][(i + 1) % 3];

		//计算每个isometric变换的三角形三个角Cot(\theta)
		cot_theta[t][0] = costh / sqrt(1.0001f - costh * costh);
		for (size_t i = 1; i < 3; i++)
		{
			costh = Ubpa::vecf3::cos_theta(p[(i + 1) % 3] - p[i], p[(i + 2) % 3] - p[i]);
			cot_theta[t][i] = costh / sqrt(1.0001f - costh * costh);
		}
	}
}
void Ubpa::LocalGlobalPara::ReZoom()
{
	size_t nV = heMesh->NumVertices();
	size_t m1 = 0, m2 = 0, m3 = 0, m4 = 0;
	for (size_t i = 0; i < nV; i++)
	{
		if (texcoords(i, 0) > texcoords(m1, 0))
			m1 = i;
		if (texcoords(i, 0) < texcoords(m2, 0))
			m2 = i;
		if (texcoords(i, 1) > texcoords(m3, 1))
			m3 = i;
		if (texcoords(i, 1) < texcoords(m4, 1))
			m4 = i;
	}
	float scale = 1.f / max(texcoords(m1, 0) - texcoords(m2, 0), texcoords(m3, 1) - texcoords(m4, 1));
	Eigen::Vector2f  sh = Eigen::Vector2f(texcoords(m2, 0), texcoords(m4, 1));
	for (size_t i = 0; i < nV; i++)
	{
		texcoords.row(i) = (texcoords.row(i) - sh.transpose())*scale;
	}
}
void Ubpa::LocalGlobalPara::GenWeight()
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	size_t nV = triMesh->GetPositions().size();

	weight_mat.resize(nV, nV);
	std::vector<Eigen::Triplet<float>> coef_elem_list;
	for (auto v : heMesh->Vertices())
	{
		//对于每个顶点，有个偏导为0的方程
		size_t v_idx = heMesh->Index(v);
		if (fixed[0] == v_idx || fixed[1] == v_idx || fixed[2] == v_idx)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, 1.f));
			continue;
		}
		std::map<size_t, float> coef;

		for (CAdjTri a : adjTri[v_idx])		//遍历所有包含它的三角形
		{
			coef[v_idx] = (coef.find(v_idx) == coef.end() ? 0 : coef[v_idx]) + cot_theta[a.t][(a.i + 2) % 3] + cot_theta[a.t][(a.i + 1) % 3];
			size_t ui1 = triangles[a.t]->idx[(a.i + 1) % 3];
			size_t ui2 = triangles[a.t]->idx[(a.i + 2) % 3];
			coef[ui1] = (coef.find(ui1) == coef.end() ? 0 : coef[ui1]) - cot_theta[a.t][(a.i + 2) % 3];
			coef[ui2] = (coef.find(ui2) == coef.end() ? 0 : coef[ui2]) - cot_theta[a.t][(a.i + 1) % 3];
		}
		for (auto cf : coef)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(v_idx, cf.first, cf.second));
		}
	}
	weight_mat.setFromTriplets(coef_elem_list.begin(), coef_elem_list.end());
}

void Ubpa::LocalGlobalPara::GenL()
{
	cout << "LocalGlobalPara::GenL() : You should not use the base class. Instead, You are supposed to use the derived class, which rewrites this function. " << std::endl;
	/*const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	L.clear();
	L.reserve(nT);
	for (size_t t = 0; t < nT; t++)
	{
		L.push_back(CalLtRigid(t));
		//cout << L[t] << std::endl;
	}*/
}

void Ubpa::LocalGlobalPara::GenVecb()
{
	//获得向量
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	size_t nV = triMesh->GetPositions().size();

	vec_b.resize(nV, 2);
	for (auto v : heMesh->Vertices())
	{
		size_t v_idx = heMesh->Index(v);
		if (fixed[0] == v_idx || fixed[1] == v_idx || fixed[2] == v_idx)
		{
			vec_b.row(v_idx) = texcoords.row(v_idx);
			continue;
		}
		Eigen::Vector2f b;
		b.fill(0.f);
		for (CAdjTri a : adjTri[v_idx])		//遍历所有包含它的三角形
		{
			auto Lt = L[a.t];
			b(0) += cot_theta[a.t][(a.i + 2) % 3] * (Lt(0, 0)*isometric_dx[a.t][a.i][0] + Lt(0, 1) * isometric_dx[a.t][a.i][1]) - cot_theta[a.t][(a.i + 1) % 3] * (Lt(0, 0)*isometric_dx[a.t][(a.i + 2) % 3][0] + Lt(0, 1) * isometric_dx[a.t][(a.i + 2) % 3][1]);
			b(1) += cot_theta[a.t][(a.i + 2) % 3] * (Lt(1, 0)*isometric_dx[a.t][a.i][0] + Lt(1, 1) * isometric_dx[a.t][a.i][1]) - cot_theta[a.t][(a.i + 1) % 3] * (Lt(1, 0)*isometric_dx[a.t][(a.i + 2) % 3][0] + Lt(1, 1) * isometric_dx[a.t][(a.i + 2) % 3][1]);
		}
		vec_b.row(v_idx) = b;
	}
}

void Ubpa::LocalGlobalPara::Flatten()
{
	size_t nV = triMesh->GetPositions().size();
	std::vector<Ubpa::pointf3> positions;
	positions.reserve(nV);
	for (int j = 0; j < nV; j++)
	{
		positions.push_back(Ubpa::pointf3(triMesh->GetTexcoords()[j][0], triMesh->GetTexcoords()[j][1], 0.f));
	}
	triMesh->Update(positions);
}


Eigen::Matrix2f Ubpa::LocalGlobalPara::CalcSt(const size_t t)
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	Matrix2f st;
	st.fill(0.f);

	for (size_t i = 0; i < 3; i++)
	{
		MatrixXf x;
		x.resize(1, 2);
		x(0, 0) = isometric_dx[t][i][0];
		x(0, 1) = isometric_dx[t][i][1];
		st += cot_theta[t][i] * ((texcoords.row(triangles[t]->idx[i]) - texcoords.row(triangles[t]->idx[(i + 1) % 3])).transpose() * x);
	}

	return st;
}


