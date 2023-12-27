/**
 * @author Devin DeMatto
 * @file NodeEdge.h
 */

#pragma once

#include "CoreMinimal.h"
#include "DelaunayHelper.h"
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
	UMapGeneration* MapGenerator = nullptr;

	// Circumcenter A
	FVector2D PointA;

	// Circumcenter B
	FVector2D PointB;

	/// Nodes that are related to this edge
	TArray<UMapNode*> Nodes;

	FLinearColor Color = FLinearColor::White;

	explicit UNodeEdge(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	void SetupEdge(const FVector2D& InPointA, const FVector2D& InPointB, UMapGeneration* InMapGenerator);

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;
};
