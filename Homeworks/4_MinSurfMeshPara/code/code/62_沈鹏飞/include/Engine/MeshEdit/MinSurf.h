#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;
	class Paramaterize;

	enum WeightType
	{
		kMean = 1,
		kCot = 2
	};

	class MinSurf : public HeapObj {
	public:
		MinSurf(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<MinSurf> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<MinSurf>(triMesh);
		}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init(Ptr<TriMesh> triMesh);

		// call it after Init()
		bool Run(WeightType);

	private:
		// kernel part of the algorithm
		void Minimize();
		void cotMinimize();

	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };
	private:
		friend class Paramaterize;

		const int idx_in_mat(std::vector<int>& vec, int i);//Only works on a sorted vector

		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
	};
}
