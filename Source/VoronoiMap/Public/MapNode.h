/**
 * @author Devin DeMatto
 * @file MapNode.h
 */

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
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
	// Reference to Map
	UMapGeneration* MapGenerator = nullptr;

	// All Neighbor Nodes
	std::vector<UMapNode*> Neighbors;

	// All Edges Making up Node
	std::vector<UNodeEdge*> Edges;

	// Circumcenter of Voronoi Node On Map
	FVector2D Circumcenter;

	// Delaunay Points
	FVector2D PointA;
	FVector2D PointB;
	FVector2D PointC;

	// What Color the Polygon will be
	FLinearColor Color = FLinearColor::Green;

	/////////////
	// Methods //
	/////////////

	// Default constructor
	explicit UMapNode(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	// Paint Method
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;

	/// Checks if Node is Being Hovered Over
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	bool IsPointInsideVoronoiNode(const FVector2D& Point) const;
	void SetNodeColor(const FLinearColor& Color);


	// Method to initialize the node
	void Initialize(const FVector2D& InCircumcenter, const FVector2D& InPointA, const FVector2D& InPointB, const FVector2D& InPointC, UMapGeneration* InMapGenerator);

	// Methods to add neighbors
	void AddNeighbor(UMapNode* Neighbor);

	// Add Edge & Reference to This Node
	void AddEdge(UNodeEdge* Edge);
};

