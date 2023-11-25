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
	TArray<int32> VertexIndices;

	// Indices of adjacent triangles
	TArray<int32> AdjacentTrianglesIndices;
};


/**
* Graph that Describes Triangles
*/
USTRUCT(BlueprintType)
struct FDelaunayGraph
{
	GENERATED_BODY()

	TArray<FDelaunayNode> Triangles;
};

USTRUCT(BlueprintType)
struct FVoronoiNode
{
	GENERATED_BODY()

	FVector Position; // Position of the Voronoi node.

	// Using indices for adjacency to be Blueprint-friendly
	TArray<int32> AdjacentVerticesIndices;
};


USTRUCT(BlueprintType)
struct FVoronoiGraph
{
	GENERATED_BODY()

	TArray<FVoronoiNode> Vertices; // Existing vertices.
};


struct FDelaunayEdge
{
	int32 A, B;

	FDelaunayEdge(int32 InA, int32 InB) : A(InA), B(InB) {}

	// Ensure that edge (A, B) is equal to edge (B, A)
	bool operator==(const FDelaunayEdge& Other) const
	{
		return (A == Other.A && B == Other.B) || (A == Other.B && B == Other.A);
	}

	// Custom hash function
	friend uint32 GetTypeHash(const FDelaunayEdge& Edge)
	{
		return GetTypeHash(Edge.A) ^ GetTypeHash(Edge.B);
	}
};

///
/// Vornoi Map Generation Main Class
///

/**
 * Main Entry for Generating Vornoi Map
 */
UCLASS()
class VORONOIMAP_API UVoronoiGen : public UObject
{
	GENERATED_BODY()

private:
	// Generates Random Points in Range of Width and Height, stores in Original Points
	TArray<double> GeneratePoints();

	// Builds the Delauny Graph given data from delaunator
	void BuildDelaunayGraph(const delaunator::Delaunator& D);

	// Builds Voronoi Graph Based on Data from Delaunator
	void BuildVoronoiGraph(const delaunator::Delaunator& D);

	// Computes Circumference of 3 Points
	static FVector ComputeCircumcenter(const FVector& P1, const FVector& P2, const FVector& P3);
public:
	/// Constructor
	UVoronoiGen();

	/// Destructor
	~UVoronoiGen();

	/// Width of Map
	int MWidth = 500;

	/// Height of Map
	int MHeight = 500;

	/// Resolution of Map (Amount of Points to Generate)
	int MResolution = 100;

	// Array to hold the original points used for triangulation
	TArray<FVector> MOriginalPoints;

	// Delaunay Graph Instance
	FDelaunayGraph MDelaunayGraph;

	// Voronoi Graph Instance
	FVoronoiGraph MVoronoiGraph;

	// Main Generation Function
	void Generate();
};
