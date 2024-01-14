#pragma once
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <UGM/UGM>
#include <vector>



namespace Ubpa {
	class TriMesh;
	class MinSurf;
	class Paramaterize;

	// mesh boundary == 1
	class ASAP : public HeapObj {
	public:
		ASAP(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<ASAP> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<ASAP>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();
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
			vecf3 pos;
			vecf2 coord;
		};
		class E : public TEdge<V, E, P> { };
		class P : public TPolygon<V, E, P> { };


	private:
		void Initialize();
		void Anchor_Index();
		void ASAP_para();
		void Set_Matrix_u();
		void Set_Matrix_ab();
		void Solve();

		double Dist3(V* v1, V* v2);
		double Cot3(V* v1, V* v2, V* v3);//cot(v1v2v3)
		double Cos3(V* v1, V* v2, V* v3);//cos(v1v2v3)
		int Find_Index(int vector_index, P* triangle);
		int Find_next(int index);
		int Find_back(int index);

	private:
		friend class Paramaterize;

		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		std::vector<pointf2> texture_coordinate;

		Eigen::SparseMatrix<double> coM;//coeff Matrix I don't know how to name it, so I use this one(
		Eigen::VectorXd b;
		Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
		std::vector<Eigen::Triplet<double>> tripletlist;

		size_t anchor_index_1;
		size_t anchor_index_2;

		std::vector<std::vector<double>> cot_list;
		std::vector<std::vector<double>> diff_x;
		std::vector<std::vector<double>> diff_y;
		std::vector<std::vector<pointf2>> isometric_list;
	};
}