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
	// Store Currently Selected Node
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	UMapNode* SelectedNode = nullptr;

	// Should We Draw Node Edges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawVoronoiEdges = true;

	// Should we Draw Nodes Association to Neighbors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawDelaunayTriangles = true;

	// Should we Draw Node Centroid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapGeneration Stats")
	bool bDrawVoronoiCentroids = true;

	/// Constructor   
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

	////////////////////
	// Public Methods //
	////////////////////

	UFUNCTION(BlueprintCallable, Category = "MapGeneration")
	void GenerateMap();

	UFUNCTION(BlueprintPure, Category = "MapGeneration")
	UMapNode* GetSelectedNode() const { return SelectedNode; }

	void MarkNodeForRemoval(UMapNode* Node);

	void SetSelectedNode(UMapNode* Node);

private:
	// Stores all Voronoi Nodes
	UPROPERTY()
	TArray<UMapNode*> Nodes;

	UPROPERTY()
	// Temporarily Holds Invalid Nodes
	TArray<UMapNode*> InvalidNodes;

	// Generate Margin Around Map For Clipping
	FVector2D BoundaryOffset = FVector2D(150, 150);

	////////////////////////////
	// Map Generation Methods //
	////////////////////////////

	TArray<FVector2D> GeneratePoints() const;

	void GenerateGraph();

	void RelateGraph(const FDelaunayMesh&, const TArray<FVector2D>& Points);

	void ProcessInvalidNodes();
};
