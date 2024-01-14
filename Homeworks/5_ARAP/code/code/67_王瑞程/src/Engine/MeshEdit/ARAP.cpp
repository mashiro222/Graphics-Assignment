#include <Engine/MeshEdit/ARAP.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/SparseCholesky>
#include <Eigen/SVD>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

ARAP::ARAP(Ptr<TriMesh> triMesh):LocalGlobalPara(triMesh)
{
	
}

bool ARAP::Run(int itenum) {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::ARAP::Run\n"
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
	}
	ReZoom();
	std::vector<Ubpa::pointf2> tritexcoords;
	tritexcoords.reserve(nV);
	for (int j = 0; j < nV; j++)
	{
		tritexcoords.push_back(Ubpa::pointf2(texcoords(j, 0), texcoords(j, 1)));
	}
	triMesh->Update(tritexcoords);
	return true;
}

void Ubpa::ARAP::SetFixed()
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
	fixed[2] = -1;
}

void Ubpa::ARAP::GenL()
{
	const std::vector<Ptr<Ubpa::Triangle>>& triangles = triMesh->GetTriangles();
	size_t nT = triangles.size();
	L.clear();
	L.reserve(nT);
	for (size_t t = 0; t < nT; t++)
	{
		L.push_back(CalLtRigid(CalcSt(t)));
	}
}

Eigen::Matrix2f Ubpa::ARAP::CalLtRigid(const Eigen::Matrix2f & S)
{
	JacobiSVD<Eigen::Matrix2f> svd(S, ComputeFullU|ComputeFullV);
	auto m = svd.singularValues();
	return svd.matrixU()*svd.matrixV().transpose();
}