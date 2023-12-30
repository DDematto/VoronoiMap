/**
 * @author Devin DeMatto
 * @file NodeEdge.h
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NodeEdge.generated.h"

class UMapGeneration;
class UMapNode;

/**
 * Base Class for Edge of Voronoi Node
 */
UCLASS()
class VORONOIMAP_API UNodeEdge : public UUserWidget
{
	GENERATED_BODY()
public:
	// Reference to Map Generator
	UPROPERTY()
	UMapGeneration* MapGenerator = nullptr;

	// Circumcenter A
	FVector2D PointA;

	// Circumcenter B
	FVector2D PointB;

	// Points Making up BezierCurve
	TArray<FVector2D> BezierCurvePoints;

	/// Nodes that are related to this edge
	TArray<UMapNode*> Nodes;

	// Flag if Edge is Inside Map
	bool bIsEdgeInsideMap;

	// Flag if Edge is Partially In Map
	bool bIsPartiallyInMap;

	// Color Of Edge
	FLinearColor Color = FLinearColor::Black;

	bool bDrawA = false;
	bool bDrawB = false;

	// Constructor & Initial Setup

	explicit UNodeEdge(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	void SetupEdge(const FVector2D& InPointA, const FVector2D& InPointB, UMapGeneration* InMapGenerator);

	// Edge Positioning & Bezier Calculation

	bool IsPointInsideMap(const FVector2D& Point) const;

	void CalculateBezier();

	//////////////////
	//  Event Logic //
	//////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;

};
