/**
 * @author Devin DeMatto
 * @file MapGeneration.cpp
 */

#include "MapGeneration.h"
#include "FPoissonSampling.h"

 // Constructor and Events
void UMapGeneration::NativeConstruct()
{
	Super::NativeConstruct();
	GeneratePoints();
}

int32 UMapGeneration::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const auto InContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	for (auto Point : DataPoints)
	{
		DrawPoint(InContext, AllottedGeometry, Point, FLinearColor::Blue);
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

// Logic

/**
 * Creates a Poisson Distribution of Points Based on MapSize
 */
void UMapGeneration::GeneratePoints()
{
	// Define parameters for Poisson Disk Sampling
	const float ExclusionRadius = DynamicExclusionRadius(); // Define an appropriate exclusion radius
	constexpr int Iterations = 20; // Set the number of iterations for the sampling
	constexpr int K = 3; // Amount of Times a Point will attempt to place

	// Initialize the random stream
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();

	// Call the Poisson Disk Sampling function
	DataPoints = FPoissonSampling::GeneratePoissonDiscSamples(MapSize.X, MapSize.Y, ExclusionRadius, K, Iterations, RandomStream);
}

float UMapGeneration::DynamicExclusionRadius() const
{
	constexpr float BaseMapSize = 500.0f; // Base map size (both width and height)
	constexpr float BaseExclusionRadius = 25.0f; // Base exclusion radius for the minimum map size

	const float ScaleFactor = FMath::Sqrt((MapSize.X * MapSize.Y) / (BaseMapSize * BaseMapSize));
	return BaseExclusionRadius * ScaleFactor;
}


// Testing Helper
