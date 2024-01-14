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

			//ARAP��ASAP���ַ�����ʵ�ֲ���Parametrize����
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

		//�������ɲ�����ʱ�������С��������Ϊ�˿�Ч��
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

		//�������vector<vecf2> texcoords�Ľӿڣ�ֻ��friend���ܷ���
		bool GetTexcoods(EnumMethod weightmethod, EnumMethod boundarymethod, Eigen::MatrixX2f& texcoords);

	};
}
