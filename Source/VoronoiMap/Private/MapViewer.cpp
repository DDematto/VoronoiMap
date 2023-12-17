// @author Devin DeMatto \n I am just trying out some silly stuff :P

#include "MapViewer.h"

/////////////////
// Constructor //
/////////////////

/**
 * Default Constructor, Sets up Control for Key bind Manipulation
 * @param ObjectInitializer
 */
UMapViewer::UMapViewer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMapViewer::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}

void UMapViewer::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D LastWidgetSize = MyGeometry.GetAbsoluteSize();

	// Check if the size has changed since the last tick
	if (WidgetSize != LastWidgetSize)
	{
		WidgetSize = LastWidgetSize;
		ScalingFactor = CalculateScalingFactor();
		ViewportSize = MapSize * ScalingFactor;
		ViewportPosition = ViewportSize / 2.0;
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
FReply UMapViewer::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Existing zoom level adjustment logic
	const float ZoomChange = InMouseEvent.GetWheelDelta() * 0.10f;
	CurrentZoomLevel += ZoomChange;
	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, 0.0f, 1.0f);

	// Calculate the new viewport size based on zoom level
	const FVector2D FullyZoomedOutSize = MapSize * ScalingFactor;
	const FVector2D MaxZoomSize = FVector2D(50.0f, 50.0f);
	const FVector2D OldViewportSize = ViewportSize;
	ViewportSize = FMath::Lerp(FullyZoomedOutSize, MaxZoomSize, CurrentZoomLevel);

	// Get the cursor position relative to the widget
	const FVector2D CursorPosition = InMouseEvent.GetScreenSpacePosition() - InGeometry.AbsolutePosition;

	// Calculate the zoom focal point on the map
	const FVector2D ZoomFocalPoint = ViewportPosition + (CursorPosition - OldViewportSize / 2.0f);

	// Adjust the viewport position based on the zoom focal point
	ViewportPosition = ZoomFocalPoint - (ViewportSize / 2.0f);

	// Ensure the viewport is still within bounds
	//RecalculateViewportPositionAfterZoom();

	return FReply::Handled();
}


/**
 * Sets up Panning Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UMapViewer::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	IsPanning = true;
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

/**
 * Sets up Panning Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UMapViewer::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	IsPanning = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

/**
 * Sets up Panning Functionality
 * @param InGeometry FGeometry
 * @param InMouseEvent FPointerEvent
 * @return FReply
 */
FReply UMapViewer::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (IsPanning) {
		const FVector2D MouseDelta = InMouseEvent.GetCursorDelta();
		RecalculateViewportPosition(MouseDelta);
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

///////////
// Logic //
///////////

FVector2D UMapViewer::CalculateScalingFactor()
{
	// Calculate scaling factors for both width and height
	const float ScaleFactorWidth = WidgetSize.X / MapSize.X;
	const float ScaleFactorHeight = WidgetSize.Y / MapSize.Y;

	ScalingFactor = FVector2D(ScaleFactorWidth, ScaleFactorHeight);

	// Apply the final scaling factor to both dimensions
	return ScalingFactor;
}

void UMapViewer::RecalculateViewportPosition(const FVector2D& MovementDelta)
{
	FVector2D PotentialNewPosition = ViewportPosition + MovementDelta;

	if (PanningLimitsEnabled)
	{
		const FVector2D HalfViewportSize = ViewportSize / 2.0f;
		const FVector2D ScaledMapSize = MapSize * ScalingFactor;

		// Clamp the potential new position to the scaled map bounds
		PotentialNewPosition.X = FMath::Clamp(PotentialNewPosition.X, HalfViewportSize.X, ScaledMapSize.X - HalfViewportSize.X);
		PotentialNewPosition.Y = FMath::Clamp(PotentialNewPosition.Y, HalfViewportSize.Y, ScaledMapSize.Y - HalfViewportSize.Y);
	}

	ViewportPosition = PotentialNewPosition;
}

void UMapViewer::RecalculateViewportPositionAfterZoom()
{
	if (ZoomLimitsEnabled)
	{
		const FVector2D HalfViewportSize = ViewportSize / 2.0f;
		const FVector2D ScaledMapSize = MapSize * ScalingFactor;

		// Clamp the viewport position to the scaled map bounds
		ViewportPosition.X = FMath::Clamp(ViewportPosition.X, HalfViewportSize.X, ScaledMapSize.X - HalfViewportSize.X);
		ViewportPosition.Y = FMath::Clamp(ViewportPosition.Y, HalfViewportSize.Y, ScaledMapSize.Y - HalfViewportSize.Y);
	}
}

bool UMapViewer::IsPointWithinViewport(const FVector2D& Point) const
{
	// Calculate the bounds of the viewport
	const FVector2D ViewportTopLeft = FVector2D(ViewportPosition.X - ViewportSize.X / 2.0f, ViewportPosition.Y - ViewportSize.Y / 2.0f);
	const FVector2D ViewportBottomRight = FVector2D(ViewportPosition.X + ViewportSize.X / 2.0f, ViewportPosition.Y + ViewportSize.Y / 2.0f);

	// Check if the point is within the viewport bounds
	return Point.X >= ViewportTopLeft.X && Point.X <= ViewportBottomRight.X && Point.Y >= ViewportTopLeft.Y && Point.Y <= ViewportBottomRight.Y;
}


///////////////////////////
// Drawing Functionality //
///////////////////////////

int32 UMapViewer::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// Draw the borders
	DrawWidgetBorder(InContext, AllottedGeometry);
	DrawMapBorder(InContext, AllottedGeometry);
	DrawViewportWindow(InContext, AllottedGeometry);

	DrawPoint(InContext, AllottedGeometry, ViewportPosition, FLinearColor::Red);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UMapViewer::DrawWidgetBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const
{
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
		ESlateDrawEffect::None, FLinearColor::Yellow, true, BorderWidth);
}


void UMapViewer::DrawMapBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const
{
	const FVector2D ScaledMap = MapSize * ScalingFactor;

	// Define points for the border rectangle
	TArray<FVector2D> BorderPoints;
	BorderPoints.Add(FVector2D(0, 0));
	BorderPoints.Add(FVector2D(ScaledMap.X, 0));
	BorderPoints.Add(ScaledMap);
	BorderPoints.Add(FVector2D(0, ScaledMap.Y));
	BorderPoints.Add(FVector2D(0, 0));

	// Width of Border
	constexpr float BorderWidth = 1.1f;

	// Draw the Border
	FSlateDrawElement::MakeLines(InContext.OutDrawElements, InContext.LayerId, AllottedGeometry.ToPaintGeometry(), BorderPoints,
		ESlateDrawEffect::None, FLinearColor::Red, true, BorderWidth);
}

void UMapViewer::DrawViewportWindow(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const
{
	// Calculate the bounds of the viewport
	const FVector2D ViewportTopLeft = FVector2D(ViewportPosition.X - ViewportSize.X / 2.0f, ViewportPosition.Y - ViewportSize.Y / 2.0f);
	const FVector2D ViewportBottomRight = FVector2D(ViewportPosition.X + ViewportSize.X / 2.0f, ViewportPosition.Y + ViewportSize.Y / 2.0f);

	// Define points for the border rectangle
	TArray<FVector2D> BorderPoints;
	BorderPoints.Add(ViewportTopLeft);                                      // Top Left
	BorderPoints.Add(FVector2D(ViewportBottomRight.X, ViewportTopLeft.Y));  // Top Right
	BorderPoints.Add(ViewportBottomRight);                                  // Bottom Right
	BorderPoints.Add(FVector2D(ViewportTopLeft.X, ViewportBottomRight.Y));  // Bottom Left
	BorderPoints.Add(ViewportTopLeft);                                      // Back to Top Left to close the rectangle

	// Width of Border
	constexpr float BorderWidth = 1.1f;

	// Draw the Border
	FSlateDrawElement::MakeLines(InContext.OutDrawElements, InContext.LayerId, AllottedGeometry.ToPaintGeometry(), BorderPoints,
		ESlateDrawEffect::None, FLinearColor::Blue, true, BorderWidth);
}

void UMapViewer::DrawPoint(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const FVector2D& Point, const FLinearColor& Color)
{
	// Size of the box (point)
	const FVector2D BoxSize = FVector2D(5.0f, 5.0f);

	// Create a paint geometry for the box
	const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry(Point, BoxSize);

	// Create a brush for the box
	FSlateBrush BoxBrush;
	BoxBrush.TintColor = FSlateColor(Color);

	// Draw the box
	FSlateDrawElement::MakeBox(InContext.OutDrawElements, InContext.LayerId,
							   PaintGeometry, &BoxBrush,
							   ESlateDrawEffect::None, Color);
}




/////////////////////
// Setters/Getters //
/////////////////////

/**
 * Sets Size of Map and Calculates Aspect Ratio
 * @param Size New Size of Map
 */
void UMapViewer::SetMapSize(const FVector2D Size)
{
	MapSize = Size;
}

//////////////////////////////////////
//          TESTING Class           //
//////////////////////////////////////

void UMapViewerTestHelper::FakeConstruct(const FVector2D& InWidgetSize, const FVector2D& InMapSize)
{
	WidgetSize = InWidgetSize;
	MapSize = InMapSize;

	// Calculate and log the scaling factor
	ScalingFactor = CalculateScalingFactorExposed();

	// Set the viewport size based on the scaling factor and map size
	ViewportSize = MapSize * ScalingFactor;

	// Calculate the center of the scaled map
	ViewportPosition = FVector2D(MapSize.X * ScalingFactor.X / 2.0f, MapSize.Y * ScalingFactor.Y / 2.0f);
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
