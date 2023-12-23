/**
 * @author Devin DeMatto
 * @file MapNode.cpp
 */

#include "MapNode.h"
#include "MapGeneration.h"
#include "NodeEdge.h"

 /**
  *
  * @param InCircumcenter Circumcenter Calculated
  * @param InPointA Point A (Delaunay)
  * @param InPointB Point B (Delaunay)
  * @param InPointC Point C (Delaunay)
  * @param InMapGenerator Map Reference
  */
void UMapNode::Initialize(const FVector2D& InCircumcenter, const FVector2D& InPointA, const FVector2D& InPointB, const FVector2D& InPointC, UMapGeneration* InMapGenerator)
{
	Circumcenter = InCircumcenter;
	PointA = InPointA;
	PointB = InPointB;
	PointC = InPointC;
	MapGenerator = InMapGenerator;

	// Generate a random color
	const float Red = FMath::RandRange(0.0f, 1.0f);
	const float Green = FMath::RandRange(0.0f, 1.0f);
	const float Blue = FMath::RandRange(0.0f, 1.0f);
	Color = FLinearColor(Red, Green, Blue, 1.0f);
}

/////////////////////
//  Painting Logic //
/////////////////////

int32 UMapNode::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const auto MyContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (MapGenerator)
	{
		// Draw The Voronoi Polygon
		LayerId += 1;
		MapGenerator->DrawPolygon(MyContext, AllottedGeometry, this);

		// Draw Delaunay Edges
		if (MapGenerator->bDrawDelaunayEdges)
		{
			LayerId += 1;
			const auto UpdatedContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

			// Draw the Delaunay edges
			MapGenerator->DrawLine(UpdatedContext, AllottedGeometry, PointA, PointB, FLinearColor::White);
			MapGenerator->DrawLine(UpdatedContext, AllottedGeometry, PointB, PointC, FLinearColor::White);
			MapGenerator->DrawLine(UpdatedContext, AllottedGeometry, PointC, PointA, FLinearColor::White);
		}

		// Draw Voronoi Edges
		if (MapGenerator->bDrawVoronoiEdges) {
			for (const UNodeEdge* Edge : Edges)
			{
				LayerId += 1;
				Edge->NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
			}
		}

		// Draw the Voronoi Edge Corners
		if (MapGenerator->bDrawVoronoiCorners)
		{
			LayerId += 1;
			const auto UpdatedContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
			MapGenerator->DrawPoint(UpdatedContext, AllottedGeometry, Circumcenter, FLinearColor::Blue);
		}
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FReply UMapNode::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FVector2D MousePosition = MapGenerator->GetMousePositionInVirtualSpace();

	// Check if MousePosition is within this Voronoi Node
	if (IsPointInsideVoronoiNode(MousePosition))
	{
		// Update rendering of this node to Green
		SetNodeColor(FLinearColor::Green);

		// Update neighboring nodes to Red
		for (UMapNode* Neighbor : Neighbors)
		{
			Neighbor->SetNodeColor(FLinearColor::Red);
		}
	}
	else
	{
		// Reset Color
		SetNodeColor(FLinearColor::White);

		// Reset neighbors' color
		for (UMapNode* Neighbor : Neighbors)
		{
			Neighbor->SetNodeColor(FLinearColor::White);
		}
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

bool UMapNode::IsPointInsideVoronoiNode(const FVector2D& Point) const
{
	for (const UNodeEdge* Edge : Edges)
	{
		// Assuming each edge has two points: PointA and PointB
		const FVector2D A = Edge->PointA;
		const FVector2D B = Edge->PointB;

		// Compute the cross product of the vector from A to B and the vector from A to the Point
		const float CrossProduct = (B.X - A.X) * (Point.Y - A.Y) - (B.Y - A.Y) * (Point.X - A.X);

		// If the cross product is negative, the point is on the right side of the edge (outside for a convex polygon)
		if (CrossProduct < 0)
		{
			return false;
		}
	}

	// The point is on the left side of all edges, so it's inside the polygon
	return true;
}

//////////////////////
// Dual Graph Logic //
//////////////////////

/**
 * Adds Nearby Node as Neighbor
 * @param Neighbor Voronoi Node
 */
void UMapNode::AddNeighbor(UMapNode* Neighbor) {
	Neighbors.push_back(Neighbor);
}


/**
 * Adds Edge to Node and Adds Node Reference to Edge
 * @param Edge Related Edge
 */
void UMapNode::AddEdge(UNodeEdge* Edge) {
	Edge->Nodes.push_back(this);
	Edges.push_back(Edge);
}

///////////////////////
// Setters & Getters //
///////////////////////

void UMapNode::SetNodeColor(const FLinearColor& UpdatedColor)
{
	Color = UpdatedColor;
}