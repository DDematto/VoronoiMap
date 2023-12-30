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
struct FDelaunayMesh;

/**
 * Map Generation Class
 */
UCLASS()
class VORONOIMAP_API UMapGeneration : public UInteractiveMap
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawVoronoiEdges = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawDelaunayTriangles = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawVoronoiCentroids = true;

	// Constructor   
	explicit UMapGeneration(const FObjectInitializer& ObjectInitializer) : UInteractiveMap(ObjectInitializer) {};

	virtual void NativeConstruct() override;

	//////////////////
	//  Event Logic //
	//////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY()
	UMapNode* SelectedNode = nullptr;

private:
	// Stores all Voronoi Nodes
	UPROPERTY()
	TArray<UMapNode*> Nodes;

	UPROPERTY()
	TArray<UMapNode*> InvalidNodes;

	// Generates Poisson Distribution
	TArray<FVector2D> GeneratePoints() const;

	// Generate Margin Around Map For Clipping
	FVector2D BoundaryOffset = FVector2D(150, 150);

	// Create Delaunay Graph
	void GenerateMap();

	// Creates Relationship from Delaunay Graphs to Nodes & Edges
	void GenerateGraph(const FDelaunayMesh&, const TArray<FVector2D>& Points);
	void ProcessInvalidNodes();

public:
	void MarkNodeForRemoval(UMapNode* Node);
};
