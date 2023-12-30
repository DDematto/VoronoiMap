/**
 * @author Devin DeMatto
 * @file MapNode.h
 */

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "NodeEdge.h"
#include "MapNode.generated.h"

class UMapGeneration;
class UNodeEdge;

/**
 * Base Class for Voronoi Node
 */
UCLASS()
class VORONOIMAP_API UMapNode : public UUserWidget
{
	GENERATED_BODY()

public:
	// Reference to Map Generator
	UPROPERTY()
	UMapGeneration* MapGenerator = nullptr;

	// Centroid of Node (Original Point)
	FVector2D Centroid;

	// Neighboring Nodes
	UPROPERTY()
	TArray<UMapNode*> Neighbors;

	// Edges Making up Nodes
	UPROPERTY()
	TArray<UNodeEdge*> Edges;

	// Vertices for Mesh Generation
	TArray<FVector2D> Vertices;

	// Indices for Mesh Generation
	TArray<SlateIndex> Indices;

	// Color of 2D Polygon
	FColor PolygonColor = FColor::MakeRandomColor();

	// Color of Centroid
	FLinearColor CentroidColor = FLinearColor::White;

	// Is Node Currently Selected 
	bool IsSelected = false;

	// Is Node a Border Node
	bool IsBorderNode = false;

	// Draw Vertices Traversal
	bool bDrawVerticesTraversal = false;

	// Default constructor
	explicit UMapNode(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	///////////////////////////
	//  Structure Generation //
	///////////////////////////

	void SetupNode(UMapGeneration*, FVector2D);
	void AddNeighbor(UMapNode*);
	void AddEdge(UNodeEdge*);

	//////////////////////
	//  Mesh Generation //
	//////////////////////

	void BuildMesh();
	void SortEdges();

	// Node Positioning Check
	bool IsInNode(const FVector2D& Point);

	//////////////////
	//  Event Logic //
	//////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};

