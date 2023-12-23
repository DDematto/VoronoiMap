/*
 * Delaunator Documentation
 *
 * + new Delaunator(coords)
 * -- Constructs a delaunay triangulation object given an array of point coordinates of the form: [x0, y0, x1, y1, ...] (use a typed array for best performance).
 *
 * + Delaunator.from(points[, getX, getY])
 * -- Constructs a delaunay triangulation object given an array of points ([x, y] by default). getX and getY are optional functions of the form (point) => value for custom point formats. Duplicate points are skipped.
 *
 *
 * + delaunay.triangles
 * -- A Uint32Array array of triangle vertex indices (each group of three numbers forms a triangle). All triangles are directed counterclockwise.
 *
 * + delaunay.halfedges
 * -- A Int32Array array of triangle half-edge indices that allows you to traverse the triangulation. i-th half-edge in the array corresponds to vertex triangles[i] the half-edge is coming from. halfedges[i] is the index of a twin half-edge in an adjacent triangle (or -1 for outer half-edges on the convex hull).
 * -- The flat array-based data structures might be counterintuitive, but they're one of the key reasons this library is fast.
 *
 * + delaunay.hull
 * -- A Uint32Array array of indices that reference points on the convex hull of the input data, counter-clockwise.
 *
 * + delaunay.coords
 * -- An array of input coordinates in the form [x0, y0, x1, y1, ....], of the type provided in the constructor (or Float64Array if you used Delaunator.from).
 *
 * + delaunay.update()
 * --Updates the triangulation if you modified delaunay.coords values in place, avoiding expensive memory allocations. Useful for iterative relaxation algorithms such as Lloyd's.
 */

//#include "MapGeneration.h"
#include "Misc/AutomationTest.h"
//#include "MapNode.h"
//#include "NodeEdge.h"

BEGIN_DEFINE_SPEC(FMapGenerationTests, "MapGenerationTests", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FMapGenerationTests)

void FMapGenerationTests::Define()
{
	Describe("General Setup", [this]()
	{
		It("should instantiate the MapGeneration class as an object", [this]()
		{

		});

	});

}
