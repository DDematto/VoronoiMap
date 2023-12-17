/**
* @author Devin DeMatto
* @file MapViewer.h
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapViewer.generated.h"

/**
 * Base Class for Interactive Map
 */
UCLASS()
class VORONOIMAP_API UMapViewer : public UUserWidget
{
	GENERATED_BODY()
public:
	/// Constructor   
	explicit UMapViewer(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	friend class UMapViewerTestHelper; // Testing Class

	// Size of Map (Points are all within this Size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Settings")
	FVector2D MapSize = FVector2D(500, 500);

	// Set in UI Editor Size of Actual UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Settings")
	FVector2D WidgetSize = FVector2D(0, 0);

	// Scale to Make MapSize = WidgetSize
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ScalingFactor = FVector2D(1, 1);

	// Center of Our Viewport (Can be Controlled by Zoom & Panning)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ViewportPosition = FVector2D(250.0f, 250.0f);

	// Size of our Viewport Window (Max size should be MapSize * ScalingFactor)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ViewportSize = FVector2D(100, 100);

	// Normalized Value Controlling Viewport Size (0 - Zoomed Out Full Map) (1 - Zoomed in)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	float CurrentZoomLevel = 0.0f;

	// Are we currently moving the viewport
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	bool IsPanning = false;

	// Should we Clip Points outside of Viewport?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Options")
	bool BorderClippingEnabled = true;

	// Can we Pan outside of Scaled Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Options")
	bool PanningLimitsEnabled = true;

	// Can we Zoom outside of Scaled Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Options")
	bool ZoomLimitsEnabled = true;

private:
	///////////
	// Logic //
	///////////

	FVector2D CalculateScalingFactor();

	void RecalculateViewportPosition(const FVector2D& MovementDelta);

	void RecalculateViewportPositionAfterZoom();

	///////////////////////////
	// Drawing Functionality // 
	///////////////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	void DrawWidgetBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const;
	void DrawMapBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const;
	void DrawViewportWindow(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const;
	static void DrawPoint(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const FVector2D& Point, const FLinearColor& Color);

	////////////
	// Events //
	////////////

	/// Zooming in On Map
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// Start Panning
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// Stop Panning
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// Panning Map
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	bool IsPointWithinViewport(const FVector2D& Point) const;

	/////////////////////
	// Setters/Getters //
	/////////////////////

	void SetMapSize(FVector2D Size);
};



/**
* Stub Class for MapViewer Testing
*/
UCLASS()
class UMapViewerTestHelper : public UMapViewer
{
	GENERATED_BODY()

public:
	/// Constructor   
	explicit UMapViewerTestHelper(const FObjectInitializer& ObjectInitializer) : UMapViewer(ObjectInitializer) {};

	void FakeConstruct(const FVector2D& InWidgetSize, const FVector2D& InMapSize);

	/// Map Size
	FVector2D GetMapSize() const
	{
		return MapSize;
	}

	void SetMapSize(const FVector2D& Size)
	{
		UMapViewer::SetMapSize(Size);
	}

	// Widget Size
	FVector2D GetWidgetSize() const
	{
		return WidgetSize;
	}

	void SetWidgetSize(const FVector2D& Size)
	{
		WidgetSize = Size;
	}

	// Map Panning
	bool GetIsPanning() const
	{
		return IsPanning;
	}

	void SetIsPanning(const bool Value)
	{
		IsPanning = Value;
	}

	// Zoom Level
	float GetCurrentZoomLevel() const
	{
		return CurrentZoomLevel;
	}

	void SetCurrentZoomLevel(const float Value)
	{
		CurrentZoomLevel = Value;
	}

	// Viewport Position
	FVector2D GetViewportPosition() const
	{
		return ViewportPosition;
	}

	void SetViewportPosition(const FVector2D& Position)
	{
		ViewportPosition = Position;
	}

	// Viewport Size
	FVector2D GetViewportSize() const
	{
		return ViewportSize;
	}

	void SetViewportWidth(const FVector2D Size)
	{
		ViewportSize = Size;
	}

	// Exposing logic methods

	FVector2D CalculateScalingFactorExposed()
	{
		return CalculateScalingFactor();
	}

	bool IsPointWithinViewportExposed(const FVector2D& Point) const
	{
		return IsPointWithinViewport(Point);

	}

	void RecalculateViewportPositionExposed(const FVector2D& MovementDelta)
	{
		RecalculateViewportPosition(MovementDelta);
	}

	// Exposing Event Methods
	FReply NativeOnMouseWheelExposed(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return UMapViewer::NativeOnMouseWheel(InGeometry, InMouseEvent);
	}

	FReply NativeOnMouseMoveExposed(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return UMapViewer::NativeOnMouseMove(InGeometry, InMouseEvent);
	}

	// TESTING METHODS 
	static FPointerEvent MakeFakeMoveMouseEvent(const FVector2D& CursorPosition, const FVector2D& LastCursorPosition, const FVector2D& CursorDelta);

	static FPointerEvent MakeFakeScrollMouseEvent(const float WheelDelta, const FVector2D& CursorPosition);
};