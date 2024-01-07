/**
 * @author Devin DeMatto
 * @file MapNode.cpp
 */

#include "MapNode.h"
#include "NodeEdge.h"
#include "MapGeneration.h"
#include "Biomes.h"
#include "TerrainGenerator.h"
#include "CompGeom/PolygonTriangulation.h"
#include "Algo/Reverse.h"

 //////////////////////////////////
 // Logic for Building Structure //
 //////////////////////////////////

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

///////////////////////////
// Building Polygon Mesh //
///////////////////////////

void UMapNode::BuildMesh()
{
	// Assume Node is Outside Until Proven Otherwise
	bool bNodeIsOutside = true;

	// Sort Edges Array
	SortEdges();

	// Add Vertices
	FVector2D LastPointAdded;
	for (int32 i = 0; i < Edges.Num(); ++i)
	{
		UNodeEdge* CurrentEdge = Edges[i];

		// Check if BezierCurvePoints are empty and calculate if needed
		if (CurrentEdge->BezierCurvePoints.IsEmpty())
		{
			CurrentEdge->CalculateBezier();
		}

		// Declares Node Inside Map
		if (bNodeIsOutside && (CurrentEdge->bIsEdgeInsideMap || CurrentEdge->bIsPartiallyInMap))
		{
			bNodeIsOutside = false;
		}

		TArray<FVector2D> PointsToAdd;

		// If it's the first edge or the last point matches the first bezier point, add as is
		if (i == 0 || LastPointAdded.Equals(CurrentEdge->BezierCurvePoints[0]))
		{
			PointsToAdd = CurrentEdge->BezierCurvePoints;
		}
		else // If the last point doesn't match, reverse the order
		{
			PointsToAdd = CurrentEdge->BezierCurvePoints;
			Algo::Reverse(PointsToAdd);
		}

		// Update the last point added for comparison in the next iteration
		LastPointAdded = PointsToAdd.Last();

		// Add points to Vertices set
		Vertices.Append(PointsToAdd);
	}

	// If Node is Outside We Remove it
	if (bNodeIsOutside)
	{
		return MapGenerator->MarkNodeForRemoval(this);
	}

	// Building Indices
	TArray<UE::Geometry::FIndex3i> OutTriangles;
	PolygonTriangulation::TriangulateSimplePolygon(Vertices, OutTriangles);

	for (const UE::Geometry::FIndex3i& Triangle : OutTriangles)
	{
		Indices.Add(Triangle.A);
		Indices.Add(Triangle.B);
		Indices.Add(Triangle.C);
	}
}

void UMapNode::SortEdges()
{
	// Map to store connections
	TMap<FVector2D, TArray<UNodeEdge*>> EdgeMap;
	TSet<UNodeEdge*> CheckedEdges; // Set to keep track of checked edges

	// Populate the map and set
	for (UNodeEdge* Edge : Edges)
	{
		EdgeMap.FindOrAdd(Edge->PointA).Add(Edge);
		EdgeMap.FindOrAdd(Edge->PointB).Add(Edge);
		CheckedEdges.Add(Edge);
	}

	TArray<UNodeEdge*> SortedEdges;
	SortedEdges.Reserve(Edges.Num()); // Preallocate memory

	UNodeEdge* CurrentEdge = Edges[0];
	SortedEdges.Add(CurrentEdge);
	CheckedEdges.Remove(CurrentEdge); // Remove from checked set

	FVector2D CurrentEndpoint = CurrentEdge->PointB;

	while (SortedEdges.Num() < Edges.Num())
	{
		TArray<UNodeEdge*>* ConnectedEdges = EdgeMap.Find(CurrentEndpoint);
		if (!ConnectedEdges)
		{
			break;
		}

		bool bFoundNextEdge = false;

		for (int32 i = 0; i < ConnectedEdges->Num(); ++i)
		{
			UNodeEdge* Edge = (*ConnectedEdges)[i];
			if (CheckedEdges.Contains(Edge))
			{
				if (Edge->PointA == CurrentEndpoint || Edge->PointB == CurrentEndpoint)
				{
					SortedEdges.Add(Edge);
					CurrentEndpoint = (Edge->PointA == CurrentEndpoint) ? Edge->PointB : Edge->PointA;
					CheckedEdges.Remove(Edge);
					bFoundNextEdge = true;
					break;
				}
			}
		}

		if (!bFoundNextEdge)
		{
			break;
		}
	}

	Edges = MoveTemp(SortedEdges);
}

////////////////////////////
// Node Positioning Check //
////////////////////////////

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

////////////////////
// Node Selection //
////////////////////

void UMapNode::Default()
{
	for (UNodeEdge* Edge : Edges)
	{
		Edge->DeselectEdge();
	}
}

void UMapNode::Selected()
{
	for (UNodeEdge* Edge : Edges)
	{
		Edge->SelectEdge();
	}
}

////////////////////
// Biome Creation //
////////////////////

// Updates Node Based on Height & Biome Information
void UMapNode::UpdateNode()
{
	const UTerrainGenerator* Terrain = MapGenerator->TerrainGen;

	FColor TempColor;
	switch (BiomeType)
	{
	case EBiomeType::Plains:
		TempColor = FColor::Green;
		break;
	case EBiomeType::Sea:
		TempColor = FColor(0, 102, 235, 255);
		Height = 120;
		break;
	case EBiomeType::Forest:
		TempColor = FColor(0, 128, 0, 255);
		break;
	}

	const float NormalizedHeight = (Height - Terrain->SeaLevel) / (MapGenerator->TerrainGen->MaxHeight - MapGenerator->TerrainGen->SeaLevel);
	const float ShadeFactor = 0.5f + 0.5f * NormalizedHeight;

	// Easy Switch for now, will change later
	if constexpr (constexpr bool bWipShowHeightMap = true)
	{
		Color = FColor(255 * ShadeFactor, 255 * ShadeFactor, 255 * ShadeFactor, TempColor.A);

		if (BiomeType == EBiomeType::Sea)
		{
			Color = FColor(0, 102, 235, 255);
		}

	}
	else
	{
		Color = FColor(TempColor.R * ShadeFactor, TempColor.G * ShadeFactor, TempColor.B * ShadeFactor, TempColor.A);
	}
}

/////////////
// Setters //
/////////////

EBiomeType UMapNode::GetBiome() const { return BiomeType; }

void UMapNode::SetBiome(const EBiomeType Biome, const bool bIsUpdate)
{
	BiomeType = Biome;

	if (bIsUpdate) UpdateNode();
}

float UMapNode::GetHeight() const { return Height; }

void UMapNode::SetHeight(const float Value, const bool bIsUpdate)
{
	const UTerrainGenerator* Terrain = MapGenerator->TerrainGen;
	Height = FMath::Clamp(Value, Terrain->SeaLevel, Terrain->MaxHeight);

	if (bIsUpdate) UpdateNode();
}

FVector2D UMapNode::GetCentroid() const { return Centroid; }

//////////////////
//  Event Logic //
//////////////////

int32 UMapNode::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (MapGenerator)
	{
		auto Context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		MapGenerator->DrawPolygon(Context, AllottedGeometry, Vertices, Indices, Color);

		if (MapGenerator->GetSelectedNode() == this && bDrawVerticesTraversal)
		{
			constexpr FLinearColor StartColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // Cyan (mixture of green and blue)
			constexpr FLinearColor EndColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Bright red

			const int VertexCount = Vertices.Num();
			for (int i = 0; i < VertexCount; ++i)
			{
				const float LeperFactor = static_cast<float>(i) / (VertexCount - 1);
				FLinearColor CalculatedColor = FLinearColor::LerpUsingHSV(StartColor, EndColor, LeperFactor);

				Context.LayerId += 1;
				MapGenerator->DrawPoint(Context, AllottedGeometry, Vertices[i], CalculatedColor, FVector2D(1, 1) * 10);
			}
		}

		if (MapGenerator->bDrawVoronoiEdges)
		{
			LayerId += 1;
			for (const UNodeEdge* Edge : Edges)
			{
				if (Edge->Nodes[0] == this)
				{
					Edge->NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
				}
			}
		}

		if (MapGenerator->bDrawVoronoiCentroids)
		{
			Context.LayerId += 1;
			MapGenerator->DrawPoint(Context, AllottedGeometry, Centroid, CentroidColor, FVector2D(2.0f, 2.0f));
		}

		if (MapGenerator->bDrawDelaunayTriangles && MapGenerator->GetSelectedNode() == this)
		{
			for (const UMapNode* Neighbor : Neighbors)
			{
				Context.LayerId += 1;
				MapGenerator->DrawLine(Context, AllottedGeometry, Centroid, Neighbor->Centroid, FLinearColor::Red, 1.0);
			}
		}
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FReply UMapNode::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsInNode(MapGenerator->GetMousePositionInVirtualSpace()))
	{
		MapGenerator->SetSelectedNode(this);
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && MapGenerator->GetSelectedNode() == this)
	{
		// Log Centroid
		//UE_LOG(LogTemp, Warning, TEXT("Node Centroid: (%f, %f)"), Centroid.X, Centroid.Y);

		// Log Neighbors
		//for (const UMapNode* Neighbor : Neighbors)
		//{
		//	const FVector2D NeighborCentroid = Neighbor->Centroid;
		//	UE_LOG(LogTemp, Warning, TEXT("Neighbor Centroid: (%f, %f)"), NeighborCentroid.X, NeighborCentroid.Y);
		//}

		// Log Edges
		// UE_LOG(LogTemp, Warning, TEXT("Edges Total: %d"), Edges.Num());
		//for (const UNodeEdge* Edge : Edges)
		//{
		// UE_LOG(LogTemp, Warning, TEXT("Edge Points: A(%f, %f), B(%f, %f)"), Edge->PointA.X, Edge->PointA.Y, Edge->PointB.X, Edge->PointB.Y);
		//}

		// Log Vertices for Mesh Generation
		//for (const FVector2D& Vertex : Vertices)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Mesh Vertex: (%f, %f)"), Vertex.X, Vertex.Y);
		//}

		// Log Indices for Mesh Generation
		//for (const SlateIndex& Index : Indices)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Mesh Index: %d"), Index);
		//}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UMapNode::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{


	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UMapNode::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
