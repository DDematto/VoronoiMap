/**
 * Main Class for Describing Biomes
 * @author Devin DeMatto
 * @file Biomes.h
 */

#pragma once

#include "CoreMinimal.h"
#include "Biomes.generated.h"

UENUM(BlueprintType)
enum class EBiomeType : uint8
{
	Sea UMETA(DisplayName = "Sea"),
	Forest UMETA(DisplayName = "Forest"),
	Plains UMETA(DisplayName = "Plains"),
};

/**
 * Basic Utility for Converting To and From Enums
 */
UCLASS()
class VORONOIMAP_API UBiomes : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Biome Conversion")
	static EBiomeType ConvertStringToBiomeType(const FString& BiomeString);

	UFUNCTION(BlueprintCallable, Category = "Biome Conversion")
	static FString ConvertBiomeTypeToString(EBiomeType BiomeType);
};
