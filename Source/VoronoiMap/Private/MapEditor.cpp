/*
* @author Devin DeMatto
* Map Editor Class For Visualizing Generation Data and Manipulating
*/

#include "MapEditor.h"
#include "MapNode.h"

const int OFFSET = 100;

/// Constructor
UMapEditor::UMapEditor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


/**
 * Custom paint logic for UMapGeneration.
 */
int32 UMapEditor::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!MapGenerationInstance.IsValid())
	{
		return LayerId;
	}

	// Initialize context with the necessary parameters
	FPaintContext Context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// Draw points if the flag is set
	if (bShowPoints)
	{
		DrawPoints(Context);
		Context.LayerId++;
	}

	// Draw Voronoi edges if the flag is set
	if (bShowVoronoiEdges)
	{
		DrawVoronoiEdges(Context);
		Context.LayerId++;
	}

	// Draw Delaunay edges if the flag is set
	if (bShowDelaunayEdges)
	{
		DrawDelaunayEdges(Context);
		Context.LayerId++;
	}

	// Draw the bounding box if the flag is set
	if (bShowBoundingBox)
	{
		DrawBoundingBox(Context);
		Context.LayerId++;
	}

	// Return the final layer index
	return Context.LayerId;
}

/**
 * Draws points on the canvas.
 */
void UMapEditor::DrawPoints(const FPaintContext& Context) const
{
	//// Define the color and size for the points
	//const FLinearColor PointColor = FLinearColor::Red;
	//const FVector2f PointSize(1.0f, 1.0f); // Updated to FVector2f

	//TArray<FVector> Points = MapGenerationInstance->GetPoints();

	//for (const FVector& Point : Points)
	//{
	//	// Convert FVector to 2D, assuming Z is ignored for this 2D representation
	//	FVector2D PointLocation(Point.X + OFFSET, Point.Y + OFFSET); // Convert to FVector2D

	//	// Create the PaintGeometry for each point
	//	FPaintGeometry PaintGeom = Context.AllottedGeometry.ToPaintGeometry(PointLocation, PointSize);

	//	// Draw each point as a small box
	//	FSlateDrawElement::MakeBox(Context.OutDrawElements, Context.LayerId, PaintGeom, FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, PointColor);
	//}
}


/**
 * Draws Voronoi edges on the canvas.
 */
void UMapEditor::DrawVoronoiEdges(const FPaintContext& Context) const
{
	//TArray<UMapNode*> Nodes = MapGenerationInstance->GetNodes();

	//for (UMapNode* Node : Nodes)
	//{
	//	for (int AdjIndex : Node->AdjacentVerticesIndices)
	//	{
	//		if (AdjIndex < Nodes.Num())
	//		{
	//			UMapNode* AdjNode = Nodes[AdjIndex];
	//			TArray<FVector2D> LinePoints = {
	//				FVector2D(Node->Position.X + OFFSET, Node->Position.Y + OFFSET),
	//				FVector2D(AdjNode->Position.X + OFFSET, AdjNode->Position.Y + OFFSET)
	//			};

	//			FSlateDrawElement::MakeLines(
	//				Context.OutDrawElements,
	//				Context.LayerId,
	//				Context.AllottedGeometry.ToPaintGeometry(),
	//				LinePoints,
	//				ESlateDrawEffect::None,
	//				FLinearColor::Green,
	//				true,
	//				1.0f
	//			);
	//		}
	//	}
	//}
}


/**
 * Draws Delaunay edges on the canvas.
 */
void UMapEditor::DrawDelaunayEdges(const FPaintContext& Context) const
{
	//if (!MapGenerationInstance.IsValid())
	//	return;

	//TArray<FDelaunay> Triangles = MapGenerationInstance->GetTriangles();
	//TArray<FVector> Points = MapGenerationInstance->GetPoints();

	//for (const FDelaunay& Triangle : Triangles)
	//{
	//	for (int i = 0; i < Triangle.VertexIndices.Num(); ++i)
	//	{
	//		FVector StartPoint = Points[Triangle.VertexIndices[i]];
	//		FVector EndPoint = Points[Triangle.VertexIndices[(i + 1) % Triangle.VertexIndices.Num()]];

	//		TArray<FVector2D> LinePoints = {
	//			FVector2D(StartPoint.X + OFFSET, StartPoint.Y + OFFSET),
	//			FVector2D(EndPoint.X + OFFSET, EndPoint.Y + OFFSET)
	//		};

	//		FSlateDrawElement::MakeLines(
	//			Context.OutDrawElements,
	//			Context.LayerId,
	//			Context.AllottedGeometry.ToPaintGeometry(),
	//			LinePoints,
	//			ESlateDrawEffect::None,
	//			FLinearColor::Blue,
	//			true,
	//			1.0f
	//		);
	//	}
	//}
}



/**
 * Draws the bounding box of the diagram.
 */
void UMapEditor::DrawBoundingBox(const FPaintContext& Context) const
{
	const int Width = MapGenerationInstance->GetWidth();
	const int Height = MapGenerationInstance->GetHeight();

	// Define the vertices of the bounding box
	const FVector2D Vertex1(OFFSET, OFFSET);
	const FVector2D Vertex2(Width + OFFSET, OFFSET);
	const FVector2D Vertex3(Width + OFFSET, Height + OFFSET);
	const FVector2D Vertex4(OFFSET, Height + OFFSET);

	// Create an array of points for the box outline, closing the loop at the end
	const TArray<FVector2D> OutlinePoints = { Vertex1, Vertex2, Vertex3, Vertex4, Vertex1 };

	// Define the line color and thickness
	const FLinearColor LineColor = FLinearColor::Yellow;
	constexpr float LineThickness = 2.0f;

	// Draw the border
	FSlateDrawElement::MakeLines(Context.OutDrawElements, Context.LayerId, Context.AllottedGeometry.ToPaintGeometry(), OutlinePoints,
		ESlateDrawEffect::None, LineColor, true, LineThickness);
}




