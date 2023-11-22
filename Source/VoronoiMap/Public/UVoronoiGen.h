// @author Devin DeMatto \n I am just trying out some silly stuff :P

#pragma once

#include <jc_voronoi.h>
#include "CoreMinimal.h"
#include "UVoronoiGen.generated.h"

///
/// GRAPH STRUCTURE
///

/**
* Node that Describes Triangles
*/
USTRUCT(BlueprintType)
struct FDelaunayNode
{
	GENERATED_BODY()

	// Indices of vertices forming the triangle
	UPROPERTY(BlueprintReadOnly)
	TArray<int32> VertexIndices;

	// Indices of adjacent triangles
	UPROPERTY(BlueprintReadOnly)
	TArray<int32> AdjacentTrianglesIndices;
};


/**
* Graph that Describes Triangles
*/
USTRUCT(BlueprintType)
struct FDelaunayGraph
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FDelaunayNode> Triangles;
};

/**
* Node that Describes Voronoi Node
*/
USTRUCT(BlueprintType)
struct FVoronoiNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector Position;

	// Using indices for adjacency to be Blueprint-friendly
	UPROPERTY(BlueprintReadOnly)
	TArray<int32> AdjacentVerticesIndices;
};

/**
* Graph that Describes Voronoi
*/
USTRUCT(BlueprintType)
struct FVoronoiGraph
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FVoronoiNode> Vertices;
};

///
/// Vornoi Map Generation Main Class
///

/**
 * Main Entry for Generating Vornoi Map
 */
UCLASS(Blueprintable)
class VORONOIMAP_API UVoronoiGen : public UObject
{
	GENERATED_BODY()

private:

	void BuildVoronoiGraph(const jcv_diagram* diagram);
	void RelateGraphs();

public:
	/// Constructor
	UVoronoiGen();

	/// Destructor
	~UVoronoiGen();


	/// Width of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Settings")
	int mWidth = 500;

	/// Height of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Settings")
	int mHeight = 500;

	/// Resolution of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Settings")
	int mResolution = 100;

	// Delaunay Graph Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Data")
	FDelaunayGraph mDelaunayGraph;

	// Voronoi Graph Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Data")
	FVoronoiGraph mVoronoiGraph;

	UFUNCTION(BlueprintCallable, Category = "Voronoi Generation")
	void Generate();

	/// Generate Coordinates for Map Generation
	TArray<double> GeneratePoints();
};
