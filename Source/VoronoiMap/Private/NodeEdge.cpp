/**
 * @author Devin DeMatto
 * @file NodeEdge.cpp
 */

#include "NodeEdge.h"
#include "MapGeneration.h"
#include "MapNode.h"

int32 UNodeEdge::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (MapGenerator)
	{
		MapGenerator->DrawLine(InContext, AllottedGeometry, PointA, PointB, FLinearColor::White);
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

/**
 * Initializes the Edge
 * @param InPointA Position on Map
 * @param InPointB Position on Map
 * @param InMapGenerator Map Reference
 */
void UNodeEdge::Initialize(const FVector2D& InPointA, const FVector2D& InPointB, UMapGeneration* InMapGenerator) {
	PointA = InPointA;
	PointB = InPointB;
	MapGenerator = InMapGenerator;
}

/**
 * Calculates Length of Edge
 * @return Length of Edge
 */
double UNodeEdge::Length() const {
	return (PointB - PointA).Size();
}