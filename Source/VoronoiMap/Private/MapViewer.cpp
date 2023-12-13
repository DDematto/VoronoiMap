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
	UE_LOG(LogTemp, Warning, TEXT("UMapViewer constructor called"));

	GeneratePoints();
}

void UMapViewer::NativeConstruct()
{
	Super::NativeConstruct();

	// Ensure that the widget is focusable
	SetIsFocusable(true);

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;

		// Explicitly set focus to this widget
		SetFocus();

		SetVisibility(ESlateVisibility::Visible);
		bIsFocusable = true;
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
	// Store the old zoom level
	const float OldZoomLevel = CurrentZoomLevel;

	// Adjust CurrentZoomLevel based on wheel delta
	CurrentZoomLevel -= InMouseEvent.GetWheelDelta() * ZoomSensitivity;

	// Clamp CurrentZoomLevel within 0.0 (fully zoomed out) and 1.0 (fully zoomed in)
	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel, 0.1f, 1.0f);

	// Calculate the cursor position relative to the map's center
	const FVector2D CursorPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D MapCenter = InGeometry.GetLocalSize() / 2.0f;
	const FVector2D RelativeCursorPosition = CursorPosition - MapCenter;

	// Adjust MapPosition based on zoom level change
	if (OldZoomLevel != 0)
	{
		MapPosition -= RelativeCursorPosition * (1.0f / OldZoomLevel - 1.0f / CurrentZoomLevel);
	}

	InvalidateLayoutAndVolatility();

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
		const FVector2D MouseDelta = InMouseEvent.GetCursorDelta();
		MapPosition += MouseDelta;

		const FVector2D CurrentViewportSize = InGeometry.GetLocalSize();
		const FVector2D ScaledMapSize = FVector2D(Width, Height) * (1 / CurrentZoomLevel);

		// Calculate the maximum panning boundaries
		const float MaxX = FMath::Max(0.0f, (ScaledMapSize.X - CurrentViewportSize.X) / 2.0f);
		const float MaxY = FMath::Max(0.0f, (ScaledMapSize.Y - CurrentViewportSize.Y) / 2.0f);

		// Clamp the MapPosition within these boundaries
		MapPosition.X = FMath::Clamp(MapPosition.X, -MaxX, MaxX);
		MapPosition.Y = FMath::Clamp(MapPosition.Y, -MaxY, MaxY);

		return FReply::Handled();
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

///////////////////////////
// Drawing Functionality //
///////////////////////////

int32 UMapViewer::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// Draw the borders
	DrawBorders(InContext, AllottedGeometry);

	// Draw Data to Screen
	DrawPoints(InContext);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UMapViewer::DrawBorders(FPaintContext& InContext, const FGeometry& AllottedGeometry)
{
	constexpr float BorderWidth = 1.1f;
	const FVector2D Size = AllottedGeometry.GetLocalSize();

	TArray<FVector2D> BorderPoints;

	// Define points for the border rectangle
	BorderPoints.Add(FVector2D(0, 0));
	BorderPoints.Add(FVector2D(Size.X, 0));
	BorderPoints.Add(FVector2D(Size.X, Size.Y));
	BorderPoints.Add(FVector2D(0, Size.Y));
	BorderPoints.Add(FVector2D(0, 0));

	FSlateBrush BorderBrush;
	BorderBrush.TintColor = FLinearColor::White;

	FSlateDrawElement::MakeLines(InContext.OutDrawElements, InContext.LayerId, AllottedGeometry.ToPaintGeometry(),
		BorderPoints, ESlateDrawEffect::None, FLinearColor::White, true, BorderWidth);
}

//////////////////////////////////
// Transform Elements Functions //
//////////////////////////////////

/**
* Transform Points to Be Drawn on Map View
* @param Points Points to be Transformed
* @return FVector Array
*/
TArray<FVector> UMapViewer::TransformPolygons(const TArray<FVector>& Points) const
{
	TArray<FVector> TransformedPoints;

	return TArray<FVector>();
}

/**
* Transform a Single Point to Be Drawn on Map View
* @param Point Point to be Transformed
* @return Transformed Point or a default FVector (e.g., FVector::ZeroVector) if out of bounds
*/
FVector UMapViewer::TransformPoint(const FVector& Point) const
{
	const FVector2D CurrentViewportSize = GetCachedGeometry().GetLocalSize();
	const FVector2D ViewportCenter = CurrentViewportSize / 2.0f;

	const FVector2D ScaledPosition = (FVector2D(Point.X, Point.Y) - ViewportCenter) * (1.0f / CurrentZoomLevel);
	const FVector2D AdjustedPosition = ScaledPosition + MapPosition;
	const FVector2D ZoomedPosition = AdjustedPosition + ViewportCenter;

	// Clipping logic
	if (ZoomedPosition.X >= 0 && ZoomedPosition.X <= CurrentViewportSize.X &&
		ZoomedPosition.Y >= 0 && ZoomedPosition.Y <= CurrentViewportSize.Y)
	{
		return FVector(ZoomedPosition.X, ZoomedPosition.Y, 0);
	}

	return FVector::ZeroVector;
}


/////////////////////
// DEMO/DEBUG DATA //
/////////////////////



/**
 * Generates Points for Testing
 * This Data Cannot Be Manipulated by Any Draw Method
 */
void UMapViewer::GeneratePoints()
{
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();

	for (int32 i = 0; i < 500; ++i)
	{
		FColoredPoint Point;
		Point.Position.X = RandomStream.FRandRange(0, Width);
		Point.Position.Y = RandomStream.FRandRange(0, Height);
		Point.Color = FLinearColor::MakeRandomColor();
		DemoPoints.Add(Point);
	}
}

/**
 * Draws Points, Cannot Manipulate Actual Point FVector
 * @param InContext FPaintContext
 */
void UMapViewer::DrawPoints(const FPaintContext& InContext) const
{
	const FVector2D BoxSize(3.0f, 3.0f);

	// Use a basic brush
	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");

	for (const FColoredPoint& ColoredPoint : DemoPoints)
	{
		FVector TransformedPoint = TransformPoint(ColoredPoint.Position);
		if (TransformedPoint != FVector::ZeroVector)
		{
			FSlateDrawElement::MakeBox(
				InContext.OutDrawElements,
				InContext.LayerId,
				InContext.AllottedGeometry.ToPaintGeometry(BoxSize, FSlateLayoutTransform(FVector2D(TransformedPoint.X, TransformedPoint.Y))),
				GenericBrush,
				ESlateDrawEffect::None,
				ColoredPoint.Color
			);
		}
	}
}




