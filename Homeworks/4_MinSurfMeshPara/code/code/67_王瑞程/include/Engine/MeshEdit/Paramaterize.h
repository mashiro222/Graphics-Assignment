#pragma once

#include <Engine/MeshEdit/MinSurf.h>
#include <Basic/HeapObj.h>
#include <Eigen/Sparse>


namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class Paramaterize : public HeapObj {
	public:
		enum EnumMethod
		{
			UNIFORM_WEIGHT,
			COTANGENT_WEIGHT,
			SHAPE_PRESERVING,
			ROUND_BOUNDARY,
			SQUARE_BOUNDARY,
		};
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<Paramaterize> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Paramaterize>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);

		bool Run(EnumMethod weightmethod, EnumMethod boundarymethod);

		//将网格变成参数化时计算的最小曲面网格，为了看效果
		void GetMinSurfMesh();
		
	private:
		Ptr<MinSurf> minSurf;
		Ptr<TriMesh> triMesh;

		void GenWeight_Uniform();
		void GenWeight_Cotangent();
		void GenWeight_ShapePreserving();

		void MapBoundaryRound();
		void MapBoundarySquare();
	};
}
