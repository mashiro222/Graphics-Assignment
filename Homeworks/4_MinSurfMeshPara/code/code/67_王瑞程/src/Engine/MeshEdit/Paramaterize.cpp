#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Basic/Geometry.h>

using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) {
	minSurf = MinSurf::New(triMesh);
	this->triMesh = triMesh;

}

void Paramaterize::Clear() {
	minSurf->Clear();
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	Clear();
	minSurf->Init(triMesh);
	return true;
}

bool Paramaterize::Run(EnumMethod weightmethod, EnumMethod boundarymethod) {
	switch (weightmethod)
	{
	case UNIFORM_WEIGHT:
		GenWeight_Uniform();
		break;
	case COTANGENT_WEIGHT:
		GenWeight_Cotangent();
		break;
	case SHAPE_PRESERVING:
		GenWeight_ShapePreserving();
		break;
	default:
		cout << "Invalid Parameter: weightmethod" << std::endl;
		return false;
		break;
	}
	switch (boundarymethod)
	{
	case ROUND_BOUNDARY:
		MapBoundaryRound();
		break;
	case SQUARE_BOUNDARY:
		MapBoundarySquare();
		break;
	default:
		cout << "Invalid Parameter: boundarymethod" << std::endl;
		return false;
		break;
	}
	minSurf->GenVecb();
	//调用minSurf的最小化求解
	minSurf->Minimize();
	size_t nV = minSurf->heMesh->NumVertices();
	
	//texcoords是引用
	std::vector<Ubpa::pointf2>& texcoords = triMesh->GetTexcoords();

	//triMesh 和 heMesh 中的点索引一致， 所以对应赋值
	for (int i = 0; i < texcoords.size(); i++)
	{
		texcoords[i] =Ubpa::pointf2(minSurf->heMesh->Vertices()[i]->pos[0], minSurf->heMesh->Vertices()[i]->pos[1]);
	}
	return false;
}

void Ubpa::Paramaterize::GetMinSurfMesh()
{
	// 转换 half-edge structure -> triangle mesh
	size_t nV = minSurf->heMesh->NumVertices();
	size_t nF = minSurf->heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : minSurf->heMesh->Vertices())
		positions.push_back(v->pos.cast_to<Ubpa::pointf3>());
	for (auto f : minSurf->heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(minSurf->heMesh->Index(v)));
	}
	triMesh->Init(indice, positions);
}

void Ubpa::Paramaterize::GenWeight_Cotangent()
{
	float cosa, cosb, w;
	Ubpa::vecf3 pre, suc;

	std::vector<Eigen::Triplet<float>> coef_list;
	for (auto v : minSurf->heMesh->Vertices())
	{
		size_t v_idx = minSurf->heMesh->Index(v);
		if (v->IsBoundary())
		{
			coef_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, 1.f));
		}
		else
		{
			float sum_weight = 0;
			auto adjVs = v->AdjVertices();
			size_t n_adjVs = adjVs.size();
			for (int j = 0; j < n_adjVs; j++)
			{
				pre = adjVs[(j + n_adjVs - 1) % n_adjVs]->pos;
				suc = adjVs[(j + 1) % n_adjVs]->pos;
				cosa = Ubpa::vecf3::cos_theta(v->pos - pre, adjVs[j]->pos - pre);
				cosb = Ubpa::vecf3::cos_theta(v->pos - suc, adjVs[j]->pos - suc);
				w = - cosa/sqrt(1.000001f - cosa*cosa) - cosb/sqrt(1.000001f - cosb*cosb);
				sum_weight += w;
				coef_list.push_back(Eigen::Triplet<float>(v_idx, minSurf->heMesh->Index(adjVs[j]), w));
			}
			coef_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, -sum_weight));
		}
	}
	size_t nV = minSurf->heMesh->NumVertices();
	minSurf->weight_mat.resize(nV, nV);
	minSurf->weight_mat.setFromTriplets(coef_list.begin(), coef_list.end());
}

void Ubpa::Paramaterize::GenWeight_ShapePreserving()
{
	vector<float> angs;
	vector<float> mu;
	vector<Ubpa::vecf2> ps;
	Ubpa::vecf2 zerop = Ubpa::vecf2(0.f, 0.f);

	float area0;
	Ubpa::vecf3 pre, suc;

	std::vector<Eigen::Triplet<float>> coef_list;
	for (auto v : minSurf->heMesh->Vertices())
	{
		size_t v_idx = minSurf->heMesh->Index(v);
		if (v->IsBoundary())
		{
			coef_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, 1.f));
		}
		else
		{
			auto adjVs = v->AdjVertices();
			size_t n_adjVs = adjVs.size();
			coef_list.push_back(Eigen::Triplet<float>(v_idx, v_idx, (float)(n_adjVs)));
			if (n_adjVs > angs.size())
			{
				angs.resize(n_adjVs);
				ps.resize(n_adjVs);
				mu.resize(n_adjVs);
			}
			float sum_ang = 0;
			//求ang(u_j, v, u_{j+1})
			for (int j = 0; j < n_adjVs; j++)
			{
				angs[j] = acosf(Ubpa::vecf3::cos_theta(adjVs[j]->pos - v->pos, adjVs[(j + 1) % n_adjVs]->pos - v->pos));
				sum_ang += angs[j];
			}
			//建立局部测地极坐标
			float plane_ang = 0;
			float len;
			for (int j = 0; j < n_adjVs; j++)
			{
				len = Ubpa::vecf3::distance(adjVs[j]->pos, v->pos);
				ps[j] = Ubpa::vecf2(cosf(plane_ang)*len, sinf(plane_ang)*len);
				plane_ang += 2.f*3.14159265f*angs[j] / sum_ang;
			}
			//计算系数
			for (int j = 0; j < n_adjVs; j++)
				mu[j] = 0.f;
			for (int j = 0; j < n_adjVs; j++)
			{
				//找到r(j)
				int r;
				for (r = 0; r < n_adjVs; r++)
				{
					if (r == j|| (r + 1) % n_adjVs == j ) continue;
					if (Ubpa::Geometry::InsideTri2f(ps[j], ps[r], ps[(r + 1) % n_adjVs], Ubpa::vecf2(0.f, 0.f)))
						break;
				}
				area0 = Ubpa::Geometry::AreaTri2f(ps[j], ps[r], ps[(r + 1) % n_adjVs]);
				mu[j] += Ubpa::Geometry::AreaTri2f(zerop, ps[r], ps[(r + 1) % n_adjVs]) / area0;
				mu[r] += Ubpa::Geometry::AreaTri2f(zerop, ps[j], ps[(r + 1) % n_adjVs]) / area0;
				mu[(r+1) % n_adjVs] += Ubpa::Geometry::AreaTri2f(zerop, ps[j], ps[r]) / area0;
			}
			for (int j = 0; j < n_adjVs; j++)
			{
				coef_list.push_back(Eigen::Triplet<float>(v_idx, minSurf->heMesh->Index(adjVs[j]), -mu[j]));
			}	
		}
	}
	size_t nV = minSurf->heMesh->NumVertices();
	minSurf->weight_mat.resize(nV, nV);
	minSurf->weight_mat.setFromTriplets(coef_list.begin(), coef_list.end());
}

void Ubpa::Paramaterize::MapBoundarySquare()
{
	// boundary 不是引用
	auto boundary = minSurf->heMesh->Boundaries()[0];
	size_t nBVs = boundary.size();
	float delta = 4.f / (float)nBVs;
	float z = 0.f;
	size_t i = 0;
	while(z < 1.f)
	{
		boundary[i++]->End()->pos = Ubpa::vecf3(z, 0.f, 0.f);
		z += delta;
	}
	z -= 1.f;
	while (z < 1.f)
	{
		boundary[i++]->End()->pos = Ubpa::vecf3(1.f, z, 0.f);
		z += delta;
	}
	z -= 1.f;
	while (z < 1.f)
	{
		boundary[i++]->End()->pos = Ubpa::vecf3(1.f-z, 1.f, 0.f);
		z += delta;
	}
	z -= 1.f;
	while (z < 1.f)
	{
		boundary[i++]->End()->pos = Ubpa::vecf3(0.f, 1.f-z, 0.f);
		z += delta;
	}
}

void Ubpa::Paramaterize::MapBoundaryRound()
{
	// boundary 不是引用
	auto boundary = minSurf->heMesh->Boundaries()[0];
	size_t nBVs = boundary.size();

	float delta = 3.14159265f * 2.f / (float)nBVs;
	float theta = 0;
	for (int i = 0; i < nBVs; i++)
	{
		boundary[i]->End()->pos = Ubpa::vecf3(0.5f + 0.5f*cos(theta), 0.5f + 0.5f*sin(theta), 0.f);
		theta += delta;
	}
}

void Ubpa::Paramaterize::GenWeight_Uniform()
{
	minSurf->GenWeight();
}