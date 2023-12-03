// @author Devin DeMatto \n I am just trying out some silly stuff :P

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

private:
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

public:
	/// Constructor   
	explicit UMapViewer(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	TArray<FVector> TransformPolygons(const TArray<FVector>& Points) const;
	FVector TransformPoint(const FVector& Point) const;
	bool BoundsCheck();

protected:

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


	/////////////////////
	// Setters/Getters //
	/////////////////////

	int32 GetHeight() const { return Height; }

	int32 GetWidth() const { return Width; }

	void SetHeight(const int32 height) { Height = height; }

	void SetWidth(const int32 width) { Width = width; }

public:
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
