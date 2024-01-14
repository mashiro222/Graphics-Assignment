#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) {
	Init(triMesh);
}

void Paramaterize::Clear() {
	min->Clear();
}

bool Ubpa::Paramaterize::SetTexcoords()
{
	return my_trimesh->Update(Texcoords());
}

std::vector<Ubpa::pointf2> Ubpa::Paramaterize::Texcoords()
{
	Ptr<TriMesh> temp_tri;
	temp_tri = TriMesh::New(my_trimesh->GetIndice(), my_trimesh->GetPositions(), my_trimesh->GetNormals(), my_trimesh->GetTexcoords(), my_trimesh->GetTangents());//Why cannot I simply copy one?
	Paramaterize para_temp(temp_tri);
	para_temp.Run();

	vector<Ubpa::pointf2> result;

	auto vs = temp_tri->GetPositions();

	for (const auto& c : vs)
	{
		result.push_back(pointf2(c[0], c[2]));
	}
	return result;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	min = make_shared<MinSurf>(triMesh);
	my_trimesh = triMesh;
	return false;
}

bool Paramaterize::Run() {
	auto bound = min->heMesh->Boundaries()[0];
	const int N = bound.size();
	int extra = N % 4;
	int points[4];

	for (int i = 3; i >= 0; i--)
	{
		if (extra > 0)
		{
			points[i] = (i + 1) * (N / 4) + extra--;
		}
		else {
			points[i] = (i + 1) * (N / 4);
		}
	}

	for (int i = 0; i < N; i++)
	{
		if (i < points[0]) {
			int quarter = points[0];
			double step = 1.0 / quarter;

			(bound[i]->End()->pos = vecf3(0, 0, step * (i)));
		}
		else if (i < points[1]) {
			int quarter = points[1] - points[0];
			double step = 1.0 / quarter;
			(bound[i]->End()->pos = vecf3(step * (i - points[0]), 0, 1));
		}
		else if (i < points[2]) {
			int quarter = points[2] - points[1];
			double step = 1.0 / quarter;
			(bound[i]->End()->pos = vecf3(1, 0, 1 - step * (i - points[1])));
		}
		else
		{
			int quarter = points[3] - points[2];
			double step = 1.0 / quarter;
			(bound[i]->End()->pos = vecf3(1 - step * (i - points[2]), 0, 0));
		}
	}

	return min->Run(kMean);
}