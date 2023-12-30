/**
 * @author Devin DeMatto
 * @file MapGeneration.cpp
 */

#include "MapGeneration.h"
#include "DelaunayHelper.h"
#include <algorithm>
#include "FPoissonSampling.h"
#include "MapNode.h"
#include "NodeEdge.h"
#include "VectorUtil.h"

 // Constructor
void UMapGeneration::NativeConstruct()
{
	Super::NativeConstruct();

	SetMapSize(FVector2D(500, 500));

	GenerateMap();
}

//////////////////
//  Event Logic //
//////////////////

int32 UMapGeneration::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	for (const UMapNode* Node : Nodes)
	{
		Node->NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId - 2, InWidgetStyle, bParentEnabled);
	}

	// Decrement the LayerId for UMapGeneration
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId - 1, InWidgetStyle, bParentEnabled);
}

FReply UMapGeneration::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	for (UMapNode* Node : Nodes)
	{
		Node->NativeOnMouseMove(InGeometry, InMouseEvent);
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UMapGeneration::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	for (UMapNode* Node : Nodes)
	{
		Node->NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UMapGeneration::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	for (UMapNode* Node : Nodes)
	{
		Node->NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

/**
 * Creates a Poisson Distribution of Points Based on MapSize
 */
TArray<FVector2D> UMapGeneration::GeneratePoints() const
{
	// Define parameters for Poisson Disk Sampling
	constexpr int Iterations = 30; // Set the number of iterations for the sampling
	constexpr int K = 5; // Amount of Times a Point will attempt to place
	constexpr int Spacing = 50; // Set the Spacing Between Nodes

	// Setup the random stream
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();

	// Call the Poisson Disk Sampling function with Spacing parameter
	return FPoissonSampling::GeneratePoissonDiscSamples(MapSize.X + BoundaryOffset.X, MapSize.Y + BoundaryOffset.Y, Spacing, K, Iterations, RandomStream);
}

// Main Method For Map Generation
void UMapGeneration::GenerateMap()
{
	Nodes.Empty();

	const TArray<FVector2D> Points = GeneratePoints();

	// Generate Delaunay triangulation
	const FDelaunayMesh DelaunayMesh = UDelaunayHelper::CreateDelaunayTriangulation(Points);

	// Dual Graph Generation
	GenerateGraph(DelaunayMesh, Points);

	// Try To Build Mesh, Remove Nodes Out of Bounds, And Clip Nodes in Bounds
	for (const auto& Node : Nodes)
	{
		Node->BuildMesh();
	}

	ProcessInvalidNodes();
}

// Convert to Data Structures
void UMapGeneration::GenerateGraph(const FDelaunayMesh& Delaunator, const TArray<FVector2D>& Points)
{
	// Clear any existing nodes to prepare for generating a new graph.
	Nodes.Empty();

	// Map to keep track of unique edges using triangle index pairs as keys.
	TMap<TPair<FTriangleIndex, FTriangleIndex>, UNodeEdge*> UniqueEdgesMap;

	// A mapping from point indices to their corresponding Voronoi nodes (UMapNode instances).
	TMap<FPointIndex, UMapNode*> PointIndexToNodeMap;

	// Step 1: Create Voronoi nodes for each point in the original set.
	for (int32 i = 0; i < Points.Num(); ++i)
	{
		FPointIndex PointIndex(i);
		FVector2D Point = Points[i] - (BoundaryOffset / 2);

		UMapNode* Node = NewObject<UMapNode>(this, UMapNode::StaticClass());
		Node->SetupNode(this, Point);
		Nodes.Add(Node);
		PointIndexToNodeMap.Add(PointIndex, Node);
	}

	// Step 2: Establish edges and associate them with the Voronoi nodes.
	for (FSideIndex SideIndex = 0; SideIndex.Value < Delaunator.HalfEdges.Num(); ++SideIndex)
	{
		if (!SideIndex.IsValid())
			continue;

		FSideIndex OppositeEdgeIndex = UDelaunayHelper::OppositeHalfEdge(Delaunator, SideIndex);
		if (!OppositeEdgeIndex.IsValid())
			continue;

		// Get the triangles on either side of this edge
		FTriangleIndex CurrentTriangleIndex = UDelaunayHelper::GetTriangleIndexFromHalfEdge(SideIndex);
		FTriangleIndex AdjacentTriangleIndex = UDelaunayHelper::GetTriangleIndexFromHalfEdge(OppositeEdgeIndex);

		// Use the sorted pair of triangle indices to check for unique edges
		TPair<FTriangleIndex, FTriangleIndex> SortedTrianglePair(
			FMath::Min(CurrentTriangleIndex, AdjacentTriangleIndex),
			FMath::Max(CurrentTriangleIndex, AdjacentTriangleIndex)
		);

		UNodeEdge* Edge = nullptr;

		// Check if this edge has already been created
		if (UniqueEdgesMap.Contains(SortedTrianglePair))
		{
			Edge = UniqueEdgesMap[SortedTrianglePair];
		}
		else
		{
			// Create the edge and add it to the map
			FVector2D CircumcenterCurrent = UDelaunayHelper::GetTriangleCircumcenter(
				UDelaunayHelper::ConvertTriangleIDToTriangle(Delaunator, CurrentTriangleIndex)
			) - (BoundaryOffset / 2);

			FVector2D CircumcenterAdjacent = UDelaunayHelper::GetTriangleCircumcenter(
				UDelaunayHelper::ConvertTriangleIDToTriangle(Delaunator, AdjacentTriangleIndex)
			) - (BoundaryOffset / 2);

			Edge = NewObject<UNodeEdge>(this, UNodeEdge::StaticClass());
			Edge->SetupEdge(CircumcenterCurrent, CircumcenterAdjacent, this);

			UniqueEdgesMap.Add(SortedTrianglePair, Edge);
		}

		// Check if the current and adjacent triangles share a common vertex with the node's original point
		TArray<FPointIndex> CurrentTrianglePoints = UDelaunayHelper::PointsOfTriangle(Delaunator, CurrentTriangleIndex);
		TArray<FPointIndex> AdjacentTrianglePoints = UDelaunayHelper::PointsOfTriangle(Delaunator, AdjacentTriangleIndex);

		for (FPointIndex PointIndex : CurrentTrianglePoints)
		{
			if (AdjacentTrianglePoints.Contains(PointIndex)) // This check ensures both triangles share this vertex
			{
				UMapNode* Node = PointIndexToNodeMap[PointIndex];
				if (Node && !Node->Edges.Contains(Edge))
				{
					Node->AddEdge(Edge);

					// Add each node as a neighbor to each other if not already neighbors
					for (FPointIndex OtherPointIndex : CurrentTrianglePoints)
					{
						if (OtherPointIndex != PointIndex)
						{
							UMapNode* OtherNode = PointIndexToNodeMap[OtherPointIndex];
							if (OtherNode && !Node->Neighbors.Contains(OtherNode))
							{
								Node->AddNeighbor(OtherNode);
							}
						}
					}

					for (FPointIndex OtherPointIndex : AdjacentTrianglePoints)
					{
						if (OtherPointIndex != PointIndex)
						{
							UMapNode* OtherNode = PointIndexToNodeMap[OtherPointIndex];
							if (OtherNode && !Node->Neighbors.Contains(OtherNode))
							{
								Node->AddNeighbor(OtherNode);
							}
						}
					}
				}
			}
		}

	}
}

// Removal of Invalid Nodes

void UMapGeneration::MarkNodeForRemoval(UMapNode* Node)
{
	if (Node && !InvalidNodes.Contains(Node))
	{
		InvalidNodes.Add(Node);
	}
}

void UMapGeneration::ProcessInvalidNodes()
{
	TArray<UNodeEdge*> EdgesToRemove;

	// Process nodes for removal
	for (UMapNode* Node : InvalidNodes)
	{
		for (UNodeEdge* Edge : Node->Edges)
		{
			Edge->Nodes.Remove(Node);
			if (Edge->Nodes.Num() == 0)
			{
				EdgesToRemove.AddUnique(Edge);
			}
		}

		for (UMapNode* Neighbor : Node->Neighbors)
		{
			Neighbor->Neighbors.Remove(Node);
		}
	}

	// Remove edges and nodes
	for (UMapNode* Node : InvalidNodes)
	{
		Nodes.Remove(Node);
	}

	InvalidNodes.Empty();
}

