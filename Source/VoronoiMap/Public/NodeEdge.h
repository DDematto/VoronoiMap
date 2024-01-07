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


UENUM(BlueprintType)
enum class EEdgeType : uint8
{
	None UMETA(DisplayName = "None"),
	Road UMETA(DisplayName = "Road"),
	River UMETA(DisplayName = "River"),
	Cliff UMETA(DisplayName = "Cliff")
};


UENUM(BlueprintType)
enum class ESelectionState : uint8
{
	Default UMETA(DisplayName = "Default"),
	Selected UMETA(DisplayName = "Selected")
};

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

	UPROPERTY()
	EEdgeType EdgeType = EEdgeType::None;

	UPROPERTY()
	ESelectionState SelectionState = ESelectionState::Default;

	/// Nodes that are related to this edge
	UPROPERTY()
	TArray<UMapNode*> Nodes;

	// Circumcenter A
	FVector2D PointA;

	// Circumcenter B
	FVector2D PointB;

	// Points Making up BezierCurve
	TArray<FVector2D> BezierCurvePoints;

	// Flag if Edge is Inside Map
	bool bIsEdgeInsideMap;

	// Flag if Edge is Partially In Map
	bool bIsPartiallyInMap;

	// Color Of Edge
	FLinearColor Color;

	bool bDrawA = false;
	bool bDrawB = false;

	// Constructor & Initial Setup
	explicit UNodeEdge(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	void SetupEdge(const FVector2D& InPointA, const FVector2D& InPointB, UMapGeneration* InMapGenerator);

	// Edge Positioning & Bezier Calculation

	bool IsPointInsideMap(const FVector2D& Point) const;

	void CalculateBezier();

	////////////////////
	// Edge Selection //
	////////////////////

	void SetEdgeType(EEdgeType NewType);

	void SelectEdge();

	void DeselectEdge();

	void UpdateEdgeColor();

	//////////////////
	//  Event Logic //
	//////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;
};
