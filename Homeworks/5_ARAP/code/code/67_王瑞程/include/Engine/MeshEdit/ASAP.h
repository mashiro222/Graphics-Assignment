#pragma once

#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <Engine/MeshEdit/LocalGlobalPara.h>
#include <Eigen/Sparse>
#include <Eigen/src/SparseLU/SparseLU.h>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;

	//ARAP 参数化
	class ASAP : public LocalGlobalPara {
	public:
		ASAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ASAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ASAP>(triMesh);
		}
	public:
		//重写Run
		bool Run(int itenum);
		bool RunOnce();
	private:

		void SetFixed();
		void SetFixedOnce();
		void GenL();
		
		void GenWeightOnce();
		void GenVecbOnce();


		Eigen::Matrix2f CalLtSimilar(const Eigen::Matrix2f& S);
		Eigen::Matrix2f CalcJacobian(const size_t t);
	};
}