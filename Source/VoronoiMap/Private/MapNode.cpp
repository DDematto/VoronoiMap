/**
 * @author Devin DeMatto
 * @file MapNode.cpp
 */

#include "MapNode.h"
#include "MapGeneration.h"
#include "NodeEdge.h"

int32 UMapNode::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (MapGenerator)
	{
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

		// Draw the Voronoi Point
		if (MapGenerator->bDrawVoronoiCorners)
		{
			LayerId += 1;
			const auto UpdatedContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
			MapGenerator->DrawPoint(UpdatedContext, AllottedGeometry, Circumcenter, FLinearColor::Blue);
		}

	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}


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
}


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