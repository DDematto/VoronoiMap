/**
 * Main Class for Map Generation
 * @author Devin DeMatto
 * @file TerrainGenerator.cpp
 */

#include <SimplexNoiseBPLibrary.h>
#include "TerrainGenerator.h"
#include "MapGeneration.h"
#include "NodeEdge.h"
#include "Biomes.h"


void UTerrainGenerator::GenerateTerrain()
{
	GenerateLand();
	GenerateCliffs();
	GenerateBiomes();
}

void UTerrainGenerator::GenerateLand()
{
	LandNodes.Empty();
	LandEdges.Empty();

	// Set the simplex noise seed
	USimplexNoiseBPLibrary::setNoiseSeed(TerrainSeed);
	const FVector2D MapSize = MapGen->GetMapSize();

	// Terrain Generation Noise
	TArray<UMapNode*> Nodes = MapGen->GetNodes();
	for (UMapNode* Node : Nodes)
	{
		// Decrease the divisor to increase the scale of noise features
		const float NoiseX = Node->GetCentroid().X / (MapSize.X / 3); // Larger divisor = smaller features
		const float NoiseY = Node->GetCentroid().Y / (MapSize.Y / 3);

		// Generate noise value with adjusted parameters for larger features
		const float NoiseValue = USimplexNoiseBPLibrary::GetSimplexNoise2D_EX(NoiseX, NoiseY, TerrainLacunarity, TerrainPersistence,
			TerrainOctaves, 1.0, true);

		// Calculate square gradient
		const float GradientX = FMath::Abs(Node->GetCentroid().X - MapSize.X / 2) / (MapSize.X / 2);
		const float GradientY = FMath::Abs(Node->GetCentroid().Y - MapSize.Y / 2) / (MapSize.Y / 2);
		float SquareGradient = FMath::Max(GradientX, GradientY);

		// Lower power for a softer gradient
		SquareGradient = FMath::Pow(SquareGradient, 1.0f);

		// Apply the square gradient & Max Height
		Node->SetHeight(NoiseValue * (1.0f - SquareGradient) * MaxHeight, false);

		// If We Have Land Lets Cache it For Quicker Calculation
		if (Node->GetHeight() > SeaLevel)
		{
			LandNodes.Add(Node);
		}
		else
		{
			Node->SetBiome(EBiomeType::Sea);
		}
	}

	// Cache Edges
	TSet<UNodeEdge*> UniqueEdges;
	for (UMapNode* Node : LandNodes)
	{
		for (UNodeEdge* Edge : Node->Edges)
		{
			if (!LandEdges.Contains(Edge)) { LandEdges.Add(Edge); }
		}
	}
}

void UTerrainGenerator::GenerateBiomes() {
	const FVector2D MapSize = MapGen->GetMapSize();
	USimplexNoiseBPLibrary::setNoiseSeed(BiomeSeed);

	for (UMapNode* Node : LandNodes) {
		// Scale the coordinates for the noise function
		const float NoiseX = Node->GetCentroid().X / (MapSize.X / 2.0f);
		const float NoiseY = Node->GetCentroid().Y / (MapSize.Y / 2.0f);

		// Generate moisture value for each node
		const float Moisture = USimplexNoiseBPLibrary::GetSimplexNoise2D_EX(NoiseX, NoiseY, MoistureLacunarity, MoisturePersistence,
			MoistureOctaves, 1.0, true);

		// Calculate biome based on noise value (height) and moisture
		const EBiomeType Biome = DetermineBiome(Node, Moisture);
		Node->SetBiome(Biome);
	}

	// Group nodes into regions based on biome
	//GroupNodesIntoRegions();
}

EBiomeType UTerrainGenerator::DetermineBiome(const UMapNode* Node, const float Moisture) const
{
	// Define arbitrary thresholds for moisture to differentiate between biomes
	constexpr float MoistureThresholdForForest = 0.6f; // Above this value, it's likely to be a forest if it's not too high
	constexpr float MoistureThresholdForPlains = 0.3f; // Above this value, it's likely to be plains if it's not a forest

	// Define height thresholds for different biomes
	constexpr float ForestLevel = 600.0f; // Below this height, it can be a forest

	if (Node->GetHeight() < ForestLevel) {
		if (Moisture > MoistureThresholdForForest) {
			return EBiomeType::Forest;
		}
		else if (Moisture > MoistureThresholdForPlains) {
			return EBiomeType::Plains;
		}
		else {
			// If it's drier than the Plains threshold, we default to Plains with sparse vegetation
			return EBiomeType::Plains;
		}
	}

	// The higher we go, the less likely it is to support a dense forest, so it becomes plains or arctic tundra
	if (Moisture > MoistureThresholdForForest) {
		// High altitude forest, could be a mountain forest
		return EBiomeType::Forest;
	}

	return EBiomeType::Plains;
}

void UTerrainGenerator::GenerateCliffs()
{
	// Loop Through Edges And Find Cliffs
	for (UNodeEdge* Edge : LandEdges)
	{
		float HeightDiff = 0;

		if (Edge->Nodes.Num() == 2) // Check if there are two nodes
		{
			// Calculate the height difference between the two nodes
			HeightDiff = FMath::Abs(Edge->Nodes[0]->GetHeight() - Edge->Nodes[1]->GetHeight());
		}
		else if (Edge->Nodes.Num() == 1) // Check if there is only one node
		{
			// Calculate the height difference between the node and sea level
			HeightDiff = Edge->Nodes[0]->GetHeight() - SeaLevel;
		}

		// Check if the node's height exceeds sea level by CliffDiff or more
		if (HeightDiff >= CliffDiff)
		{
			Edge->SetEdgeType(EEdgeType::Cliff);
		}
	}
}