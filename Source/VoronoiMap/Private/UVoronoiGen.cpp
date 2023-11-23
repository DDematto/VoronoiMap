// @author Devin DeMatto \n I am just trying out some silly stuff :P

#include <delaunator.hpp>
#include "UVoronoiGen.h"
#include "VectorUtil.h"

UVoronoiGen::UVoronoiGen()
{
}

UVoronoiGen::~UVoronoiGen()
{
}

TArray<double> UVoronoiGen::GeneratePoints() const
{
	TArray<double> Coords;
	for (int i = 0; i < MResolution; ++i)
	{
		double x = FMath::RandRange(0, MWidth);
		double y = FMath::RandRange(0, MHeight);
		Coords.Add(x);
		Coords.Add(y);
	}

	return Coords;
}

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

	// Relate Two Graphs
	RelateGraphs(d);
}

void UVoronoiGen::BuildDelaunayGraph(const delaunator::Delaunator& D)
{
	// Build Delaunay Graph
	MDelaunayGraph.Triangles.Empty();
	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		FDelaunayNode Node;
		Node.VertexIndices = {
			static_cast<int32>(D.triangles[i]),
			static_cast<int32>(D.triangles[i + 1]),
			static_cast<int32>(D.triangles[i + 2])
		};
		MDelaunayGraph.Triangles.Add(Node);
	}
}

void UVoronoiGen::BuildVoronoiGraph(const delaunator::Delaunator& D)
{
	// Clear existing Voronoi graph
	MVoronoiGraph.Vertices.Empty();

	// Iterate over Delaunay triangles and compute Voronoi vertices (circumcenters)
	for (std::size_t i = 0; i < D.triangles.size(); i += 3)
	{
		// Get the points of the triangle
		FVector P1 = FVector(D.coords[2 * D.triangles[i]], D.coords[2 * D.triangles[i] + 1], 0);
		FVector P2 = FVector(D.coords[2 * D.triangles[i + 1]], D.coords[2 * D.triangles[i + 1] + 1], 0);
		FVector P3 = FVector(D.coords[2 * D.triangles[i + 2]], D.coords[2 * D.triangles[i + 2] + 1], 0);

		// Add the circumcenter as a vertex in the Voronoi graph
		FVoronoiNode Node;
		Node.Position = ComputeCircumcenter(P1, P2, P3);
		// ... You'll also need to fill in the adjacency information here
		MVoronoiGraph.Vertices.Add(Node);
	}

	// ... Further processing to establish edges and handle infinite edges
}

FVector UVoronoiGen::ComputeCircumcenter(const FVector& P1, const FVector& P2, const FVector& P3)
{
	// Convert FVector to TVector2
	const UE::Geometry::TVector2<float> A(P1.X, P1.Y);
	const UE::Geometry::TVector2<float> B(P2.X, P2.Y);
	const UE::Geometry::TVector2<float> C(P3.X, P3.Y);

	// Calculate circumcenter
	const auto Circumcenter = UE::Geometry::VectorUtil::Circumcenter(A, B, C, KINDA_SMALL_NUMBER);

	// Convert back to FVector
	return FVector(Circumcenter.X, Circumcenter.Y, 0);
}

void UVoronoiGen::RelateGraphs(const delaunator::Delaunator& D)
{
	// Assumption: MVoronoiGraph.Vertices and MDelaunayGraph.Triangles are populated
	// and correspond to each other in order: the i-th Voronoi vertex corresponds to
	// the circumcenter of the i-th Delaunay triangle.

	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		FDelaunayNode& DelaunayNode = MDelaunayGraph.Triangles[i / 3];
		auto& [Position, AdjacentVerticesIndices] = MVoronoiGraph.Vertices[i / 3];

		// Iterate over the 3 edges of the triangle
		for (size_t j = 0; j < 3; ++j)
		{
			const size_t EdgeIndex = i + j;
			const size_t OppositePointIndex = D.triangles[D.halfedges[EdgeIndex]];

			// Check for valid opposite point index
			if (OppositePointIndex != delaunator::INVALID_INDEX)
			{
				// Find the triangle that contains the oppositePoint as a vertex.
				const size_t AdjacentTriangleIndex = FindTriangleContainingPoint(D, OppositePointIndex);

				// Add adjacency information to the Voronoi node.
				AdjacentVerticesIndices.AddUnique(AdjacentTriangleIndex);
			}
			// Else, handle infinite edges or boundary conditions as needed
		}
	}
}

size_t UVoronoiGen::FindTriangleContainingPoint(const delaunator::Delaunator& D, const size_t PointIndex)
{
	for (size_t i = 0; i < D.triangles.size(); i += 3)
	{
		if (D.triangles[i] == PointIndex || D.triangles[i + 1] == PointIndex || D.triangles[i + 2] == PointIndex)
		{
			return i / 3;
		}
	}
	return delaunator::INVALID_INDEX; // or a sentinel value indicating an error
}

