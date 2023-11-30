// @author Devin DeMatto @file MapGeneration.cpp

#include "MapGeneration.h"
#include "VectorUtil.h"
#include "MapNode.h"
#include <delaunator.hpp>
#include "Intersection/IntrLine2Line2.h"

/*
 * Constructor
 */
UMapGeneration::UMapGeneration(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
}

/*
 * Destructor
 */
UMapGeneration::~UMapGeneration()
{
}

//
// Generation Logic
//

/**
 * Generates the dual graph of the Voronoi and Delaunay diagrams.
 */
void UMapGeneration::Generate()
{
	GeneratePoints();
	delaunator::Delaunator Diagram = GenerateDiagram();
	GenerateDelaunayGraph(Diagram);
	GenerateVoronoiGraph(Diagram);
}

/**
 * Generates random points within the specified width and height.
 */
void UMapGeneration::GeneratePoints()
{
	Points.Empty();

	for (int i = 0; i < Resolution; ++i)
	{
		double X = FMath::RandRange(0, Width);
		double Y = FMath::RandRange(0, Height);
		Points.Add(FVector(X, Y, 0));
	}
}

/**
 * Creates a diagram using Delaunator. The diagram may be stored as a member variable.
 */
delaunator::Delaunator UMapGeneration::GenerateDiagram()
{
	std::vector<double> FlatPoints;

	for (const FVector& Point : Points)
	{
		FlatPoints.push_back(Point.X);
		FlatPoints.push_back(Point.Y);
	}

	// Generate Delaunay Triangulation using Delaunator
	return delaunator::Delaunator(FlatPoints);
}


