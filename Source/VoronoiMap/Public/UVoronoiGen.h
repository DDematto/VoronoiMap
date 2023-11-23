// @author Devin DeMatto \n I am just trying out some silly stuff :P

#pragma once

#include "CoreMinimal.h"
#include "UVoronoiGen.generated.h"

namespace delaunator {
	class Delaunator;
}

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

	TArray<double> GeneratePoints() const;
	void BuildDelaunayGraph(const delaunator::Delaunator& D);
	void BuildVoronoiGraph(const delaunator::Delaunator& D);
	static FVector ComputeCircumcenter(const FVector& P1, const FVector& P2, const FVector& P3);
	void RelateGraphs(const delaunator::Delaunator& D);
	static size_t FindTriangleContainingPoint(const delaunator::Delaunator& D, size_t PointIndex);

public:
	/// Constructor
	UVoronoiGen();

	/// Destructor
	~UVoronoiGen();


	/// Width of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Settings")
	int MWidth = 500;

	/// Height of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Settings")
	int MHeight = 500;

	/// Resolution of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Settings")
	int MResolution = 100;

	// Delaunay Graph Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Data")
	FDelaunayGraph MDelaunayGraph;

	// Voronoi Graph Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voronoi Data")
	FVoronoiGraph MVoronoiGraph;

	UFUNCTION(BlueprintCallable, Category = "Voronoi Generation")
	void Generate();
};
