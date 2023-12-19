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

	// Current Position on Map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ViewportPosition = FVector2D(250.0f, 250.0f);

	// How large of an area we are viewing on the map
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	FVector2D ViewportSize = FVector2D(0.0f, 0.0f);

	// Normalized Value Controlling Viewport Size (0 - Zoomed Out Full Map) (1 - Zoomed in)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MapViewer Status")
	float ZoomLevel = 0.0f;

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

	void UpdateViewportSize();
	void RepositionViewportIfNeeded();

	FVector2D TranslateToWidgetSpace(const FVector2D& VirtualPoint) const;

	///////////////////////////
	// Drawing Functionality // 
	///////////////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
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
		IsPanning = true;
		FReply ReturnData = UMapViewer::NativeOnMouseMove(InGeometry, InMouseEvent);
		IsPanning = false;
		return ReturnData;
	}

	// TESTING METHODS 
	static FPointerEvent MakeFakeMoveMouseEvent(const FVector2D& CursorPosition, const FVector2D& LastCursorPosition, const FVector2D& CursorDelta);

	static FPointerEvent MakeFakeScrollMouseEvent(const float WheelDelta, const FVector2D& CursorPosition);
};