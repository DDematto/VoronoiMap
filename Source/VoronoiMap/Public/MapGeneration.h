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
class UMapEdge;

namespace delaunator
{
	class Delaunator;
}

/**
 * UMapGeneration is a class responsible for generating a Voronoi map and Delaunay triangles.
 * This class provides functionality to create and visualize these structures for world generation.
 */
UCLASS(Blueprintable)
class VORONOIMAP_API UMapGeneration : public UObject
{
    GENERATED_BODY()

public:
    //
    // Constructor & Destructor
    //

    /// Constructor
    UMapGeneration(const FObjectInitializer& ObjectInitializer);

    /// Destructor
    virtual ~UMapGeneration() override;

    //
    // Map Generation Function
    //

    /// Main entry function for generating the dual graph (Voronoi and Delaunay structures)
    UFUNCTION(BlueprintCallable, Category = "Map Generation")
    void Generate();

private:
    //
    // Generation Helpers
    //

    /// Generates random points for Voronoi sites/Delaunay vertices
    void GeneratePoints();

    /// Generates initial Delaunay triangulation
    delaunator::Delaunator GenerateDiagram();

    /// Generates Delaunay graph from triangulation
    void GenerateDelaunayGraph(delaunator::Delaunator& delaunator);

    /// Generates Voronoi graph from Delaunay triangulation
    void GenerateVoronoiGraph(delaunator::Delaunator& delaunator);

    /// Identifies nodes on the boundary of the map
    void IdentifyBoundaryNodes();

    /// Handles infinite edges in Voronoi graph
    void HandleInfiniteEdges();

    /// Rebinds edges on the map boundary
    void RebindBoundaryEdges();

    /// Ensures the graph maintains Voronoi/Delaunay properties
    void ValidateGraphIntegrity();

    //
    // Node and Edge Storage
    //

    /// Array of nodes in the graph
    TArray<UMapNode*> Nodes;

    /// Array of edges in the graph
    TArray<UMapEdge*> Edges;

    /// Nodes on the edge of the map
    TArray<UMapNode*> BoundaryNodes;

    /// Points generated for Voronoi sites/Delaunay vertices
    TArray<FVector> Points;

public:
    //
    // Attributes for Map Dimensions and Resolution
    //

    /// Map Width
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Attributes")
    int Width = 1000;

    /// Map Height
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Attributes")
    int Height = 1000;

    /// Map Resolution
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Attributes")
    int Resolution = 500;

    //
    // Getters
    //

    /// Gets the map width
    int GetWidth() const { return Width; };

    /// Gets the map height
    int GetHeight() const { return Height; };

    /// Gets the nodes in the graph
    TArray<UMapNode*> GetNodes() const { return Nodes; }

    /// Gets the points generated for Voronoi sites/Delaunay vertices
    TArray<FVector> GetPoints() const { return Points; };

    /// Gets the nodes on the map's boundary
    TArray<UMapNode*> GetBoundaryNodes() const { return BoundaryNodes; }
};
