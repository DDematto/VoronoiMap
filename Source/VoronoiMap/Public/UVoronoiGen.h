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
struct FDelaunayNode
{
	// Indices of vertices forming the triangle
	TArray<int32> VertexIndices;

	// Indices of adjacent triangles
	TArray<int32> AdjacentTrianglesIndices;
};


/**
* Graph that Describes Triangles
*/
struct FDelaunayGraph
{
	TArray<FDelaunayNode> Triangles;
};

/**
* Node that Describes Voronoi Node
*/
struct FVoronoiNode
{
	FVector Position;

	// Using indices for adjacency to be Blueprint-friendly
	TArray<int32> AdjacentVerticesIndices;
};

/**
* Graph that Describes Voronoi
*/
struct FVoronoiGraph
{
	TArray<FVoronoiNode> Vertices;
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
	int MWidth = 500;

	/// Height of Map
	int MHeight = 500;

	/// Resolution of Map
	int MResolution = 100;

	// Delaunay Graph Instance
	FDelaunayGraph MDelaunayGraph;

	// Voronoi Graph Instance
	FVoronoiGraph MVoronoiGraph;

	void Generate();
};
