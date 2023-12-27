/**
 * @author Devin DeMatto
 * @file MapNode.cpp
 */

#include "MapNode.h"
#include "MapGeneration.h"
#include "NodeEdge.h"

 /**
  * Setups Node with Critical Information
  * @param InMapGenerator Map Generator
  * @param Point Centroid Of Node
  */
void UMapNode::SetupNode(UMapGeneration* InMapGenerator, const FVector2D Point)
{
	Centroid = Point;
	MapGenerator = InMapGenerator;
}

/**
 * Adds Nearby Node as Neighbor
 * @param Neighbor Voronoi Node
 */
void UMapNode::AddNeighbor(UMapNode* Neighbor) {
	Neighbors.Add(Neighbor);
	Neighbor->Neighbors.Add(this);
}

/**
 * Adds Edge to Node and Adds Node Reference to Edge
 * @param Edge Related Edge
 */
void UMapNode::AddEdge(UNodeEdge* Edge) {
	Edge->Nodes.Add(this);
	Edges.Add(Edge);
}

/**
 * Creates Vertices and Index Array For Region Checking and Mesh Generation
 */
void UMapNode::BuildMesh()
{
	// Clear previous data
	Vertices.Empty();
	Indices.Empty();

	if (Edges.IsEmpty())
	{
		return;
	}

	// Start with the first edge
	UNodeEdge* CurrentEdge = Edges[0];
	const FVector2D StartPoint = CurrentEdge->PointA;
	FVector2D EndPoint = CurrentEdge->PointB;

	// Keep track of the edges we've already used
	TSet<UNodeEdge*> UsedEdges;
	UsedEdges.Add(CurrentEdge);

	// Add the start point of the first edge
	Vertices.Add(StartPoint);

	bool bIsClosed = false;
	bool bDisconnected = false; // Flag for disconnected polygon

	while (!bIsClosed)
	{
		Vertices.Add(EndPoint);

		UNodeEdge* NextEdge = nullptr;

		// Find the next edge
		for (UNodeEdge* Edge : Edges)
		{
			if (UsedEdges.Contains(Edge))
			{
				continue;
			}

			if (Edge->PointA == EndPoint)
			{
				NextEdge = Edge;
				EndPoint = Edge->PointB;
				break;
			}
			if (Edge->PointB == EndPoint)
			{
				NextEdge = Edge;
				EndPoint = Edge->PointA;
				break;
			}
		}

		if (NextEdge)
		{
			UsedEdges.Add(NextEdge);
			CurrentEdge = NextEdge;
		}
		else
		{
			// No connecting edge found, polygon is disconnected
			bDisconnected = true;
			break;
		}

		// Check if we have looped back to the start
		if (EndPoint == StartPoint)
		{
			bIsClosed = true;
		}
	}

	if (bDisconnected)
	{
		// Set CentroidColor to Red
		CentroidColor = FLinearColor::Red;
	}
	else
	{
		// Create indices for triangulation
		// Assuming a convex polygon
		for (int32 i = 1; i < Vertices.Num() - 1; i++)
		{
			Indices.Add(0);
			Indices.Add(i);
			Indices.Add(i + 1);
		}
	}
}

bool UMapNode::IsInNode(const FVector2D& Point)
{
	const int32 NumVertices = Vertices.Num();
	if (NumVertices < 3)
	{
		return false;
	}

	bool bIsInside = false;
	for (int32 i = 0, j = NumVertices - 1; i < NumVertices; j = i++)
	{
		if (((Vertices[i].Y > Point.Y) != (Vertices[j].Y > Point.Y)) &&
			(Point.X < (Vertices[j].X - Vertices[i].X) * (Point.Y - Vertices[i].Y) / (Vertices[j].Y - Vertices[i].Y) + Vertices[i].X))
		{
			bIsInside = !bIsInside;
		}
	}

	return bIsInside;
}

/////////////////////
//  Painting Logic //
/////////////////////

int32 UMapNode::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (MapGenerator)
	{
		LayerId += 1;
		const auto Context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		MapGenerator->DrawPolygon(Context, AllottedGeometry, Vertices, Indices, PolygonColor);

		if (MapGenerator->bDrawVoronoiEdges)
		{
			LayerId += 1;
			for (const UNodeEdge* Edge : Edges)
			{
				Edge->NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
			}
		}

		if (MapGenerator->bDrawVoronoiCentroids)
		{
			LayerId += 1;
			const auto UpdatedContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
			MapGenerator->DrawPoint(UpdatedContext, AllottedGeometry, Centroid, CentroidColor);
		}

		if (MapGenerator->bDrawDelaunayTriangles && IsSelected)
		{
			LayerId += 1;
			const auto UpdatedContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
			for (const UMapNode* Neighbor : Neighbors)
			{
				MapGenerator->DrawLine(UpdatedContext, AllottedGeometry, Centroid, Neighbor->Centroid, FLinearColor::Black, 1.0);
			}
		}
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FReply UMapNode::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	IsSelected = IsInNode(MapGenerator->GetMousePositionInVirtualSpace());

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}
