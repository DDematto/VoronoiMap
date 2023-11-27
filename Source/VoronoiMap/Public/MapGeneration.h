/*
* @author Devin DeMatto
* Map Generation Class For Voronoi Dual Graph Approach
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapGeneration.generated.h"

// Forward declarations
class UMapNode;

namespace delaunator
{
	class Delaunator;
}

/**
* Node that Describes Triangles
*/
USTRUCT(BlueprintType)
struct FDelaunay
{
	GENERATED_BODY()

	// Indices of vertices forming the triangle
	TArray<int32> VertexIndices;

	// Indices of adjacent triangles
	TArray<int32> AdjacentTrianglesIndices;

	// Indicates if each edge is on the convex hull
	TArray<bool> IsEdgeOnConvexHull;

	// Intersection points with the bounding box (for edges on the convex hull)
	TArray<FVector> ConvexHullIntersectionPoints;
};


/**
 * UMapGeneration is a class responsible for generating a Voronoi map and Delaunay triangles.
 * This class provides functionality to create and visualize these structures for world generation.
 */
UCLASS(Blueprintable)
class VORONOIMAP_API UMapGeneration : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	/**
	 * Main entry function for generating the dual graph (Voronoi and Delaunay structures).
	 */
	void Generate();


private:
	//
	// Generation Helpers
	//
	void GeneratePoints();

	delaunator::Delaunator GenerateDiagram();

	void GenerateDelaunayGraph(delaunator::Delaunator);


	void GenerateVoronoiGraph(delaunator::Delaunator);

	FVector CalculateCircumcenter(const FDelaunay&);

private:
	// Holds all Voronoi nodes generated
	TArray<UMapNode*> Nodes;

	// Holds all Delaunay triangles generated
	TArray<FDelaunay> Triangles;

	// Holds all points generated
	TArray<FVector> Points;

public:
	//
	// Constructor & Destructor
	//
	UMapGeneration(const FObjectInitializer& ObjectInitializer);


	virtual ~UMapGeneration() override;

	//
	// Attributes for map dimensions and resolution
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Attributes")
	int Width = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Attributes")
	int Height = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Attributes")
	int Resolution = 500;

	//
	// Getters
	//
	int GetWidth() const { return Width; };
	int GetHeight() const { return Height; };
	TArray<UMapNode*> GetNodes() { return Nodes; }
	TArray<FDelaunay> GetTriangles() { return Triangles; }
	TArray<FVector> GetPoints() { return Points; };
};