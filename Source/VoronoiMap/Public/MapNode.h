// @author Devin DeMatto \n I am just trying out some silly stuff :P

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapNode.generated.h"

// Forward Declaration
class UMapEdge;

/**
 * Class Describing Node Generated from Map
 */
UCLASS()
class VORONOIMAP_API UMapNode : public UUserWidget
{
	GENERATED_BODY()

public:
	/// Constructor
	UMapNode(const FObjectInitializer& ObjectInitializer);

	/// Destructor
	~UMapNode();

	// Node Position
	FVector Position;

	// Adjacent Vertices
	TArray<int32> AdjacentVerticesIndices;

	// Boundary Node Indicator
	bool bIsBoundaryNode;

	// Circumcenter
	FVector Circumcenter;

	// Area of Cell
	float Area;

	// Connected Edges
	TArray<UMapEdge*> AdjacentEdges;

	// Vertices of the Voronoi cell
	TArray<FVector> CellVertices;

	// Direct neighbors
	TArray<UMapNode*> NeighboringNodes;

	// Perimeter of the Voronoi cell
	float Perimeter;

	/// Calculates circumcenter, area, and perimeter
	void CalculateGeometricProperties();

	/// Determines the shape of the Voronoi cell
	void DetermineCellShape();

	// Gets edges that form the boundary of the cell
	TArray<UMapEdge*> GetBoundaryEdges();
};


UCLASS()
class VORONOIMAP_API UMapEdge : public UObject
{
	GENERATED_BODY()

public:
	// Endpoint A
	UMapNode* NodeA;

	// Endpoint B
	UMapNode* NodeB;

	// Infinite Edge Indicator
	bool bIsInfiniteEdge;

	// Length of the Edge
	float Length;

	// Midpoint of the Edge
	FVector Midpoint;

	// Direction Vector of the Edge
	FVector Direction;

	// Indicates if it's a Delaunay Edge
	bool bIsDelaunayEdge;

	// Enhanced Methods
	void CalculateEdgeProperties();
	void CheckDelaunayCondition();
};