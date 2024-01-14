#pragma once

#include <Engine/MeshEdit/MinSurf.h>
#include <Basic/HeapObj.h>
#include <Eigen/Sparse>


namespace Ubpa {
	class TriMesh;
	class MinSurf;
	class ARAP;
	class LocalGlobalPara;

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

			//ARAP和ASAP两种方法的实现不在Parametrize类里
			ARAP,
			ASAP,
			ASAP_ONCE,
		};
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
		~Paramaterize();
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
		friend class ARAP;
		friend class LocalGlobalPara;

		Ptr<MinSurf> minSurf;
		Ptr<TriMesh> triMesh;
		std::vector<Ubpa::vecf2> texcoords;


		void GenWeight_Uniform();
		void GenWeight_Cotangent();
		void GenWeight_ShapePreserving();

		void MapBoundaryRound();
		void MapBoundarySquare();

		//用来获得vector<vecf2> texcoords的接口，只有friend类能访问
		bool GetTexcoods(EnumMethod weightmethod, EnumMethod boundarymethod, Eigen::MatrixX2f& texcoords);

	};
}
