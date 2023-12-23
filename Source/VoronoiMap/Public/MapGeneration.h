/**
 * Main Class for Map Generation
 * @author Devin DeMatto
 * @file MapGeneration.h
 */

#pragma once

#include "CoreMinimal.h"
#include "InteractiveMap.h"
#include "MapGeneration.generated.h"

class UNodeEdge;
class UMapNode;

namespace delaunator {
	class Delaunator;
}

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawVoronoiEdges = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawDelaunayEdges = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawVoronoiCorners = false;


private:
	// Generates Points Around Map
	TArray<FVector2D> GeneratePoints() const;
	float DynamicExclusionRadius() const;

	// Create Delaunay Graph
	void GenerateMap();
	void GenerateGraph(const delaunator::Delaunator&);

	// Stores all Voronoi Nodes
	TArray<UMapNode*> Nodes;
};
