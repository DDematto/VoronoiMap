// @author Devin DeMatto @file MapGeneration.cpp

#include "MapGeneration.h"
#include "VectorUtil.h"
#include "MapNode.h"
#include <delaunator.hpp>
#include "Intersection/IntrLine2Line2.h"

/*
 * Constructor
 */
UMapGeneration::UMapGeneration(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
}

/*
 * Destructor
 */
UMapGeneration::~UMapGeneration()
{
}

//
// Generation Logic
//

/**
 * Generates the dual graph of the Voronoi and Delaunay diagrams.
 */
void UMapGeneration::Generate()
{
	GeneratePoints();
	delaunator::Delaunator Diagram = GenerateDiagram();
	GenerateDelaunayGraph(Diagram);
	GenerateVoronoiGraph(Diagram);
}

/**
 * Generates random points within the specified width and height.
 */
void UMapGeneration::GeneratePoints()
{
	Points.Empty();

	for (int i = 0; i < Resolution; ++i)
	{
		double X = FMath::RandRange(0, Width);
		double Y = FMath::RandRange(0, Height);
		Points.Add(FVector(X, Y, 0));
	}
}

/**
 * Creates a diagram using Delaunator. The diagram may be stored as a member variable.
 */
delaunator::Delaunator UMapGeneration::GenerateDiagram()
{
	std::vector<double> FlatPoints;

	for (const FVector& Point : Points)
	{
		FlatPoints.push_back(Point.X);
		FlatPoints.push_back(Point.Y);
	}

	// Generate Delaunay Triangulation using Delaunator
	return delaunator::Delaunator(FlatPoints);
}

/**
 * Generates the Delaunay graph based on the points.
 */
void UMapGeneration::GenerateDelaunayGraph(delaunator::Delaunator Diagram)
{
	// Clear or initialize data structures
	Nodes.Empty();
	Triangles.Empty();

	// Iterate over each triangle
	for (size_t i = 0; i < Diagram.triangles.size(); i += 3)
	{
		FDelaunay DelaunayTriangle;

		// Set vertex indices
		DelaunayTriangle.VertexIndices = {
			static_cast<int>(Diagram.triangles[i]),
			static_cast<int>(Diagram.triangles[i + 1]),
			static_cast<int>(Diagram.triangles[i + 2])
		};

		// Iterate over the edges of the triangle to find adjacent triangles
		for (int j = 0; j < 3; ++j)
		{
			size_t edgeIndex = i + j;
			size_t twinEdge = Diagram.halfedges[edgeIndex];
			if (twinEdge != std::numeric_limits<size_t>::max()) // Check if the edge is not on the convex hull
			{
				// The adjacent triangle is the one that contains the twin edge
				size_t adjacentTriangleIndex = twinEdge / 3;
				DelaunayTriangle.AdjacentTrianglesIndices.Add(adjacentTriangleIndex);
			}
		}

		// Add the triangle to the Triangles array
		Triangles.Add(DelaunayTriangle);
	}
}

/**
 * Generates the Voronoi graph based on the Delaunay graph.
 */
void UMapGeneration::GenerateVoronoiGraph(delaunator::Delaunator Diagram)
{
	Nodes.Empty();

	// Calculate circumcenters and create Voronoi nodes
	for (const FDelaunay& Triangle : Triangles)
	{
		FVector Circumcenter = CalculateCircumcenter(Triangle);
		UMapNode* Node = NewObject<UMapNode>();
		Node->Position = Circumcenter;
		Nodes.Add(Node);
	}

	// Create Voronoi edges by connecting circumcenters of adjacent triangles
	for (int i = 0; i < Triangles.Num(); ++i)
	{
		const FDelaunay& Triangle = Triangles[i];
		for (int AdjIndex : Triangle.AdjacentTrianglesIndices)
		{
			if (AdjIndex != -1 && !Nodes[i]->AdjacentVerticesIndices.Contains(AdjIndex))
			{
				Nodes[i]->AdjacentVerticesIndices.Add(AdjIndex);
				Nodes[AdjIndex]->AdjacentVerticesIndices.Add(i);
			}
		}

		// Handle edges on the convex hull
		for (int j = 0; j < Triangle.IsEdgeOnConvexHull.Num(); ++j)
		{
			if (Triangle.IsEdgeOnConvexHull[j])
			{
				FVector IntersectionPoint = Triangle.ConvexHullIntersectionPoints[j];
				UMapNode* HullNode = NewObject<UMapNode>();
				HullNode->Position = IntersectionPoint;
				Nodes.Add(HullNode);

				int HullNodeIndex = Nodes.Num() - 1;
				Nodes[i]->AdjacentVerticesIndices.Add(HullNodeIndex);
				HullNode->AdjacentVerticesIndices.Add(i);
			}
		}
	}
}

FVector UMapGeneration::CalculateCircumcenter(const FDelaunay& Triangle)
{
	if (Triangle.VertexIndices.Num() < 3)
	{
		return FVector::ZeroVector;
	}

	// Convert triangle vertices to TVector2<RealType>
	FVector A = Points[Triangle.VertexIndices[0]];
	FVector B = Points[Triangle.VertexIndices[1]];
	FVector C = Points[Triangle.VertexIndices[2]];

	UE::Math::TVector2<float> A2D(A.X, A.Y);
	UE::Math::TVector2<float> B2D(B.X, B.Y);
	UE::Math::TVector2<float> C2D(C.X, C.Y);

	// Calculate circumcenter in 2D
	UE::Math::TVector2<float> Circumcenter2D = UE::Geometry::VectorUtil::Circumcenter<float>(A2D, B2D, C2D, KINDA_SMALL_NUMBER);

	// Convert back to FVector
	return FVector(Circumcenter2D.X, Circumcenter2D.Y, 0.0f);
}
