
#include <algorithm>
using std::min;
using std::max;

#include "vector.h"

namespace math {

	// Line-Line Intersection
	// param:	l1_start -	Point where line 1 starts,
	//			l1_end	 -  Point where line 1 ends,
	//			l2_start -  Point where line 2 starts,
	//			l1_end	 -  Point where line 2 ends,
	// return:	Point where the 2 lines intersect, 
	//			null, if no intersection
	//
	inline Vector2 LineLineIntersection(Vector2 l1_start, Vector2 l1_end, Vector2 l2_start, Vector2 l2_end) {

		float A1 = l1_end.y - l1_start.y;
		float B1 = l1_start.x - l1_end.x;
		float C1 = A1*l1_start.x + B1*l1_start.y;

		float A2 = l2_end.y - l2_start.y;
		float B2 = l2_start.x - l2_end.x;
		float C2 = A2*l2_start.x + B2*l2_start.y;

		float det = A1*B2 - A2*B1;

		if (det != 0) {
			float x = (B2*C1 - B1*C2) / det;
			float y = (A1*C2 - A2*C1) / det;

			if (x >= min(l1_start.x, l1_end.x) && x <= max(l1_start.x, l1_end.x) &&
				x >= min(l2_start.x, l2_end.x) && x <= max(l2_start.x, l2_end.x) &&
				y >= min(l1_start.y, l1_end.y) && y <= max(l1_start.y, l1_end.y) &&
				y >= min(l2_start.y, l2_end.y) && y <= max(l2_start.y, l2_end.y))
				return Vector2(x, y); 
		}
		return Vector2(-99.0f, -99.0f); // No intersect, return "null"
	}


};
#endif
