//////////////////////////////////////////////////////////////////////////////
// Returns a Series of 2D Local Space Locations Evenly Spaced from one another to avoid unpleasant clumping
// of locations. You can use these 2D Locations by adding a Worldspace Center Location, and finding a navigable
// point on the NavMesh.
//
// This was Written by Nick Crosgrove, 2022
// Based in Part on https://observablehq.com/@techsparx/an-improvement-on-bridsons-algorithm-for-poisson-disc-samp/2
// This Poisson Disk Sampler Algorithm is Released as a CC-BY License
// Commercial Usage is Fine, but Attribution is Required.
// The full Text of the license can be found here: https://creativecommons.org/licenses/by/4.0/legalcode
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"

class VORONOIMAP_API FPoissonSampling
{
public:
	FPoissonSampling();
	~FPoissonSampling();

	static TArray<FVector2D> GeneratePoissonDiscSamples(const int Width, const int Height, const float ExclusionRadius, const int K, const int Iterations, UPARAM(ref) FRandomStream
														& RandomStream);

private:
	static void IteratePoissonDiscSamples(const int Width, const int Height, const float ExclusionRadius, const int K, const FRandomStream& RandomStream, TArray<
										  FVector2D>& FinalSamplesArray);
	static bool IsPointInsideBox(FVector2D const& Point, const float MinX, const float MaxX, const float MinY, const float MaxY);
};
