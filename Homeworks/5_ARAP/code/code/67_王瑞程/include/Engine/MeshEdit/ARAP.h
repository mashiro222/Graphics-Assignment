#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <Engine/MeshEdit/LocalGlobalPara.h>
#include <Eigen/Sparse>
#include <Eigen/src/SparseLU/SparseLU.h>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;

	//ARAP ������
	class ARAP : public LocalGlobalPara{
	public:
		ARAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ARAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ARAP>(triMesh);
		}
	public:
		//��дRun
		bool Run(int itenum);

	private:

		void SetFixed();
		void GenL();

		Eigen::Matrix2f CalLtRigid(const Eigen::Matrix2f& S);

	};
}