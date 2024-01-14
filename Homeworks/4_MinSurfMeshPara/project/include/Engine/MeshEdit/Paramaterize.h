#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <vector>
#include <cmath>

enum BoundaryType {
	kCircle,
	kSquare
};
enum BarycentricType {
	kUniform,
	kCot
};
enum DisplayType {
	kon,
	koff
};

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class Paramaterize : public HeapObj {
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<Paramaterize> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Paramaterize>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();
		void Set_Boundary_Square();
		void Set_Boundary_Circle();
		void Set_Barycentric_Uniform();
		void Set_Barycentric_Cot();
		void Set_Display();
		
	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P : public TPolygon<V, E, P> { };
		BoundaryType boundarytype;
		BarycentricType barycentrictype;
		DisplayType displaytype = koff;

	private:
		void Paramaterization();
		void Boundary_Square();
		void Boundary_Circle();
		void Boundary();
		void Laplace();
		void Laplace_Uniform();
		void Laplace_Cot();
		void Solve();

		double Dist3(V* v1, V* v2);
		double Cot3(V* v1, V* v2, V* v3);//cot(v1v2v3)

	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		std::vector<size_t> Boundary_Index;
		std::vector<pointf2> Boundary_list;//pointf2 is from point.h(Ubpa)
		Eigen::SparseMatrix<double> L;
		Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
		//texture coordinate
		std::vector<pointf2> texture_coordinate;
	};
}
