/**
 * Functionality For MapViewer and Test Class
 * @author Devin DeMatto
 * @file MapViewer.cpp
 */

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
FReply UMapViewer::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	constexpr float ZoomDelta = 0.1f;
	ZoomLevel = FMath::Clamp(ZoomLevel + (InMouseEvent.GetWheelDelta() * ZoomDelta), 0.0f, 1.0f);

	// Update the viewport size after changing the zoom level
	UpdateViewportSize();

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
	if (IsPanning)
	{
		// Calculate a zoom-dependent factor
		const float ZoomFactor = FMath::Lerp(1.0f, (MapSize / ViewportSize).GetMin(), ZoomLevel);

		// Adjust the cursor delta based on the zoom factor
		const FVector2D VirtualDelta = -InMouseEvent.GetCursorDelta() / ZoomFactor;

		// Calculate the new viewport position
		FVector2D NewViewportPosition = ViewportPosition + VirtualDelta;

		// Calculate the half size of the viewport
		const FVector2D HalfViewportSize = ViewportSize / 2;

		// Clamp the new viewport position to the map boundaries
		NewViewportPosition.X = FMath::Clamp(NewViewportPosition.X, HalfViewportSize.X, MapSize.X - HalfViewportSize.X);
		NewViewportPosition.Y = FMath::Clamp(NewViewportPosition.Y, HalfViewportSize.Y, MapSize.Y - HalfViewportSize.Y);

		// Update the viewport position
		ViewportPosition = NewViewportPosition;
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

///////////
// Logic //
///////////

/**
 * Updates Viewport Size Based on Zoom Level
 */
void UMapViewer::UpdateViewportSize()
{
	// Define the minimum size of the viewport when fully zoomed in
	const FVector2D MinViewportSize = FVector2D(50, 50);

	// Linearly interpolate between the full scaled map size and the minimum viewport size based on the current zoom level
	ViewportSize = FMath::Lerp(MapSize, MinViewportSize, ZoomLevel);
}

/**
 * Calculates New ViewPort Position Based on Current Viewport Size
 */
void UMapViewer::RepositionViewportIfNeeded()
{
	// Calculate the top-left corner of the viewport
	FVector2D ViewportTopLeft = ViewportPosition - (ViewportSize / 2);

	// Adjust the top-left corner to stay within map boundaries
	ViewportTopLeft.X = FMath::Max(0.0f, ViewportTopLeft.X);
	ViewportTopLeft.Y = FMath::Max(0.0f, ViewportTopLeft.Y);

	// Adjust for the bottom-right corner
	const FVector2D BottomRightCorner = ViewportTopLeft + ViewportSize;
	if (BottomRightCorner.X > MapSize.X)
	{
		ViewportTopLeft.X -= (BottomRightCorner.X - MapSize.X);
	}
	if (BottomRightCorner.Y > MapSize.Y)
	{
		ViewportTopLeft.Y -= (BottomRightCorner.Y - MapSize.Y);
	}

	// Recalculate the viewport position based on the adjusted top-left corner
	ViewportPosition = ViewportTopLeft + (ViewportSize / 2);
}

/**
 * Converts Virtual Points to Widget Space
 * @param VirtualPoint The Point being Translated
 * @return Location in Widget Space
 */
FVector2D UMapViewer::TranslateToWidgetSpace(const FVector2D& VirtualPoint) const
{
	// Calculate the top-left corner of the viewport in virtual coordinates
	const FVector2D ViewportTopLeft = ViewportPosition - (ViewportSize / 2);

	// Calculate the relative position of the point from the viewport's top-left corner
	const FVector2D RelativePosition = VirtualPoint - ViewportTopLeft;

	// Determine the scale factor based on the current viewport size
	const FVector2D ScaleFactor = WidgetSize / ViewportSize;

	// Apply the scale factor to the relative position
	return RelativePosition * ScaleFactor;
}

///////////////////////////
// Drawing Functionality //
///////////////////////////

int32 UMapViewer::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// Draw the Widget Drawing Space
	DrawWidgetBorder(InContext, AllottedGeometry);

	// Draw the Border (Should move as ViewportPosition Changes)
	DrawMapBorder(InContext, AllottedGeometry);

	// Center of Viewport 
	DrawPoint(InContext, AllottedGeometry, ViewportPosition, FLinearColor::Red);

	// Center of Map
	DrawPoint(InContext, AllottedGeometry, MapSize / 2, FLinearColor::Yellow);

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
		ESlateDrawEffect::None, FLinearColor::White, true, BorderWidth);
}

void UMapViewer::DrawMapBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const
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
	constexpr float BorderWidth = 1.1f;
	const FLinearColor BorderColor = FLinearColor::Red;

	// Draw the border
	FSlateDrawElement::MakeLines(InContext.OutDrawElements, InContext.LayerId, AllottedGeometry.ToPaintGeometry(), BorderPoints,
								 ESlateDrawEffect::None, BorderColor, true, BorderWidth);
}

void UMapViewer::DrawPoint(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const FVector2D& VirtualPoint, const FLinearColor& Color) const
{
	// Translate the virtual point to widget space
	const FVector2D WidgetSpacePoint = TranslateToWidgetSpace(VirtualPoint);

	// Size of the box (point)
	const FVector2D BoxSize = FVector2D(5.0f, 5.0f);

	// Create a paint geometry for the box at the widget space position
	const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry(WidgetSpacePoint, BoxSize);

	// Create a brush for the box
	FSlateBrush BoxBrush;
	BoxBrush.TintColor = FSlateColor(Color);

	// Draw the box
	FSlateDrawElement::MakeBox(InContext.OutDrawElements, InContext.LayerId, PaintGeometry, &BoxBrush, ESlateDrawEffect::None, Color);
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


