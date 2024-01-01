/**
 * Main Class for Map Generation
 * @author Devin DeMatto
 * @file TerrainGenerator.cpp
 */

#include "TerrainGenerator.h"
#include <SimplexNoiseBPLibrary.h>
#include "MapGeneration.h"
#include "MapNode.h"


void TerrainGenerator::GenerateTerrain()
{
	// Set the simplex noise seed
	const int32 Seed = MapGen->GetMapSeed();
	USimplexNoiseBPLibrary::setNoiseSeed(Seed);

	const FVector2D MapSize = MapGen->GetMapSize();

	TArray<UMapNode*> Nodes = MapGen->GetNodes();
	for (UMapNode* Node : Nodes)
	{
		// Decrease the divisor to increase the scale of noise features
		const float NoiseX = Node->Centroid.X / (MapSize.X / 2.0f); // Larger divisor = smaller features
		const float NoiseY = Node->Centroid.Y / (MapSize.Y / 2.0f);

		// Generate noise value with adjusted parameters for larger features
		const float NoiseValue = USimplexNoiseBPLibrary::GetSimplexNoise2D_EX(
			NoiseX, NoiseY,
			1.8f, // Lacunarity - Adjust for feature frequency
			0.7f, // Persistence - Adjust for feature height
			5,    // Octaves - Reduce for less fragmentation
			1.0f, // InFactor
			true  // ZeroToOne
		);

		// Calculate square gradient
		const float GradientX = FMath::Abs(Node->Centroid.X - MapSize.X / 2) / (MapSize.X / 2);
		const float GradientY = FMath::Abs(Node->Centroid.Y - MapSize.Y / 2) / (MapSize.Y / 2);
		float SquareGradient = FMath::Max(GradientX, GradientY);
		SquareGradient = FMath::Pow(SquareGradient, 1.0f); // Lower power for a softer gradient

		// Apply the square gradient
		float FinalHeight = NoiseValue * (1.0f - SquareGradient);

		// Ensure we have a contiguous landmass by setting a minimum height
		if (FinalHeight < 0.2f) {
			FinalHeight = 0.2f; // This sets a base height to prevent too much sea
		}

		Node->Height = FMath::Clamp(FinalHeight * 100.0f, 0.0f, 100.0f);

		if (Node->Height > SeaLevel)
		{
			LandNodes.Add(Node);
		}
	}

	GenerateBiomes();
}


void TerrainGenerator::GenerateBiomes() const
{
	for (UMapNode* Node : LandNodes)
	{
		Node->bIsSea = false;



	}
}
