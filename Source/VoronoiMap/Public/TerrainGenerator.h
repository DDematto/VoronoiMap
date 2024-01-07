/**
 * Main Class for Map Generation
 * @author Devin DeMatto
 * @file TerrainGenerator.h
 */

#pragma once

#include "CoreMinimal.h"
#include "MapNode.h"
#include "TerrainGenerator.generated.h"

class UMapGeneration;
class UNodeEdge;
class UMapNode;

/**
 * Main Class for Terrain Generation
 */
UCLASS()
class VORONOIMAP_API UTerrainGenerator : public UObject
{
	GENERATED_BODY()

public:
	/// Constructor
	explicit UTerrainGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

	/// Map Generator Data
	UPROPERTY()
	UMapGeneration* MapGen = nullptr;

	/// Stores all Land Nodes
	UPROPERTY()
	TArray<UMapNode*> LandNodes;

	/// Stores all Land Edges
	UPROPERTY()
	TArray<UNodeEdge*> LandEdges;

	////////////////////////
	// Terrain Parameters //
	////////////////////////

	// Map Seed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Generator Stats")
	int32 TerrainSeed = FMath::RandRange(0, 1000);

	// Biome Seed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Generator Stats")
	int32 BiomeSeed = FMath::RandRange(0, 1000);

	/// Max Height of Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Stats")
	float MaxHeight = 1000;

	/// Sea Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Stats")
	float SeaLevel = 120;

	/// Difference in Height for Cliffs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Stats")
	float CliffDiff = 100;

	/////////////////////////
	// Terrain Noise Stats //
	/////////////////////////

	/// Lacunarity - Adjust for feature frequency
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise Stats")
	float TerrainLacunarity = 1.8f;

	/// Persistence - Adjust for feature height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise Stats")
	float TerrainPersistence = 0.7f;

	/// Octaves - Reduce for less fragmentation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Noise Stats")
	float TerrainOctaves = 5;

	/////////////////////////
	// Moisture Noise Stats /
	/////////////////////////

	/// Lacunarity - Adjust for feature frequency
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moisture Noise Stats")
	float MoistureLacunarity = 1.8f;

	/// Persistence - Adjust for feature height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moisture Noise Stats")
	float MoisturePersistence = 0.7f;

	/// Octaves - Reduce for less fragmentation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moisture Noise Stats")
	float MoistureOctaves = 5;

	// Main method Generating Terrain, Land, Biomes, etc...
	UFUNCTION(BlueprintCallable, Category = "Terrain Functions")
	void GenerateTerrain();

	// Generates New Land Nodes
	UFUNCTION(BlueprintCallable, Category = "Terrain Functions")
	void GenerateLand();

	// Generates New Biome Over Land
	UFUNCTION(BlueprintCallable, Category = "Terrain Functions")
	void GenerateBiomes();

	EBiomeType DetermineBiome(const UMapNode* Node, float Moisture) const;
	void GenerateCliffs();
};
