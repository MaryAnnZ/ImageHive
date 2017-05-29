#include "IntersectionHelper.h"


//http://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
bool IntersectionHelper::isInside(std::vector<cv::Point> polygonVertices,cv::Point p)
{
	int n = polygonVertices.size();

	// Create a point for line segment from p to infinite
	cv::Point extreme = cv::Point(1000, p.y);

	// Count intersections of the above line with sides of polygon
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;
		// Check if the line segment from 'p' to 'extreme' intersects
		// with the line segment from 'polygon[i]' to 'polygon[next]'
		if (doIntersect(polygonVertices[i], polygonVertices[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next',
			// then check if it lies on segment. If it lies, return true,
			// otherwise false
			if (orientation(polygonVertices[i], p, polygonVertices[next]) == 0)
				return onSegment(polygonVertices[i], p, polygonVertices[next]);

			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise
	return count & 1;  // Same as (count%2 == 1)
}


// The function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool IntersectionHelper::doIntersect(cv::Point p1, cv::Point q1, cv::Point p2, cv::Point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool IntersectionHelper::onSegment(cv::Point p, cv::Point q, cv::Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;
	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int IntersectionHelper::orientation(cv::Point p, cv::Point q, cv::Point r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

/*
* http://alienryderflex.com/polygon/
//  int    polyCorners  =  how many corners the polygon has
//  float  polyX[]      =  horizontal coordinates of corners
//  float  polyY[]      =  vertical coordinates of corners
//  float  x, y         =  point to be tested
*/
bool IntersectionHelper::pointInPolygon(int polyCorners, std::vector<float> polyX, std::vector<float> polyY, float x, float y) {
	int   i, j = polyCorners - 1;
	bool  oddNodes = false;

	for (i = 0; i<polyCorners; i++) {
		if ((polyY.at(i)< y && polyY.at(j) >= y
			|| polyY[j]< y && polyY[i] >= y)
			&& (polyX[i] <= x || polyX[j] <= x)) {
			oddNodes ^= (polyX[i] + (y - polyY[i]) / (polyY[j] - polyY[i])*(polyX[j] - polyX[i])<x);
		}
		j = i;
	}
	return oddNodes;
}