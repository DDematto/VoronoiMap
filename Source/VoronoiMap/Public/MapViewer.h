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

protected:
	friend class UMapViewerTestHelper; // Testing Class

	// Size of Map (Points are all within this Size)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D MapSize = FVector2D(500, 500);

	// Current Position on Map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ViewportPosition = FVector2D(250.0f, 250.0f);

	// How large of an area we are viewing on the map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ViewportSize = FVector2D(0.0f, 0.0f);

	// Can we Pan outside of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Graphics")
	bool PanningLimitsEnabled = true;

	// Show Widget Border?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Graphics")
	bool ShowWidgetBorder = false;

	// Show Map Center?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapViewer Graphics")
	bool ShowMapCenter = true;

private:
	// Set in UI Editor Size of Actual UI
	FVector2D WidgetSize = FVector2D(0, 0);

	// Normalized Value Controlling Viewport Size (0 - Zoomed Out Full Map) (1 - Zoomed in)
	float ZoomLevel = 0.0f;

	// Mouse Position in Virtual Space
	FVector2D MousePositionInVirtualSpace;

	// Are we currently moving the viewport
	bool bIsPanning = false;

	///////////
	// Logic //
	///////////

	void UpdateViewportSize();
	void RepositionViewportIfNeeded();

	FVector2D TranslateToWidgetSpace(const FVector2D& VirtualPoint) const;
	FVector2D TranslateToVirtualSpace(const FVector2D& ScreenPoint) const;

	///////////////////////////
	// Drawing Functionality // 
	///////////////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	void DrawWidgetBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const;
	void DrawMapBorder(const FPaintContext& InContext, const FGeometry& AllottedGeometry) const;
	void DrawPoint(const FPaintContext& InContext, const FGeometry& AllottedGeometry, const FVector2D& VirtualPoint,
				   const FLinearColor& Color) const;

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
	/// Constructor Sets Up Widget and Map Size
	explicit UMapViewerTestHelper(const FObjectInitializer& ObjectInitializer) : UMapViewer(ObjectInitializer) {};
	void SetData(const FVector2D& InWidgetSize, const FVector2D& InMapSize);

	///////////////////////
	// Exposing Variables//
	///////////////////////

	// Overloaded Setter/Getter for WidgetSize
	void SetWidgetSize(const FVector2D& NewWidgetSize) { WidgetSize = NewWidgetSize; }
	const FVector2D& GetWidgetSize() const { return WidgetSize; }

	// Overloaded Setter/Getter for ZoomLevel
	void SetZoomLevel(const float NewZoomLevel) { ZoomLevel = NewZoomLevel; }
	float GetZoomLevel() const { return ZoomLevel; }

	// Overloaded Setter/Getter for MousePositionInVirtualSpace
	void SetMousePositionInVirtualSpace(const FVector2D& NewMousePositionInVirtualSpace) { MousePositionInVirtualSpace = NewMousePositionInVirtualSpace; }
	const FVector2D& GetMousePositionInVirtualSpace() const { return MousePositionInVirtualSpace; }

	// Overloaded Setter/Getter for bIsPanning
	void SetIsPanning(const bool NewIsPanning) { bIsPanning = NewIsPanning; }
	bool GetIsPanning() const { return bIsPanning; }

	// Overloaded Setter/Getter for MapSize
	void SetMapSize(const FVector2D& NewMapSize) { MapSize = NewMapSize; }
	const FVector2D& GetMapSize() const { return MapSize; }

	// Overloaded Setter/Getter for ViewportPosition
	void SetViewportPosition(const FVector2D& NewViewportPosition) { ViewportPosition = NewViewportPosition; }
	const FVector2D& GetViewportPosition() const { return ViewportPosition; }

	// Overloaded Setter/Getter for ViewportSize
	void SetViewportSize(const FVector2D& NewViewportSize) { ViewportSize = NewViewportSize; }
	const FVector2D& GetViewportSize() const { return ViewportSize; }

	// Overloaded Setter/Getter for PanningLimitsEnabled
	void SetPanningLimitsEnabled(const bool NewPanningLimitsEnabled) { PanningLimitsEnabled = NewPanningLimitsEnabled; }
	bool GetPanningLimitsEnabled() const { return PanningLimitsEnabled; }


	// Exposing logic methods
	void UpdateViewportSizeExposed()
	{
		UpdateViewportSize();
	}

	// Exposing Event Methods
	FReply NativeOnMouseWheelExposed(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return UMapViewer::NativeOnMouseWheel(InGeometry, InMouseEvent);
	}

	FReply NativeOnMouseMoveExposed(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		bIsPanning = true;
		FReply ReturnData = UMapViewer::NativeOnMouseMove(InGeometry, InMouseEvent);
		bIsPanning = false;
		return ReturnData;
	}

	// TESTING METHODS 
	static FPointerEvent MakeFakeMoveMouseEvent(const FVector2D& CursorPosition, const FVector2D& LastCursorPosition, const FVector2D& CursorDelta);

	static FPointerEvent MakeFakeScrollMouseEvent(const float WheelDelta, const FVector2D& CursorPosition);
};