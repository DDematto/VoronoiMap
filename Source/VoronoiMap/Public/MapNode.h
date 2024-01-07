/**
 * @author Devin DeMatto
 * @file MapNode.h
 */

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Biomes.h"
#include "MapNode.generated.h"

enum class EBiomeType : uint8;
class UMapGeneration;
class UNodeEdge;

/**
 * Base Class for Voronoi Node
 */
UCLASS()
class VORONOIMAP_API UMapNode : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	// Reference to Map Generator
	UMapGeneration* MapGenerator = nullptr;

	UPROPERTY()
	// Neighboring Nodes
	TArray<UMapNode*> Neighbors;

	UPROPERTY()
	// Edges Making up Nodes
	TArray<UNodeEdge*> Edges;

private:
	// Type of Biome
	EBiomeType BiomeType;

	// Centroid of Node (Original Point)
	FVector2D Centroid;

	// Height of Node
	float Height = 0.0f;

	// Vertices for Mesh Generation
	TArray<FVector2D> Vertices;

	// Indices for Mesh Generation
	TArray<SlateIndex> Indices;

	// Color of 2D Polygon
	FColor Color;

	// Color of Centroid
	FLinearColor CentroidColor = FLinearColor::Black;

	// Draw Vertices Traversal (DEBUG)
	bool bDrawVerticesTraversal = false;

	// Default constructor
	explicit UMapNode(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer) {}

	void SortEdges();

	// Node Positioning Check
	bool IsInNode(const FVector2D& Point);

public:
	///////////////////////////
	//  Structure Generation //
	///////////////////////////

	void SetupNode(UMapGeneration*, FVector2D);
	void AddNeighbor(UMapNode*);
	void AddEdge(UNodeEdge*);
	void BuildMesh();

	/////////////////////
	/// Node Selection //
	/////////////////////

	void Default();
	void Selected();
	void UpdateNode();

	///////////////////////
	// Setters / Getters //
	///////////////////////

	UFUNCTION(BlueprintPure, Category = "Node Data")
	EBiomeType GetBiome() const;

	UFUNCTION(BlueprintCallable, Category = "Node Data")
	void SetBiome(const EBiomeType Biome, const bool bIsUpdate = true);

	UFUNCTION(BlueprintPure, Category = "Node Data")
	float GetHeight() const;

	UFUNCTION(BlueprintCallable, Category = "Node Data")
	void SetHeight(const float Value, const bool bIsUpdate = true);

	UFUNCTION(BlueprintPure, Category = "Node Data")
	FVector2D GetCentroid() const;

	//////////////////
	//  Event Logic //
	//////////////////

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
							  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
							  bool bParentEnabled) const override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
