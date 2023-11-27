/*
* @author Devin DeMatto
* Map Editor Class For Visualizing Generation Data and Manipulating
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapGeneration.h"
#include "MapEditor.generated.h"

/**
 * Map Editor Class For Visualizing Generation Data and Manipulating
 */
UCLASS()
class VORONOIMAP_API UMapEditor : public UUserWidget
{
	GENERATED_BODY()

public:
	// Constructor
	UMapEditor(const FObjectInitializer& ObjectInitializer);

protected:
	// Paint Method
	virtual int32 NativePaint(const FPaintArgs&, const FGeometry&, const FSlateRect&, FSlateWindowElementList&, int32, const FWidgetStyle&, bool) const override;

private:
	TWeakObjectPtr<UMapGeneration> MapGenerationInstance;

	//
	// Visualization Helpers
	//
	void DrawPoints(const FPaintContext&) const;

	void DrawVoronoiEdges(const FPaintContext&) const;

	void DrawDelaunayEdges(const FPaintContext&) const;

	void DrawBoundingBox(const FPaintContext&) const;

public:
	//
	// Boolean attributes to control the visualization
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization Flags")
	bool bShowVoronoiEdges = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization Flags")
	bool bShowVoronoiCenters = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization Flags")
	bool bShowDelaunayEdges = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization Flags")
	bool bShowBoundingBox = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization Flags")
	bool bShowPoints = true;

	// Function to set the MapGeneration instance
	UFUNCTION(BlueprintCallable, Category = "Map Editor")
	void SetMapGenerationInstance(UMapGeneration* NewInstance) { MapGenerationInstance = NewInstance; };
};
