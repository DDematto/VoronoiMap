// @author Devin DeMatto \n I am just trying out some silly stuff :P


#include "UVoronoiGen.h"
#include "delaunator.hpp"
#include "jc_voronoi.h"

UVoronoiGen::UVoronoiGen()
{
}

UVoronoiGen::~UVoronoiGen()
{
}

void UVoronoiGen::Generate()
{
	// Generate Random Points for Map
	const TArray<double> coordinates = GeneratePoints();

	// Generate Delaunay Triangulation using Delaunator
	std::vector<double> stdCoordinates(coordinates.GetData(), coordinates.GetData() + coordinates.Num());
	delaunator::Delaunator d(stdCoordinates);

	// Build Delaunay Graph
	mDelaunayGraph.Triangles.Empty();
	for (std::size_t i = 0; i < d.triangles.size(); i += 3)
	{
		FDelaunayNode node;
		node.VertexIndices = { d.triangles[i], d.triangles[i + 1], d.triangles[i + 2] };
		mDelaunayGraph.Triangles.Add(node);
	}

	// Generate Voronoi Diagram using JCV
	jcv_diagram diagram;
	memset(&diagram, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(coordinates.Num() / 2, (const jcv_point*)coordinates.GetData(), nullptr, nullptr, &diagram);

	// Build Voronoi Graph
	BuildVoronoiGraph(&diagram);

	jcv_diagram_free(&diagram);

	// Relate Delaunay and Voronoi Graphs
	RelateGraphs();
}

void UVoronoiGen::BuildVoronoiGraph(const jcv_diagram* diagram)
{
	mVoronoiGraph.Vertices.Empty();
	mVoronoiGraph.Vertices.SetNum(diagram->numsites);

	const jcv_site* sites = jcv_diagram_get_sites(diagram);
	for (int i = 0; i < diagram->numsites; ++i)
	{
		const jcv_site* site = &sites[i];
		FVoronoiNode& node = mVoronoiGraph.Vertices[i];

		node.Position = FVector(site->p.x, site->p.y, 0);
	}
}


void UVoronoiGen::RelateGraphs()
{
	// Assuming each DelaunayNode corresponds to one VoronoiNode and vice versa
	for (size_t i = 0; i < mDelaunayGraph.Triangles.Num(); ++i)
	{
		FDelaunayNode& delaunayNode = mDelaunayGraph.Triangles[i];
		FVoronoiNode& voronoiNode = mVoronoiGraph.Vertices[i];

		// Establish Voronoi edges based on Delaunay adjacency:
		for (int32 adjacentIndex : delaunayNode.AdjacentTrianglesIndices)
		{
			// Each adjacent triangle corresponds to an adjacent Voronoi vertex
			if (!voronoiNode.AdjacentVerticesIndices.Contains(adjacentIndex))
			{
				voronoiNode.AdjacentVerticesIndices.Add(adjacentIndex);
			}
		}
	}
}


TArray<double> UVoronoiGen::GeneratePoints()
{
	TArray<double> coords;
	for (int i = 0; i < mResolution; ++i)
	{
		double x = FMath::RandRange(0, mWidth);
		double y = FMath::RandRange(0, mHeight);
		coords.Add(x);
		coords.Add(y);
	}

	return coords;
}
