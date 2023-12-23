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
	/// Reference to the Map
	UMapGeneration* MapGenerator = nullptr;

	/// Where the Edge is Located
	FVector2D PointA = FVector2D(0, 0);
	FVector2D PointB = FVector2D(0, 0);

	/// Nodes Connected to this Edge
	std::vector<UMapNode*> Nodes;

	/////////////
	// Methods //
	/////////////

	// Default constructor
	explicit UNodeEdge(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;

	// Initialize method to set up the edge
	void Initialize(const FVector2D& InPointA, const FVector2D& InPointB, UMapGeneration* InMapGenerator);

	// Method to calculate the length of the edge
	double Length() const;
};

