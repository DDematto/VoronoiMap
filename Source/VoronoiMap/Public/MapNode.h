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

	// References to Delaunay and Voronoi Edges
	TArray<UMapEdge*> DelaunayEdges;
	TArray<UMapEdge*> VoronoiEdges;

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

	// Method to get corresponding Voronoi/Delaunay edge
	UMapEdge* GetCorrespondingEdge(UMapEdge* Edge, bool bIsDelaunay);
};


