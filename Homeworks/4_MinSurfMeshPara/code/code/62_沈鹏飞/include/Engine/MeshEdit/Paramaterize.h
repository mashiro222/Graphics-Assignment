#pragma once

#include <Basic/HeapObj.h>
#include <vector>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class Paramaterize : public HeapObj {
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<Paramaterize> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Paramaterize>(triMesh);
		}
	public:
		void Clear();

		bool SetTexcoords();

		bool Init(Ptr<TriMesh> triMesh);

		bool Run();

		Ptr<MinSurf> min;
		Ptr<TriMesh> my_trimesh;

	private:
		std::vector<pointf2> Texcoords();

	};
}
