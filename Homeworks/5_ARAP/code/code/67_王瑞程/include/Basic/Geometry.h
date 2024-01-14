#pragma once

#include <UGM/vec.h>
#include <vector>

namespace Ubpa {
	namespace Geometry {
		/*vecf2 CoordIn(const Ubpa::vecf2 a, const vecf2 b, const vecf2 p);
		//p是否在a,b,c为顶点的平面三角形内部
		bool InsideTri2f(const Ubpa::vecf2 a, const Ubpa::vecf2 b, const Ubpa::vecf2 c, const Ubpa::vecf2 p);
		float AreaTri2f(const vecf2 a, const vecf2 b, const vecf2 c);*/

		template<typename T>
		bool IsConvexPolygon(const std::vector<point<T, 3>>& points) {
			assert(points.size() >= 4);
			std::vector<vec<T, 3>> vecs(points.size());
			std::vector<vec<T, 3>> norms(points.size());
			for (size_t i = 0; i < points.size(); i++) {
				size_t next = (i + 1) % points.size();
				vecs[i] = points[next] - points[i];
			}
			for (size_t i = 0; i < vecs.size(); i++) {
				size_t next = (i + 1) % points.size();
				norms[i] = vecs[i].cross(vecs[next]);
				for (size_t j = 0; j < i; j++) {
					if (norms[j].dot(norms[i]) < static_cast<T>(0))
						return false;
				}
			}
			return true;
		}
		inline vecf2 CoordIn(const Ubpa::vecf2 a, const vecf2 b, const vecf2 p)
		{
			float fact = (a[1] * b[0] - a[0] * b[1]);
			return vecf2(-(b[1] * p[0] - b[0] * p[1]) / fact, -(-a[1] * p[0] + a[0] * p[1]) / fact);
		}
		inline float AreaTri2f(const vecf2 a, const vecf2 b, const vecf2 c)
		{
			vecf2 u = b - a, v = c - a;
			return abs(u[0] * v[1] - u[1] * v[0]);
		}
		inline bool InsideTri2f(const Ubpa::vecf2 a, const Ubpa::vecf2 b, const Ubpa::vecf2 c, const Ubpa::vecf2 p)
		{
			vecf2 t = CoordIn(b - a, c - a, p - a);
			return 0.f <= t[0] && t[0] < 1.f&& 0.f <= t[1] && t[1] < 1.f;
		}
	}
}