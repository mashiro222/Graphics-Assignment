#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <Engine/MeshEdit/Paramaterize.h>
#include <Eigen/Sparse>
#include <Eigen/src/SparseLU/SparseLU.h>
#include <UGM/UGM>

namespace Ubpa {
	class TriMesh;
	class Paramaterize;

	//LocalGlobal 参数化，定义了Local/Global方法用到的算法框架。继承此类必须重写Run()函数。
	class LocalGlobalPara : public HeapObj {
	public:
		LocalGlobalPara(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<LocalGlobalPara> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<LocalGlobalPara>(triMesh);
		}
		virtual ~LocalGlobalPara() {}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init(Ptr<TriMesh> triMesh);

		// call it after Init()
		virtual bool Run(int itenum);

	protected:
		struct CAdjTri
		{
			size_t t, i;
			CAdjTri(const size_t _t, const size_t _i)
			{
				t = _t;
				i = _i;
			}
		};

		void InitTexCoords();
		virtual void SetFixed();
		void GenIsometric();
		void GenWeight();
		virtual void GenL();
		void GenVecb();
		void ReZoom();
	public:
		void Flatten();

		Eigen::Matrix2f CalcSt(size_t t);

	protected:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };
	protected:

		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh

		std::vector<std::vector<vecf2>> isometric_x;
		std::vector<std::vector<vecf2>> isometric_dx;
		std::vector<std::vector<float>>	cot_theta;
		std::vector<std::list<CAdjTri>> adjTri;
		Eigen::MatrixX2f texcoords;

		//u^[k]的系数矩阵
		Eigen::SparseMatrix<float> weight_mat;
		Eigen::MatrixXf vec_b;

		Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
		std::vector<Eigen::Matrix2f> L;

		size_t fixed[3];
	};
}