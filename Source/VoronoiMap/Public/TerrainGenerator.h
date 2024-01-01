/**
 * Main Class for Map Generation
 * @author Devin DeMatto
 * @file TerrainGenerator.h
 */

#pragma once

#include "CoreMinimal.h"

class UMapGeneration;
class UNodeEdge;
class UMapNode;

/**
 * Main Class for Terrain Generation
 */
class VORONOIMAP_API TerrainGenerator
{
public:
	explicit TerrainGenerator(UMapGeneration* MapGenInstance) : MapGen(MapGenInstance) {}

	~TerrainGenerator() {};

	void GenerateTerrain();
	void GenerateBiomes() const;

private:
	// Map Generator Data
	UMapGeneration* MapGen;

	// Sea Level
	float SeaLevel = 20;

	// Stores all Land Nodes
	TArray<UMapNode*> LandNodes;
};
