#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVoronoiGen.h"
#include "Visualizer.generated.h"

UCLASS()
class VORONOIMAP_API UVisualizer : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Visualization")
	void SetGraphData(const FVoronoiGraph& VoronoiGraphData, const FDelaunayGraph& DelaunayGraphData);

	UFUNCTION(BlueprintCallable, Category = "Visualization")
	void CalculateAspectRatio(int OriginalMapWidth, int OriginalMapHeight);

	UFUNCTION(BlueprintCallable, Category = "Visualization")
	void SetOriginalPoints(TArray<FVector> OriginalPoints);

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void DrawBorder(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

	void DrawDelaunayTriangles(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
							   const FSlateRect& MyCullingRect,
							   FSlateWindowElementList& OutDrawElements, int32 LayerId,
							   const FWidgetStyle& InWidgetStyle,
							   bool bParentEnabled) const;

	void DrawVoronoiEdges(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
						  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
						  bool bParentEnabled) const;

private:
	mutable FGeometry CachedAllottedGeometry;
	double MScaleX = 0.5;
	double MScaleY = 0.5;
	int MWidth = 500;
	int MHeight = 500;

	FVoronoiGraph VoronoiGraph;
	FDelaunayGraph DelaunayGraph;
	TArray<FVector> MOriginalPoints;
	bool bHasVoronoiGraphData = false;
	bool bHasDelaunayGraphData = false;
	bool bHasPoints = false;
};
