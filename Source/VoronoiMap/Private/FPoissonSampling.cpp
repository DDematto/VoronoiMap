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

TArray<FVector2D> FPoissonSampling::GeneratePoissonDiscSamples(const int Width, const int Height, const float Spacing, const int K, const int Iterations, UPARAM(ref) FRandomStream& RandomStream)
{
	// Create Return
	TArray<FVector2D> FinalSamples;

	// Adjust ExclusionRadius based on Spacing
	const float ExclusionRadius = Spacing / 2.0f;

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
	const float CellSize = ExclusionRadius * 0.707107f; // sqrt(2)/2 factor for grid cell size

	// Calculate Grid Dimensions
	const int GridWidth = FMath::CeilToInt(Width / CellSize);
	const int GridHeight = FMath::CeilToInt(Height / CellSize);

	// Initialize the grid
	TArray<TArray<int32>> Grid;
	Grid.SetNum(GridWidth);
	for (int i = 0; i < GridWidth; ++i)
	{
		Grid[i].SetNumZeroed(GridHeight);
	}

	// Populate the grid with initial samples
	for (const FVector2D& Sample : FinalSamplesArray)
	{
		const int x = FMath::FloorToInt(Sample.X / CellSize);
		const int y = FMath::FloorToInt(Sample.Y / CellSize);
		Grid[x][y] = 1; // Mark the cell as occupied
	}

	// Create a "Queue" which will expand and contract as we use it.
	TArray<FVector2D> SamplesQueue = FinalSamplesArray;

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

			// Calculate grid coordinates for the point
			const int GridX = FMath::FloorToInt(Point.X / CellSize);
			const int GridY = FMath::FloorToInt(Point.Y / CellSize);

			// Reject if we fall outside the box or grid cell is already occupied
			if (!IsPointInsideBox(Point, 0, Width, 0, Height) || Grid[GridX][GridY] == 1)
			{
				continue;
			}

			// Check surrounding cells for existing points
			bool bTooClose = false;
			for (int DX = -2; DX <= 2 && !bTooClose; ++DX)
			{
				for (int Dy = -2; Dy <= 2 && !bTooClose; ++Dy)
				{
					const int NeighborX = GridX + DX;
					const int NeighborY = GridY + Dy;

					// Check if neighbor is within grid bounds
					if (NeighborX >= 0 && NeighborX < GridWidth && NeighborY >= 0 && NeighborY < GridHeight && Grid[NeighborX][NeighborY] == 1)
					{
						const FVector2D NeighborPoint = FVector2D(NeighborX * CellSize, NeighborY * CellSize);
						const float DistX = NeighborPoint.X - Point.X;
						const float DistY = NeighborPoint.Y - Point.Y;
						if ((DistX * DistX + DistY * DistY) < Radius2)
						{
							bTooClose = true;
						}
					}
				}
			}

			if (!bTooClose)
			{
				// Add the Point and mark the grid cell
				FinalSamplesArray.Add(Point);
				Grid[GridX][GridY] = 1;
			}
		}

		// Remove the processed sample from the queue
		SamplesQueue.RemoveAt(Index);
	}
}

bool FPoissonSampling::IsPointInsideBox(FVector2D const& Point, const float MinX, const float MaxX, const float MinY, const float MaxY)
{
	return ((Point.X > MinX) && (Point.X < MaxX) && (Point.Y > MinY) && (Point.Y < MaxY));
}