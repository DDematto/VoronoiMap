// @author Devin DeMatto \n I am just trying out some silly stuff :P

#include "UVoronoiGen.h"
#include <delaunator.hpp>
#include "VectorUtil.h"

UVoronoiGen::UVoronoiGen()
{
}

UVoronoiGen::~UVoronoiGen()
{
}

// Main Generation Function
void UVoronoiGen::Generate()
{
	// Generate Random Points for Map
	const TArray<double> Coordinates = GeneratePoints();

	// Generate Delaunay Triangulation using Delaunator
	const std::vector<double> StdCoordinates(Coordinates.GetData(), Coordinates.GetData() + Coordinates.Num());
	const delaunator::Delaunator d(StdCoordinates);

	// Build Delaunay Graph
	BuildDelaunayGraph(d);

	// Build Voronoi Graph from Delaunay Triangulation
	BuildVoronoiGraph(d);
}

// May Do a different generation algorithm later to remove clustering effect
TArray<double> UVoronoiGen::GeneratePoints()
{
	TArray<double> Coords;
	MOriginalPoints.Empty();

	for (int i = 0; i < MResolution; ++i)
	{
		double x = FMath::RandRange(0, MWidth);
		double y = FMath::RandRange(0, MHeight);
		Coords.Add(x);
		Coords.Add(y);

		MOriginalPoints.Add(FVector(x, y, 0));
	}

	return Coords;
}



void UVoronoiGen::BuildDelaunayGraph(const delaunator::Delaunator& D)
{
	// Clear any existing triangles
	MDelaunayGraph.Triangles.Empty(D.triangles.size() / 3);

	// Resize the Triangles array to accommodate all triangles
	MDelaunayGraph.Triangles.SetNum(D.triangles.size() / 3);

	// Iterate through all triangles to populate the vertex indices
	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		MDelaunayGraph.Triangles[i / 3].VertexIndices = {
			static_cast<int32>(D.triangles[i]),
			static_cast<int32>(D.triangles[i + 1]),
			static_cast<int32>(D.triangles[i + 2])
		};
	}

	// Populate adjacency information
	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		FDelaunayNode& CurrentNode = MDelaunayGraph.Triangles[i / 3];

		// Half-edges are opposite to the triangle point, e.g., halfedge 0 is opposite to point 0
		for (size_t j = 0; j < 3; j++)
		{
			// Delaunator's 'halfedges' array contains the opposite half-edge for each edge
			size_t OppositeHalfEdge = D.halfedges[i + j];

			if (OppositeHalfEdge == delaunator::INVALID_INDEX)
			{
				// This edge is on the convex hull and has no adjacent triangle
				CurrentNode.AdjacentTrianglesIndices.Add(-1); // Use -1 or another sentinel value to indicate 'no neighbor'
			}
			else
			{
				// The triangle opposite to this edge is found by dividing the half-edge index by 3
				size_t AdjacentTriangle = OppositeHalfEdge / 3;
				CurrentNode.AdjacentTrianglesIndices.Add(static_cast<int32>(AdjacentTriangle));
			}
		}
	}
}

void UVoronoiGen::BuildVoronoiGraph(const delaunator::Delaunator& D)
{
	MVoronoiGraph.Vertices.Empty();

	// Define the bounding box for capping infinite edges
	FVector boundingBoxMin(0, 0, 0);
	FVector boundingBoxMax(MWidth, MHeight, 0);

	// Compute Voronoi vertices (circumcenters of Delaunay triangles)
	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		FVector P1(D.coords[2 * D.triangles[i]], D.coords[2 * D.triangles[i] + 1], 0);
		FVector P2(D.coords[2 * D.triangles[i + 1]], D.coords[2 * D.triangles[i + 1] + 1], 0);
		FVector P3(D.coords[2 * D.triangles[i + 2]], D.coords[2 * D.triangles[i + 2] + 1], 0);

		FVoronoiNode Node;
		Node.Position = ComputeCircumcenter(P1, P2, P3);
		MVoronoiGraph.Vertices.Add(Node);
	}

	// Determine adjacency of Voronoi vertices using Delaunay triangle adjacency
	for (int32 i = 0; i < MDelaunayGraph.Triangles.Num(); ++i)
	{
		const FDelaunayNode& DelaunayTriangle = MDelaunayGraph.Triangles[i];
		FVector circumcenter = MVoronoiGraph.Vertices[i].Position;

		for (int32 AdjacentTriangleIndex : DelaunayTriangle.AdjacentTrianglesIndices)
		{
			if (AdjacentTriangleIndex >= 0)
			{
				MVoronoiGraph.Vertices[i].AdjacentVerticesIndices.AddUnique(AdjacentTriangleIndex);
			}
			else
			{
				// Add the intersection point as a new Voronoi vertex
				FVoronoiNode BoundaryNode;
				int32 newVertexIndex = MVoronoiGraph.Vertices.Add(BoundaryNode);

				// Connect the current node to the new boundary node
				MVoronoiGraph.Vertices[i].AdjacentVerticesIndices.AddUnique(newVertexIndex);
			}
		}
	}
}

FVector UVoronoiGen::ComputeCircumcenter(const FVector& P1, const FVector& P2, const FVector& P3)
{
	// Convert FVector to TVector2
	const UE::Geometry::TVector2<float> A(P1.X, P1.Y);
	const UE::Geometry::TVector2<float> B(P2.X, P2.Y);
	const UE::Geometry::TVector2<float> C(P3.X, P3.Y);

	// Calculate circumcenter
	const auto Circumcenter = UE::Geometry::VectorUtil::Circumcenter(A, B, C);

	// Convert back to FVector
	return FVector(Circumcenter.X, Circumcenter.Y, 0);
}