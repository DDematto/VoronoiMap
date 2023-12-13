/**
* @author Devin DeMatto
* @file MapViewer.h
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapViewer.generated.h"

struct FColoredPoint
{
	FVector Position;
	FLinearColor Color;
};

/**
 * Base Class for Map Viewer Handles Size of Map, Displaying Map Borders, & Zooming/Panning Functionality
 */
UCLASS()
class VORONOIMAP_API UMapViewer : public UUserWidget
{
	GENERATED_BODY()
public:
	/// Constructor   
	explicit UMapViewer(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	TArray<FVector> TransformPolygons(const TArray<FVector>& Points) const;
	FVector TransformPoint(const FVector& Point) const;

	////////////////////
	// Map Boundaries //
	//////////////////// 

	int32 Width = 1200;
	int32 Height = 800;

	////////////////////
	// Zoom Variables //
	//////////////////// 

	float CurrentZoomLevel = 1.0f;
	float ZoomSensitivity = 0.05f;


	///////////////////////
	// Panning Variables //
	///////////////////////

	bool IsPanning = false;
	FVector2D MapPosition = FVector2D(0.0f, 0.0f);

	///////////////////////////
	// Drawing Functionality //
	///////////////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	static void DrawBorders(FPaintContext& InContext, const FGeometry& AllottedGeometry);

	////////////////////////
	// Events to Override //
	////////////////////////

	/// Zooming in On Map
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// Starting Movement on Map
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// Stopping Movement on Map
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// Movement On Map
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//////////////////////////////
	// Additional Functionality //
	//////////////////////////////

	UFUNCTION()
	virtual float GetAspectRatio() const
	{
		return 10.0;
	}

	/////////////////////
	// Setters/Getters //
	/////////////////////

	FVector2D GetMapSize() const { return FVector2D(Width, Height); }

	void SetHeight(const int32 height) { Height = height; }

	void SetWidth(const int32 width) { Width = width; }


	/////////////////////////////////////////////
	// TESTING LOGIC FOR VISUAL REPRESENTATION //
	/////////////////////////////////////////////

	/// Points Generated at Runtime, Cannot Be Manipulated
	TArray<FColoredPoint> DemoPoints;

	/// Generate Points
	void GeneratePoints();

	/// Draws Points Cannot Manipulate Points Variable
	void DrawPoints(const FPaintContext& InContext) const;
};


/**
* Stub Class for MapViewer Testing
*/
UCLASS()
class UMapViewerTestHelper : public UMapViewer
{
	GENERATED_BODY()

private:
	/// Test Viewport Size
	FVector2D TestViewportSize = FVector2D(0.0f, 0.0f);

public:
	/// Constructor
	UMapViewerTestHelper(const FObjectInitializer& ObjectInitializer) : UMapViewer(ObjectInitializer) {
		UE_LOG(LogTemp, Warning, TEXT("STUB CLASS constructor called"));
	}

	/////////////////////
	// Setters/Getters //
	/////////////////////

	void SetTestViewportSize(const FVector2D& NewSize) { TestViewportSize = NewSize; }

	FVector2D GetTestViewportSize() { return TestViewportSize; }

	virtual float GetAspectRatio() const override
	{
		return 5.0;
	}

	////////////////////////
	// Events to Override //
	////////////////////////

	static FPointerEvent MakeFakeMoveMouseEvent(const FVector2D& CursorPosition, const FVector2D& LastCursorPosition, const FVector2D& CursorDelta)
	{
		return FPointerEvent(0, CursorPosition, LastCursorPosition, CursorDelta, TSet<FKey>(), FModifierKeysState());
	}

	static FPointerEvent MakeFakeScrollMouseEvent(const float WheelDelta)
	{
		return FPointerEvent(0, FVector2D(0.0f, 0.0f), FVector2D(0.0f, 0.0f), TSet<FKey>(), FKey(), WheelDelta, FModifierKeysState());
	}

	///////////////////////////
	// Wrappers to MapViewer //
	///////////////////////////

	FReply CallNativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return NativeOnMouseWheel(InGeometry, InMouseEvent);
	}

	FReply CallNativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
	{
		return NativeOnMouseMove(InGeometry, InMouseEvent);
	}
};