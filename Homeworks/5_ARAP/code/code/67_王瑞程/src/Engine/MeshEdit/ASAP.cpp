#include <Engine/MeshEdit/ASAP.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/SparseCholesky>
#include <Eigen/SVD>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

ASAP::ASAP(Ptr<TriMesh> triMesh) :LocalGlobalPara(triMesh)
{

}

bool ASAP::Run(int itenum) {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ASAP::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	size_t nV = heMesh->NumVertices();

	InitTexCoords();
	SetFixed();
	GenIsometric();
	GenWeight();
	solver.compute(weight_mat);
	for (int j = 0; j < itenum; j++)
	{
		//Local
		GenL();
		//Global
		GenVecb();
		texcoords = solver.solve(vec_b);
		ReZoom();
	}

	std::vector<Ubpa::pointf2> tritexcoords;
	tritexcoords.reserve(nV);
	for (int j = 0; j < nV; j++)
	{
		tritexcoords.push_back(Ubpa::pointf2(texcoords(j, 0), texcoords(j, 1)));
	}
	triMesh->Update(tritexcoords);
	return true;
}


bool ASAP::RunOnce() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ASAP::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	size_t nV = heMesh->NumVertices();

	InitTexCoords();
	SetFixedOnce();
	GenIsometric();
	GenWeightOnce();
	GenVecbOnce();
	solver.compute(weight_mat);
	Eigen::MatrixXf res = solver.solve(vec_b);
	std::vector<Ubpa::pointf2> tritexcoords;
	tritexcoords.reserve(nV);
	for (int j = 0; j < nV; j++)
	{
		tritexcoords.push_back(Ubpa::pointf2(res(2 * j, 0), res(2 * j + 1, 0)));
	}
	triMesh->Update(tritexcoords);
	return true;
}
void Ubpa::ASAP::SetFixed()
{
	auto bd = heMesh->Boundaries();
	size_t m = 0;
	for (size_t i = 0; i < bd.size(); i++)
	{
		if (bd[i].size() > bd[m].size())
			m = i;
	}
	fixed[0] = -1;//heMesh->Index(bd[m][0]->End());
	fixed[1] = -1;//heMesh->Index(bd[m][1]->End());
	fixed[2] = -1;// heMesh->Index(bd[m][bd[m].size() * 2 / 3]->End());
}

void Ubpa::ASAP::SetFixedOnce()
{
	auto bd = heMesh->Boundaries();
	size_t m = 0;
	for (size_t i = 0; i < bd.size(); i++)
	{
		if (bd[i].size() > bd[m].size())
			m = i;
	}
	fixed[0] = heMesh->Index(bd[m][0]->End());
	fixed[1] = heMesh->Index(bd[m][bd[m].size() / 2]->End());
	fixed[2] = -1;//heMesh->Index(bd[m][bd[m].size() * 2 / 3]->End());
}

void Ubpa::ASAP::GenL()
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	L.clear();
	L.reserve(nT);
	for (size_t t = 0; t < nT; t++)
	{
		L.push_back(CalLtSimilar(CalcSt(t)));
	}
}



void Ubpa::ASAP::GenWeightOnce()
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	size_t nV = triMesh->GetPositions().size();

	weight_mat.resize(nV * 2 + nT * 2, nV * 2 + nT * 2);
	std::vector<Eigen::Triplet<float>> coef_elem_list;
	for (auto v : heMesh->Vertices())
	{
		//对于每个顶点，有个偏导为0的方程
		size_t v_idx = heMesh->Index(v);
		if (fixed[0] == v_idx || fixed[1] == v_idx || fixed[2] == v_idx)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx, 2 * v_idx, 1.f));
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx + 1 , 2 * v_idx + 1, 1.f));
			continue;
		}
		std::map<size_t, float> coef;

		for (CAdjTri a : adjTri[v_idx])		//遍历所有包含它的三角形
		{
			//统计与u相关的系数
			coef[v_idx] = (coef.find(v_idx) == coef.end() ? 0 : coef[v_idx]) + cot_theta[a.t][(a.i + 2) % 3] + cot_theta[a.t][(a.i + 1) % 3];
			size_t ui1 = triangles[a.t]->idx[(a.i + 1) % 3];
			size_t ui2 = triangles[a.t]->idx[(a.i + 2) % 3];
			coef[ui1] = (coef.find(ui1) == coef.end() ? 0 : coef[ui1]) - cot_theta[a.t][(a.i + 2) % 3];
			coef[ui2] = (coef.find(ui2) == coef.end() ? 0 : coef[ui2]) - cot_theta[a.t][(a.i + 1) % 3];
			//直接添加与at, bt相关的系数
			float coefat = 0.f, coefbt = 0.f;
			coefat = -cot_theta[a.t][(a.i + 2) % 3] * isometric_dx[a.t][a.i][0] + cot_theta[a.t][(a.i + 1) % 3] * isometric_dx[a.t][(a.i + 2) % 3][0];
			coefbt = -cot_theta[a.t][(a.i + 2) % 3] * isometric_dx[a.t][a.i][1] + cot_theta[a.t][(a.i + 1) % 3] * isometric_dx[a.t][(a.i + 2) % 3][1];
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx, 2 * nV + a.t, coefat));
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx, 2 * nV + nT + a.t, coefbt));
		}
		//与u相关系数
		for (auto cf : coef)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx, 2 * cf.first, cf.second));
		}

		for (CAdjTri a : adjTri[v_idx])		//再遍历所有包含它的三角形,处理第二个维度
		{
			//统计与u相关的系数
			coef[v_idx] = (coef.find(v_idx) == coef.end() ? 0 : coef[v_idx]) + cot_theta[a.t][(a.i + 2) % 3] + cot_theta[a.t][(a.i + 1) % 3];
			size_t ui1 = triangles[a.t]->idx[(a.i + 1) % 3];
			size_t ui2 = triangles[a.t]->idx[(a.i + 2) % 3];
			coef[ui1] = (coef.find(ui1) == coef.end() ? 0 : coef[ui1]) - cot_theta[a.t][(a.i + 2) % 3];
			coef[ui2] = (coef.find(ui2) == coef.end() ? 0 : coef[ui2]) - cot_theta[a.t][(a.i + 1) % 3];
			//直接添加与at, bt相关的系数
			float coefat = 0.f, coefbt = 0.f;
			coefat = -cot_theta[a.t][(a.i + 2) % 3] * isometric_dx[a.t][a.i][1] + cot_theta[a.t][(a.i + 1) % 3] * isometric_dx[a.t][(a.i + 2) % 3][1];
			coefbt = cot_theta[a.t][(a.i + 2) % 3] * isometric_dx[a.t][a.i][0] - cot_theta[a.t][(a.i + 1) % 3] * isometric_dx[a.t][(a.i + 2) % 3][0];
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx + 1, 2 * nV + a.t, coefat));
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx + 1, 2 * nV + nT + a.t, coefbt));
		}
		//与u相关系数
		for (auto cf : coef)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * v_idx + 1, 2 * cf.first + 1, cf.second));
		}
	}
	float coefu1[3], coefu2[3], coefat, coefbt;
	//对at偏导为 0 的方程
	for (size_t t = 0; t < nT; t++)
	{
		for (size_t i = 0; i < 3; i++)
		{
			coefu1[i] = coefu2[i] = 0;
		}
		coefat = 0.f;
		coefbt = 0.f;
		for (size_t i = 0; i < 3; i++)
		{
			coefu1[i] += -cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][0];
			coefu1[(i + 1) % 3] += +cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][0];
			coefu2[i] += -cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][1];
			coefu2[(i + 1) % 3] += +cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][1];
			coefat += cot_theta[t][(i + 2) % 3] * (isometric_dx[t][i][0] * isometric_dx[t][i][0] + isometric_dx[t][i][1] * isometric_dx[t][i][1]);
			coefbt += 0.f;
		}
		for (size_t i = 0; i < 3; i++)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * nV + t, 2 * triangles[t]->idx[i], coefu1[i]));
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * nV + t, 2 * triangles[t]->idx[i] + 1, coefu2[i]));
		}
		coef_elem_list.push_back(Eigen::Triplet<float>(2 * nV + t, 2 * nV + t, coefat));
	}
	//对bt偏导为 0 的方程
	for (size_t t = 0; t < nT; t++)
	{
		for (size_t i = 0; i < 3; i++)
		{
			coefu1[i] = coefu2[i] = 0;
		}
		coefat = 0.f;
		coefbt = 0.f;
		for (size_t i = 0; i < 3; i++)
		{
			coefu1[i] += -cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][1];
			coefu1[(i + 1) % 3] += +cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][1];
			coefu2[i] += +cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][0];
			coefu2[(i + 1) % 3] += -cot_theta[t][(i + 2) % 3] * isometric_dx[t][i][0];
			coefat += 0.f;
			coefbt += cot_theta[t][(i + 2) % 3] * (isometric_dx[t][i][0] * isometric_dx[t][i][0] + isometric_dx[t][i][1] * isometric_dx[t][i][1]);
		}
		for (size_t i = 0; i < 3; i++)
		{
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * nV + nT + t, 2 * triangles[t]->idx[i], coefu1[i]));
			coef_elem_list.push_back(Eigen::Triplet<float>(2 * nV + nT + t, 2 * triangles[t]->idx[i] + 1, coefu2[i]));
		}
		coef_elem_list.push_back(Eigen::Triplet<float>(2 * nV + +nT + t, 2 * nV + nT + t, coefbt));
	}
	weight_mat.setFromTriplets(coef_elem_list.begin(), coef_elem_list.end());
}

void Ubpa::ASAP::GenVecbOnce()
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	size_t nV = triMesh->GetPositions().size();
	vec_b.resize(2 * nV + 2 * nT, 1);
	vec_b.fill(0.f);
	for (auto v : heMesh->Vertices())
	{
		size_t v_idx = heMesh->Index(v);
		if (fixed[0] == v_idx || fixed[1] == v_idx || fixed[2] == v_idx)
		{
			vec_b(2 * v_idx , 0) = texcoords(v_idx, 0);
			vec_b(2 * v_idx + 1,0) = texcoords(v_idx, 1);
		}
	}
}

Eigen::Matrix2f Ubpa::ASAP::CalLtSimilar(const Eigen::Matrix2f & S)
{
	JacobiSVD<Eigen::Matrix2f> svd(S, ComputeFullU | ComputeFullV);
	Vector2f m = svd.singularValues();
	return ((m(0) + m(1)) / 2.f)*(svd.matrixU()*svd.matrixV().transpose());
}

Eigen::Matrix2f Ubpa::ASAP::CalcJacobian(const size_t t)
{
	size_t ui1 = triMesh->GetTriangles()[t]->idx[0];
	size_t ui2 = triMesh->GetTriangles()[t]->idx[1];
	size_t ui3 = triMesh->GetTriangles()[t]->idx[2];

	Eigen::MatrixXf A;
	A.resize(6, 6);
	A.fill(0.f);
	Eigen::VectorXf b;
	b.resize(6);
	A(0, 0) = texcoords(ui1, 0); A(0, 1) = texcoords(ui1, 1); A(0, 4) = 1.f; b(0) = isometric_x[t][0][0];
	A(1, 2) = texcoords(ui1, 0); A(0, 3) = texcoords(ui1, 1); A(0, 5) = 1.f; b(1) = isometric_x[t][0][1];
	A(2, 0) = texcoords(ui2, 0); A(0, 1) = texcoords(ui2, 1); A(0, 4) = 1.f; b(2) = isometric_x[t][1][0];
	A(3, 2) = texcoords(ui2, 0); A(0, 3) = texcoords(ui2, 1); A(0, 5) = 1.f; b(3) = isometric_x[t][1][1];
	A(4, 0) = texcoords(ui3, 0); A(0, 1) = texcoords(ui3, 1); A(0, 4) = 1.f; b(4) = isometric_x[t][2][0];
	A(5, 2) = texcoords(ui3, 0); A(0, 3) = texcoords(ui3, 1); A(0, 5) = 1.f; b(5) = isometric_x[t][2][1];
	Eigen::VectorXf ans = A.colPivHouseholderQr().solve(b);
	Eigen::Matrix2f J;
	J(0, 0) = ans(0);
	J(0, 1) = ans(1);
	J(1, 0) = ans(2);
	J(1, 1) = ans(3);
	return J;
}
