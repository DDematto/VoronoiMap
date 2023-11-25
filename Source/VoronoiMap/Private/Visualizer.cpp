#include "Visualizer.h"
#include "SlateBasics.h"
#include "Slate/SlateGameResources.h"

const FLinearColor CenterColor = FLinearColor::Red;
const FLinearColor DelaunayLineColor = FLinearColor::Blue;
const FLinearColor VoronoiEdgeColor = FLinearColor::Green;
const FLinearColor MapBorder = FLinearColor::Yellow;

const float CenterPointSize = 20.0f;
const float LineThickness = 2.0f;

void UVisualizer::SetGraphData(const FVoronoiGraph& VoronoiGraphData, const FDelaunayGraph& DelaunayGraphData)
{
	VoronoiGraph = VoronoiGraphData;
	DelaunayGraph = DelaunayGraphData;
	bHasVoronoiGraphData = true;
	bHasDelaunayGraphData = true;
	InvalidateLayoutAndVolatility();
}


int32 UVisualizer::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 MaxLayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	CachedAllottedGeometry = AllottedGeometry;

	if (!bHasVoronoiGraphData || !bHasDelaunayGraphData)
	{
		return MaxLayerId;
	}

	// Draw Delaunay Triangles with the current LayerId
	int32 BorderLayerId = LayerId + 1;
	DrawBorder(Args, AllottedGeometry, MyCullingRect, OutDrawElements, BorderLayerId, InWidgetStyle, bParentEnabled);

	// Draw Delaunay Triangles with the current LayerId
	int32 DelaunayLayerId = LayerId + 1;
	DrawDelaunayTriangles(Args, AllottedGeometry, MyCullingRect, OutDrawElements, DelaunayLayerId, InWidgetStyle, bParentEnabled);

	// Draw Voronoi Edges with a higher LayerId
	int32 VoronoiEdgeLayerId = DelaunayLayerId + 1;
	DrawVoronoiEdges(Args, AllottedGeometry, MyCullingRect, OutDrawElements, VoronoiEdgeLayerId, InWidgetStyle, bParentEnabled);

	return MaxLayerId;
}

void UVisualizer::DrawBorder(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Define the vertices
	FVector2D Vertex1(0, 0);
	FVector2D Vertex2(MWidth, 0);
	FVector2D Vertex3(MWidth, MHeight);
	FVector2D Vertex4(0, MHeight);

	// Create an array of points for the box outline, closing the loop at the end
	TArray<FVector2D> OutlinePoints = { Vertex1, Vertex2, Vertex3, Vertex4, Vertex1 };

	// Draw the Border
	FSlateDrawElement::MakeLines(
	OutDrawElements,
	LayerId,
	AllottedGeometry.ToPaintGeometry(),
	OutlinePoints,
	ESlateDrawEffect::None,
	MapBorder,
	true,
	1.0f);
}



void UVisualizer::DrawDelaunayTriangles(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	for (const FDelaunayNode& Triangle : DelaunayGraph.Triangles)
	{
		if (Triangle.VertexIndices.Num() < 3)
		{
			continue; // Ensure there are enough vertices to form a triangle
		}

		// Get the positions of the triangle vertices from the original points
		FVector2D Vertex1(MOriginalPoints[Triangle.VertexIndices[0]].X, MOriginalPoints[Triangle.VertexIndices[0]].Y);
		FVector2D Vertex2(MOriginalPoints[Triangle.VertexIndices[1]].X, MOriginalPoints[Triangle.VertexIndices[1]].Y);
		FVector2D Vertex3(MOriginalPoints[Triangle.VertexIndices[2]].X, MOriginalPoints[Triangle.VertexIndices[2]].Y);

		// Draw the triangle
		TArray<FVector2D> TrianglePoints = { Vertex1, Vertex2, Vertex3, Vertex1 }; // Close the loop
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			TrianglePoints,
			ESlateDrawEffect::None,
			DelaunayLineColor,
			true, // bAntialias
			LineThickness
		);
	}
}



void UVisualizer::DrawVoronoiEdges(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	for (int32 i = 0; i < VoronoiGraph.Vertices.Num(); ++i)
	{
		const FVoronoiNode& Node = VoronoiGraph.Vertices[i];
		FVector2D CenterPosition(Node.Position.X, Node.Position.Y);

		for (int32 AdjacentIndex : Node.AdjacentVerticesIndices)
		{
			if (AdjacentIndex == -1) continue; // Skip infinite edges

			FVector2D AdjacentCenterPosition(VoronoiGraph.Vertices[AdjacentIndex].Position.X, VoronoiGraph.Vertices[AdjacentIndex].Position.Y);

			TArray<FVector2D> EdgePoints = { CenterPosition, AdjacentCenterPosition };

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				EdgePoints,
				ESlateDrawEffect::None,
				VoronoiEdgeColor,
				true, // bAntialias
				LineThickness
			);
		}
	}
}


void UVisualizer::CalculateAspectRatio(int OriginalMapWidth, int OriginalMapHeight)
{
	MWidth = OriginalMapWidth;
	MHeight = OriginalMapHeight;
	FVector2D WidgetSize = CachedAllottedGeometry.GetLocalSize();
	MScaleX = WidgetSize.X / OriginalMapWidth;
	MScaleY = WidgetSize.Y / OriginalMapHeight;
}

void UVisualizer::SetOriginalPoints(TArray<FVector> OriginalPoints)
{
	MOriginalPoints = OriginalPoints;
	bHasPoints = true;
}
