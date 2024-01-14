#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Engine/MeshEdit/ASAP.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseCholesky>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class ARAP : public HeapObj {
	public:
		ARAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ARAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ARAP>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();			
		//bool Show();		
		void setTimes(int time);
		void Set_Display();

		enum DisplayType {
			kon,
			koff
		}displaytype = koff;

	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf2 coord;
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P : public TPolygon<V, E, P> {
		public:
			Eigen::Matrix2d L;
			std::vector<vecf2> x;
			std::vector<double> cot;	// (i,i+1)
		};

	private:
		void ARAP_Solve();
		void Initialize();
		void LocalSetL();
		void GlobalSolveU(Eigen::SimplicialLLT<Eigen::SparseMatrix<double>>& solver);
		void GlobalMatrixA(Eigen::SparseMatrix<double>& A);

		double Dist3(V* v1, V* v2);
		double Cot3(V* v1, V* v2, V* v3);//cot(v1v2v3)
		double Cos3(V* v1, V* v2, V* v3);//cos(v1v2v3)

	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;	// vertice order is same with triMesh

		
		Eigen::SparseMatrix<double> A;
		Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
		Eigen::VectorXd bx;
		Eigen::VectorXd by;

		int iteration_times = 1;
		size_t anchor_index;
		vecf2 anchor_coordinate;
	};
}