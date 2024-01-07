/**
 * @author Devin DeMatto
 * @file NodeEdge.cpp
 */

#include "NodeEdge.h"
#include "MapGeneration.h"

 /**
  * Initializes the Edge
  * @param InPointA Position on Map
  * @param InPointB Position on Map
  * @param InMapGenerator Map Reference
  */
void UNodeEdge::SetupEdge(const FVector2D& InPointA, const FVector2D& InPointB, UMapGeneration* InMapGenerator) {
	MapGenerator = InMapGenerator;
	PointA = InPointA;
	PointB = InPointB;
	bIsEdgeInsideMap = IsPointInsideMap(PointA) && IsPointInsideMap(PointB);
	bIsPartiallyInMap = !IsPointInsideMap(PointA) && IsPointInsideMap(PointB) || IsPointInsideMap(PointA) && !IsPointInsideMap(PointB);
}

void UNodeEdge::CalculateBezier() {
	// Calculate the midpoint of the line segment
	const FVector2D MidPoint = (PointA + PointB) / 2.0f;

	// Calculate a vector perpendicular to the line segment
	const FVector2D Perpendicular = FVector2D(PointB.Y - PointA.Y, PointA.X - PointB.X).GetSafeNormal();

	// Set a distance for how far the control points should be from the midpoint
	const float ControlPointDistance = (PointA - PointB).Size() / 3.0f; // Adjust this factor as needed

	// Define the control points dynamically
	const FVector2D ControlPoint1 = MidPoint + Perpendicular * ControlPointDistance;
	const FVector2D ControlPoint2 = MidPoint - Perpendicular * ControlPointDistance;

	// Define the control points array including PointA, PointB, and the dynamically calculated control points
	FVector ControlPoints[4];
	ControlPoints[0] = FVector(PointA.X, PointA.Y, 0); // Convert PointA to FVector
	ControlPoints[1] = FVector(ControlPoint1.X, ControlPoint1.Y, 0); // First dynamic control point
	ControlPoints[2] = FVector(ControlPoint2.X, ControlPoint2.Y, 0); // Second dynamic control point
	ControlPoints[3] = FVector(PointB.X, PointB.Y, 0); // Convert PointB to FVector

	// Evaluate the Bezier Curve
	TArray<FVector> OutPoints;
	FVector::EvaluateBezier(ControlPoints, 12, OutPoints);

	// Clear previous points and populate BezierCurvePoints
	BezierCurvePoints.Empty();
	for (const FVector& Point : OutPoints) {
		BezierCurvePoints.Add(FVector2D(Point.X, Point.Y));
	}
}

////////////////////
// Edge Selection //
////////////////////

void UNodeEdge::SetEdgeType(const EEdgeType NewType)
{
	EdgeType = NewType;
	UpdateEdgeColor();
}

void UNodeEdge::SelectEdge()
{
	SelectionState = ESelectionState::Selected;
	UpdateEdgeColor();
}

void UNodeEdge::DeselectEdge()
{
	SelectionState = ESelectionState::Default;
	UpdateEdgeColor();
}

void UNodeEdge::UpdateEdgeColor()
{
	if (SelectionState == ESelectionState::Selected)
	{
		Color = FLinearColor::Red;
	}
	else
	{
		switch (EdgeType)
		{
		case EEdgeType::Road:
			Color = FColor(50, 50, 50, 255); // Dark gray for roads
			break;
		case EEdgeType::River:
			Color = FColor(0, 150, 255, 255); // Clear blue for rivers
			break;
		case EEdgeType::Cliff:
			Color = FColor(100, 100, 100, 255); // Gray for cliffs
			break;
		default:
			Color = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f); // Transparent for undefined edge types
		}
	}
}

//////////////////
//  Event Logic //
//////////////////

int32 UNodeEdge::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (MapGenerator)
	{
		// Draw Edge
		MapGenerator->DrawLines(InContext, AllottedGeometry, BezierCurvePoints, Color, 2);

		// Draw Point A
		if (bDrawA)
		{
			InContext.LayerId += 1;
			MapGenerator->DrawPoint(InContext, AllottedGeometry, PointA, FLinearColor::Red, FVector2D(5, 5) * 2);
		}

		// Draw Point B
		if (bDrawA)
		{
			InContext.LayerId += 1;
			MapGenerator->DrawPoint(InContext, AllottedGeometry, PointB, FLinearColor::Red, FVector2D(5, 5) * 2);
		}
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

////////////////////////////
// Edge Calculation Logic //
////////////////////////////

bool UNodeEdge::IsPointInsideMap(const FVector2D& Point) const
{
	const FVector2D MapSize = MapGenerator->GetMapSize();

	// Assuming the map's bottom left corner is at (0,0) and the top right corner is at (MapSize.X, MapSize.Y)
	const bool bIsInsideX = Point.X >= 0 && Point.X <= MapSize.X;
	const bool bIsInsideY = Point.Y >= 0 && Point.Y <= MapSize.Y;

	return bIsInsideX && bIsInsideY;
}