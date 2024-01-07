/**
 * Main Class for Describing Biomes
 * @author Devin DeMatto
 * @file Biomes.cpp
 */

#include "Biomes.h"

 // String to Enum conversion
EBiomeType UBiomes::ConvertStringToBiomeType(const FString& BiomeString)
{
	if (BiomeString.Equals(TEXT("Sea"), ESearchCase::IgnoreCase))
	{
		return EBiomeType::Sea;
	}
	else if (BiomeString.Equals(TEXT("Forest"), ESearchCase::IgnoreCase))
	{
		return EBiomeType::Forest;
	}
	else if (BiomeString.Equals(TEXT("Plains"), ESearchCase::IgnoreCase))
	{
		return EBiomeType::Plains;
	}

	// Default case if string does not match any enum
	return EBiomeType::Plains;
}

// Enum to String conversion
FString UBiomes::ConvertBiomeTypeToString(const EBiomeType BiomeType)
{
	switch (BiomeType)
	{
	case EBiomeType::Sea:
		return TEXT("Sea");
	case EBiomeType::Forest:
		return TEXT("Forest");
	case EBiomeType::Plains:
		return TEXT("Plains");
	default:
		return TEXT("Plains");
	}
}