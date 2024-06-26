/**
 * Functionality For MapViewer and Test Class
 * @author Devin DeMatto
 * @file InteractiveMap.cpp
 */

#include "InteractiveMap.h"

 /////////////////
   // Constructor //
   /////////////////

   /**
	* Default Constructor, Sets up Control for Key bind Manipulation
	* @param ObjectInitializer
	*/
UInteractiveMap::UInteractiveMap(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UInteractiveMap::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	SetClipping(EWidgetClipping::ClipToBoundsWithoutIntersecting);
}

void UInteractiveMap::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D LastWidgetSize = MyGeometry.GetLocalSize();

	// Check if the size has changed since the last tick
	if (WidgetSize != LastWidgetSize)
	{
		WidgetSize = LastWidgetSize;

		// Calculate the center of the scaled map
		ViewportPosition = MapSize / 2;

		// Calculate ViewportSize on Startup
		UpdateViewportSize();
	}
}

//////////////////////
//  EVENTS/HANDLERS //
//////////////////////

/**
 * Sets up Mouse Wheel to Handle Zooming Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UInteractiveMap::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	constexpr float ZoomDelta = 0.05f;
	ZoomLevel = FMath::Clamp(ZoomLevel + (InMouseEvent.GetWheelDelta() * ZoomDelta), 0.0f, 1.0f);

	// Update the viewport size after changing the zoom level
	UpdateViewportSize();

	// Adjust the viewport position by the zoom offset
	ViewportPosition = MousePositionInVirtualSpace;

	// Reposition the viewport if needed
	RepositionViewportIfNeeded();

	return FReply::Handled();
}

/**
 * Sets up Panning Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UInteractiveMap::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bIsPanning = true;
	}
	return FReply::Unhandled();
}

/**
 * Sets up Panning Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UInteractiveMap::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && bIsPanning)
	{
		bIsPanning = false;
	}
	return FReply::Unhandled();
}

/**
 * Sets up Panning Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UInteractiveMap::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UUserWidget::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (bIsPanning)
	{
		// Convert the current mouse position to virtual space
		const FVector2D CurrentMousePositionInVirtualSpace = TranslateToVirtualSpace(InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()));

		// Calculate the delta in virtual space
		const FVector2D DeltaInVirtualSpace = CurrentMousePositionInVirtualSpace - MousePositionInVirtualSpace;

		// Update the viewport position by the delta
		ViewportPosition -= DeltaInVirtualSpace;

		// Clamp the new viewport position to the map boundaries if panning limits are enabled.
		if (PanningLimitsEnabled)
		{
			ViewportPosition.X = FMath::Clamp(ViewportPosition.X, ViewportSize.X / 2, MapSize.X - ViewportSize.X / 2);
			ViewportPosition.Y = FMath::Clamp(ViewportPosition.Y, ViewportSize.Y / 2, MapSize.Y - ViewportSize.Y / 2);
		}
	}
	else
	{
		// Update the initial mouse position
		MousePositionInVirtualSpace = TranslateToVirtualSpace(InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()));
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

///////////
// Logic //
///////////

/**
 * Updates Viewport Size Based on Zoom Level
 */
void UInteractiveMap::UpdateViewportSize()
{
	if (ZoomLimitsEnabled) {
		// Linearly interpolate between the full scaled map size and the minimum viewport size based on the current zoom level
		ViewportSize = FMath::Lerp(MapSize, MinViewportSize, ZoomLevel);
	}
	else
	{
		// Linearly interpolate between the full scaled map size and the minimum viewport size based on the current zoom level
		ViewportSize = FMath::Lerp(MapSize * 2, MinViewportSize, ZoomLevel);
	}
}

/**
 * Calculates New ViewPort Position Based on Current Viewport Size
 */
void UInteractiveMap::RepositionViewportIfNeeded()
{
	// Calculate the top-left & bottom-right corner of the viewport
	FVector2D ViewportTopLeft = ViewportPosition - (ViewportSize / 2);
	const FVector2D ViewportBottomRight = ViewportTopLeft + ViewportSize;

	// Adjust the top-left corner to stay within map boundaries
	ViewportTopLeft.X = FMath::Max(0.0f, ViewportTopLeft.X);
	ViewportTopLeft.Y = FMath::Max(0.0f, ViewportTopLeft.Y);

	if (ViewportBottomRight.X > MapSize.X)
	{
		ViewportTopLeft.X -= (ViewportBottomRight.X - MapSize.X);
	}
	if (ViewportBottomRight.Y > MapSize.Y)
	{
		ViewportTopLeft.Y -= (ViewportBottomRight.Y - MapSize.Y);
	}

	// Recalculate the viewport position based on the adjusted top-left corner
	ViewportPosition = ViewportTopLeft + (ViewportSize / 2);
}

/**
 * Converts Virtual Points to Widget Space
 * @param VirtualPoint The Point being Translated
 * @return Location in Widget Space
 */
FVector2D UInteractiveMap::TranslateToWidgetSpace(const FVector2D& VirtualPoint) const
{
	// Calculate the top-left corner of the viewport in virtual coordinates
	const FVector2D ViewportTopLeft = ViewportPosition - (ViewportSize / 2);

	// Calculate the relative position of the point from the viewports top-left corner
	const FVector2D RelativePosition = VirtualPoint - ViewportTopLeft;

	// Determine the scale factor based on the current viewport size
	const FVector2D ScaleFactor = WidgetSize / ViewportSize;

	// Apply the scale factor to the relative position
	return RelativePosition * ScaleFactor;
}

/**
 * Converts Cursor Position to Position on Map
 * @param ScreenPoint Point on Screen
 * @return Virtual Point
 */
FVector2D UInteractiveMap::TranslateToVirtualSpace(const FVector2D& ScreenPoint) const
{
	const FVector2D ScaleFactor = ViewportSize / WidgetSize;
	return (ScreenPoint * ScaleFactor) + (ViewportPosition - (ViewportSize / 2));
}

///////////////////////////
// Drawing Functionality //
///////////////////////////

int32 UInteractiveMap::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// Draw the Widget Drawing Space
	DrawWidgetBorder(InContext, AllottedGeometry);

	// Draw the Border Of Map
	DrawMapBorder(InContext, AllottedGeometry);

	// Center of Map
	if (ShowMapCenter)
	{
		DrawPoint(InContext, AllottedGeometry, MapSize / 2, FLinearColor::Red, FVector2D(5.0f, 5.0f));
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 1, InWidgetStyle, bParentEnabled);
}

void UInteractiveMap::DrawWidgetBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const
{
	if (!ShowWidgetBorder) { return; }

	// Define points for the border rectangle
	TArray<FVector2D> BorderPoints;
	BorderPoints.Add(FVector2D(0, 0));
	BorderPoints.Add(FVector2D(WidgetSize.X, 0));
	BorderPoints.Add(FVector2D(WidgetSize.X, WidgetSize.Y));
	BorderPoints.Add(FVector2D(0, WidgetSize.Y));
	BorderPoints.Add(FVector2D(0, 0));

	// Width of Border
	constexpr float BorderWidth = 1.1f;

	// Draw the Border
	FSlateDrawElement::MakeLines(InContext.OutDrawElements, InContext.LayerId, AllottedGeometry.ToPaintGeometry(), BorderPoints,
		ESlateDrawEffect::None, FLinearColor::White, true, BorderWidth);
}

void UInteractiveMap::DrawMapBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const
{
	TArray<FVector2D> BorderPoints;

	// Define the corners of the map in virtual space
	const FVector2D TopLeftCorner(0, 0);
	const FVector2D TopRightCorner(MapSize.X, 0);
	const FVector2D BottomLeftCorner(0, MapSize.Y);
	const FVector2D BottomRightCorner(MapSize.X, MapSize.Y);

	// Translate the corners to widget space
	BorderPoints.Add(TranslateToWidgetSpace(TopLeftCorner));
	BorderPoints.Add(TranslateToWidgetSpace(TopRightCorner));
	BorderPoints.Add(TranslateToWidgetSpace(BottomRightCorner));
	BorderPoints.Add(TranslateToWidgetSpace(BottomLeftCorner));
	BorderPoints.Add(TranslateToWidgetSpace(TopLeftCorner)); // Close the loop

	// Define the border width and color
	constexpr float BorderWidth = 2.0f;
	const FLinearColor BorderColor = FLinearColor::White;

	// Draw the border
	FSlateDrawElement::MakeLines(InContext.OutDrawElements, InContext.LayerId, AllottedGeometry.ToPaintGeometry(), BorderPoints,
								 ESlateDrawEffect::None, BorderColor, true, BorderWidth);
}

////////////////////////////
// Public Drawing Methods //
////////////////////////////

void UInteractiveMap::DrawPoint(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const FVector2D& VirtualPoint, const FLinearColor& Color, const FVector2D Size) const
{
	// Translate the virtual point to widget space
	FVector2D WidgetSpacePoint = TranslateToWidgetSpace(VirtualPoint);

	//// Adjust WidgetSpacePoint to center the box on the point
	WidgetSpacePoint -= Size * 0.5f;

	//// Create a paint geometry for the box at the widget space position
	//// Updated to use the newer API as suggested by the warning
	const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform(WidgetSpacePoint));

	//// Create a brush for the box
	FSlateBrush BoxBrush;
	BoxBrush.TintColor = FSlateColor(Color);

	// Draw the box
	FSlateDrawElement::MakeBox(InContext.OutDrawElements, InContext.LayerId, PaintGeometry, &BoxBrush, ESlateDrawEffect::None, Color);
}

void UInteractiveMap::DrawLine(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const FVector2D& VirtualStartPoint, const FVector2D& VirtualEndPoint, const FLinearColor& Color, const double Thickness) const
{
	// Translate the virtual start and end points to widget space
	const FVector2D WidgetSpaceStartPoint = TranslateToWidgetSpace(VirtualStartPoint);
	const FVector2D WidgetSpaceEndPoint = TranslateToWidgetSpace(VirtualEndPoint);

	//// Create an array of points for the line
	TArray<FVector2D> LinePoints;
	LinePoints.Add(WidgetSpaceStartPoint);
	LinePoints.Add(WidgetSpaceEndPoint);

	// Draw the line
	FSlateDrawElement::MakeLines(
		InContext.OutDrawElements,
		 InContext.LayerId,
		AllottedGeometry.ToPaintGeometry(),
		LinePoints,
		ESlateDrawEffect::None,
		Color,
		true,
		Thickness
	);
}

void UInteractiveMap::DrawLines(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const TArray<FVector2D>& Points, const FLinearColor& Color, const double Thickness) const
{
	// Create an array of points in widget space
	TArray<FVector2D> WidgetSpacePoints;
	for (const FVector2D& Point : Points)
	{
		FVector2D WidgetSpacePoint = TranslateToWidgetSpace(Point);
		WidgetSpacePoints.Add(WidgetSpacePoint);
	}

	// Draw the lines using Slate's drawing system
	FSlateDrawElement::MakeLines(
		InContext.OutDrawElements,
		InContext.LayerId,
		AllottedGeometry.ToPaintGeometry(),
		WidgetSpacePoints,
		ESlateDrawEffect::None,
		Color,
		true,
		Thickness
	);
}

void UInteractiveMap::DrawPolygon(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const TArray<FVector2D>& Vertices, const TArray<SlateIndex>& Indices, const FColor& Color) const
{
	// Array for FSlateVertex
	TArray<FSlateVertex> SlateVertices;

	// Default texture coordinates
	FVector2f DefaultTexCoord(0.0f, 0.0f);

	// Convert each vertex to FSlateVertex
	for (const FVector2D& Vertex : Vertices)
	{
		// Convert to widget space using TranslateToWidgetSpace
		const FVector2f WidgetSpaceVertex = FVector2f(TranslateToWidgetSpace(Vertex));

		// Make FSlateVertex and add to the array
		SlateVertices.Add(FSlateVertex::Make(AllottedGeometry.GetAccumulatedRenderTransform(), WidgetSpaceVertex, DefaultTexCoord, Color));
	}

	// Draw the polygon using the vertices and indices
	FSlateDrawElement::MakeCustomVerts(
		InContext.OutDrawElements,
		InContext.LayerId,
		FSlateResourceHandle(), // Default handle
		SlateVertices,
		Indices,
		nullptr, // No additional instance data
		0, // Instance offset
		0, // Number of instances (0 for non-instanced rendering)
		ESlateDrawEffect::None // Drawing effects
	);
}

/////////////////////
// Setters/Getters //
/////////////////////

/**
 * Sets Size of Map and Calculates Aspect Ratio
 * @param Size New Size of Map
 */
void UInteractiveMap::SetMapSize(const FVector2D Size)
{
	MapSize = Size;
	UpdateViewportSize();
}

//////////////////////////////////////
//          TESTING Class           //
//////////////////////////////////////
void UMapViewerTestHelper::SetData(const FVector2D& InWidgetSize, const FVector2D& InMapSize)
{
	WidgetSize = InWidgetSize;
	MapSize = InMapSize;
	ViewportPosition = MapSize / 2;
	UpdateViewportSize();
}

FPointerEvent UMapViewerTestHelper::MakeFakeMoveMouseEvent(const FVector2D& CursorPosition, const FVector2D& LastCursorPosition, const FVector2D& CursorDelta)
{
	return FPointerEvent(0, CursorPosition, LastCursorPosition, CursorDelta, TSet<FKey>(), FModifierKeysState());
}

FPointerEvent UMapViewerTestHelper::MakeFakeScrollMouseEvent(const float WheelDelta, const FVector2D& CursorPosition)
{
	return FPointerEvent(
	0,
	CursorPosition,
	CursorPosition,
	TSet<FKey>(),
	FKey(),
	WheelDelta,
	FModifierKeysState()
	);
}


