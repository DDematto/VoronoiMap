/**
 * Main Class for Map Generation
 * @author Devin DeMatto
 * @file MapGeneration.h
 */

#pragma once

#include "CoreMinimal.h"
#include "InteractiveMap.h"
#include "MapGeneration.generated.h"

 /**
  * Map Generation Class
  */
UCLASS()
class VORONOIMAP_API UMapGeneration : public UInteractiveMap
{
	GENERATED_BODY()

public:
	/// Constructor   
	explicit UMapGeneration(const FObjectInitializer& ObjectInitializer) : UInteractiveMap(ObjectInitializer) {};

	virtual void NativeConstruct() override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;


private:
	// Generates Points Around Map
	void GeneratePoints();
	float DynamicExclusionRadius() const;

	// Holds Positions of Points
	TArray<FVector2D> DataPoints;

protected:

public:
	// Getters/Setters

};
