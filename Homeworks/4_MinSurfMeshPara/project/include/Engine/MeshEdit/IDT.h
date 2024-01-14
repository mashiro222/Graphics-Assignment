#pragma once
//For Intrinsic Delaunay Triangulation structure
#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseCholesky>

namespace Ubpa {
	class TriMesh;

	// mesh boundary == 1
	class IDT : public HeapObj {
	public:
		IDT(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<IDT> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<IDT>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();


		enum DisplayType {
			kon,
			koff
		}displaytype = koff;

	private:
		//we need to refine this structure, add additional vertices and edge
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf2 coord;
			vecf3 pos;
			double cvt;//curvative(Gauss or Geodesic)
			int degree;
		};
		class E : public TEdge<V, E, P> {
		public:
			std::vector<int> sect_edge_index;
			std::vector<vecf3> sect_point;
			double l;
			bool origin;//if this edge is origin edge
		};
		class P : public TPolygon<V, E, P> {
		public:
			std::vector<vecf2> x;
			std::vector<double> theta;
		};

	private:
		void Initialize();//get the cos and length 
		void flip(E* e);//filp eij and update length cos sect_edge_index sect_point
		int check_if_delaunay(V* v1, V* v2);//check one edge
		int check_can_delaunay(V* v1, V* v2);
		void Intrinsic_Delaunay_Triangulation();//do the iDT algorithm

		double Dist3(V* v1, V* v2);
		double Cot3(V* v1, V* v2, V* v3);//cot(v1v2v3)
		double Cos3(V* v1, V* v2, V* v3);//cos(v1v2v3)
		double Angle(V* v1, V* v2, V* v3);
		double Angle_update(int i1, int i2, int i3, P* triangle);
		double Length(V* v1, V* v2, V* v3);
		int Find_Index(V* vertice, P* triangle);
		int Check_Index(V* vertice, P* triangle);

	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;	// vertice order is same with triMesh


	};
}