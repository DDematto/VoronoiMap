/**
 * @author Devin DeMatto
 * @file MapGeneration.cpp
 */

#include <algorithm>
#include "delaunator.hpp"
#include "MapGeneration.h"
#include "FPoissonSampling.h"
#include "MapNode.h"
#include "NodeEdge.h"
#include "VectorUtil.h"

 // Constructor and Events
void UMapGeneration::NativeConstruct()
{
	Super::NativeConstruct();
	GenerateMap();
}

int32 UMapGeneration::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	for (const UMapNode* Node : Nodes)
	{
		Node->NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}


/**
 * Creates a Poisson Distribution of Points Based on MapSize
 */
TArray<FVector2D> UMapGeneration::GeneratePoints() const
{
	// Define parameters for Poisson Disk Sampling
	const float ExclusionRadius = DynamicExclusionRadius(); // Define an appropriate exclusion radius
	constexpr int Iterations = 20; // Set the number of iterations for the sampling
	constexpr int K = 1; // Amount of Times a Point will attempt to place

	// Setup the random stream
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();

	// Call the Poisson Disk Sampling function
	return FPoissonSampling::GeneratePoissonDiscSamples(MapSize.X, MapSize.Y, ExclusionRadius, K, Iterations, RandomStream);
}

/**
 * Creates a Minimum Radius around each point
 * @return A value for a Exclusion Radius around each point
 */
float UMapGeneration::DynamicExclusionRadius() const
{
	constexpr float BaseMapSize = 500.0f; // Base map size (both width and height)
	constexpr float BaseExclusionRadius = 25.0f; // Base exclusion radius for the minimum map size

	const float ScaleFactor = FMath::Sqrt((MapSize.X * MapSize.Y) / (BaseMapSize * BaseMapSize));
	return BaseExclusionRadius * ScaleFactor;
}

/**
 * Main Method For Map Generation
 */
void UMapGeneration::GenerateMap()
{
	Nodes.Empty();

	TArray<FVector2D> Points = GeneratePoints();

	std::vector<double> StdCoordinates;

	// Spacing for boundary points
	constexpr int Spacing = 50;

	// Add boundary points
	// Top and bottom edges
	for (int x = 0; x <= MapSize.X; x += Spacing) {
		StdCoordinates.push_back(x);
		StdCoordinates.push_back(0); // Top edge
		StdCoordinates.push_back(x);
		StdCoordinates.push_back(MapSize.Y); // Bottom edge
	}

	// Left and right edges
	for (int y = Spacing; y < MapSize.Y; y += Spacing) {
		StdCoordinates.push_back(0);
		StdCoordinates.push_back(y); // Left edge
		StdCoordinates.push_back(MapSize.X);
		StdCoordinates.push_back(y); // Right edge
	}

	// Reserve space for generated points
	StdCoordinates.reserve(StdCoordinates.size() + Points.Num() * 2);

	// Add generated points
	for (const FVector2D& Point : Points) {
		StdCoordinates.push_back(Point.X);
		StdCoordinates.push_back(Point.Y);
	}

	// Perform Delaunay triangulation
	const delaunator::Delaunator d(StdCoordinates);

	// Dual Graph Generation
	GenerateGraph(d);
}

/**
 * Generates Graphs and Instances Nodes/Edges
 * @param Delaunator Diagram from Delaunator
 */
void UMapGeneration::GenerateGraph(const delaunator::Delaunator& Delaunator)
{
	// Map to store triangle index to UMapNode
	TMap<int32, UMapNode*> TriangleToNodeMap;

	// Step 1: Create UMapNodes for each triangle's circumcenter
	for (size_t i = 0; i < Delaunator.triangles.size(); i += 3)
	{
		// Indices of the triangle's vertices
		const int32 I1 = Delaunator.triangles[i];
		const int32 I2 = Delaunator.triangles[i + 1];
		const int32 I3 = Delaunator.triangles[i + 2];

		// Coordinates of the vertices
		const FVector2D A(Delaunator.coords[2 * I1], Delaunator.coords[2 * I1 + 1]);
		const FVector2D B(Delaunator.coords[2 * I2], Delaunator.coords[2 * I2 + 1]);
		const FVector2D C(Delaunator.coords[2 * I3], Delaunator.coords[2 * I3 + 1]);

		// Calculate circumcenter
		const FVector2D Circumcenter = UE::Geometry::VectorUtil::Circumcenter(A, B, C);

		// Create and initialize UMapNode
		UMapNode* Node = NewObject<UMapNode>(this, UMapNode::StaticClass());
		Node->Initialize(Circumcenter, A, B, C, this);

		// Store node
		Nodes.Add(Node);
		TriangleToNodeMap.Add(i / 3, Node);
	}

	// Step 2: Establish neighbors and edges
	for (size_t i = 0; i < Delaunator.halfedges.size(); ++i)
	{
		const int32 Opposite = Delaunator.halfedges[i];
		if (Opposite >= 0) // Skip outer edges
		{
			UMapNode* Node1 = TriangleToNodeMap[i / 3];
			UMapNode* Node2 = TriangleToNodeMap[Opposite / 3];

			// Check if nodes are already neighbors
			if (std::ranges::find(Node1->Neighbors, Node2) == Node1->Neighbors.end())
			{
				Node1->AddNeighbor(Node2);

				// Create an edge between Node1 and Node2
				UNodeEdge* Edge = NewObject<UNodeEdge>(this, UNodeEdge::StaticClass());
				Edge->Initialize(Node1->Circumcenter, Node2->Circumcenter, this);

				// Add edge to nodes
				Node1->AddEdge(Edge);
				Node2->AddEdge(Edge);
			}
		}
	}
}

