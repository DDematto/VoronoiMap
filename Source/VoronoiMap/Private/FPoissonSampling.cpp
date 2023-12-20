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

#include "FPoissonSampling.h"

FPoissonSampling::FPoissonSampling()
{
}

FPoissonSampling::~FPoissonSampling()
{
}

TArray<FVector2D> FPoissonSampling::GeneratePoissonDiscSamples(const int Width, const int Height, const float ExclusionRadius, const int K, const int Iterations, UPARAM(ref) FRandomStream& RandomStream)
{
	// Create Return
	TArray<FVector2D> FinalSamples;

	// First Sample is completely random
	FinalSamples.Add(FVector2D(RandomStream.GetFraction() * Width, RandomStream.GetFraction() * Height));

	// Iterate
	for (int i = 0; i < Iterations; i++) { IteratePoissonDiscSamples(Width, Height, ExclusionRadius, K, RandomStream, FinalSamples); }

	return FinalSamples;
}

void FPoissonSampling::IteratePoissonDiscSamples(const int Width, const int Height, const float ExclusionRadius, const int K, const FRandomStream& RandomStream, TArray<FVector2D>& FinalSamplesArray)
{
	// Set Essential Values for the Sampler
	const float Radius2 = ExclusionRadius * ExclusionRadius;
	const float CellSize = ExclusionRadius * 0.707107f;

	// Reserve a contiguous location in memory
	const int GridWidth = FMath::CeilToInt(Width / CellSize);
	const int GridHeight = FMath::CeilToInt(Height / CellSize);

	// Create a "Queue" which will expand and contract as we use it.
	TArray<FVector2D> SamplesQueue;
	for (FVector2D const& Sample : FinalSamplesArray)
	{
		SamplesQueue.Add(Sample);
	}

	while (SamplesQueue.Num() > 0)
	{
		// Random Sample
		const int Index = RandomStream.RandRange(0, SamplesQueue.Num() - 1);
		const FVector2D Sample = SamplesQueue[Index];

		// Try to find a candidate within K attempts
		for (int j = 0; j < K; ++j)
		{
			const float Angle = 2.0f * PI * RandomStream.GetFraction();
			const float RandomRadius = (RandomStream.GetFraction() * Radius2) + ExclusionRadius;
			FVector2D Point = FVector2D(Sample.X + (RandomRadius * FMath::Cos(Angle)), Sample.Y + (RandomRadius * FMath::Sin(Angle)));

			// Reject if we fall outside the box
			if (false == IsPointInsideBox(Point, 0, Width, 0, Height))
			{
				continue;
			}

			// Reject if we are too close to a previous entry
			bool bHasFailedDistanceCheck = false;
			for (FVector2D const& ValidEntry : FinalSamplesArray)
			{
				const float DX = ValidEntry.X - Point.X;
				const float Dy = ValidEntry.Y - Point.Y;
				if (((DX * DX) + (Dy * Dy)) < Radius2)
				{
					bHasFailedDistanceCheck = true;
					break;
				}
			}

			if (true == bHasFailedDistanceCheck)
			{
				continue;
			}

			// Add the Point
			FinalSamplesArray.Add(Point);
		}

		// We've exhausted our checks at this sample, so we remove it.
		SamplesQueue.RemoveAt(Index);
	}
}

bool FPoissonSampling::IsPointInsideBox(FVector2D const& Point, const float MinX, const float MaxX, const float MinY, const float MaxY)
{
	return ((Point.X > MinX) && (Point.X < MaxX) && (Point.Y > MinY) && (Point.Y < MaxY));
}