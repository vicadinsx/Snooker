#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <algorithm>
using std::min;
using std::max;

#include "vector.h"

namespace math {

	typedef Vector2 Point;

	// Intersection between 2 lines
	inline Point LineLineIntersection(Point ep1, Point ep2, Point ep3, Point ep4) {

		float A1 = ep2.y - ep1.y;
		float B1 = ep1.x - ep2.x;
		float C1 = A1*ep1.x + B1*ep1.y;

		float A2 = ep4.y - ep3.y;
		float B2 = ep3.x - ep4.x;
		float C2 = A2*ep3.x + B2*ep3.y;

		float det = A1*B2 - A2*B1;

		if (det != 0) {
			float x = (B2*C1 - B1*C2) / det;
			float y = (A1*C2 - A2*C1) / det;

			if (x >= min(ep1.x, ep2.x) && x <= max(ep1.x, ep2.x) &&
				x >= min(ep3.x, ep4.x) && x <= max(ep3.x, ep4.x) &&
				y >= min(ep1.y, ep2.y) && y <= max(ep1.y, ep2.y) &&
				y >= min(ep3.y, ep4.y) && y <= max(ep3.y, ep4.y))
				return Point(x, y);
		}
		return Point(-99.0f, -99.0f); // No intersect, return "null"
	}

	// Closest point to line
	inline Point closestpointonline(float lx1, float ly1, float lx2, float ly2, float x0, float y0) {

		float A1 = ly2 - ly1;
		float B1 = lx1 - lx2;

		double C1 = (ly2 - ly1)*lx1 + (lx1 - lx2)*ly1;
		double C2 = -B1*x0 + A1*y0;

		double det = A1*A1 - -B1*B1;

		double cx = 0;
		double cy = 0;

		if (det != 0) {
			cx = (float)((A1*C1 - B1*C2) / det);
			cy = (float)((A1*C2 - -B1*C1) / det);
		}

		else {
			cx = x0;
			cy = y0;
		}

		return Point(cx, cy);
	}


};
#endif
