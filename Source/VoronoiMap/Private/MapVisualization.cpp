
#include "MapVisualization.h"
#include "Components/Button.h"
#include "Components/Slider.h"

void UMapVisualization::NativeConstruct()
{
	Super::NativeConstruct();

	// Create the Voronoi generator instance
	VoronoiGenerator = NewObject<UVoronoiGen>(this, UVoronoiGen::StaticClass());

	// Ensure the sliders and button are valid
	if (HeightSlider && WidthSlider && ResolutionSlider && GenerateButton)
	{
		// Bind the slider events to the functions
		HeightSlider->OnValueChanged.AddDynamic(this, &UMapVisualization::SetHeight);
		WidthSlider->OnValueChanged.AddDynamic(this, &UMapVisualization::SetWidth);
		ResolutionSlider->OnValueChanged.AddDynamic(this, &UMapVisualization::SetResolution);

		// Bind the button click event to the function
		GenerateButton->OnClicked.AddDynamic(this, &UMapVisualization::GenerateVoronoi);

		// Initialize the slider values from the Voronoi generator
		HeightSlider->SetValue(VoronoiGenerator->MHeight);
		WidthSlider->SetValue(VoronoiGenerator->MWidth);
		ResolutionSlider->SetValue(VoronoiGenerator->MResolution);
	}
}

void UMapVisualization::SetHeight(const float Height)
{
	if (VoronoiGenerator.IsValid())
	{
		VoronoiGenerator->MHeight = FMath::RoundToInt(Height);
	}
}

void UMapVisualization::SetWidth(const float Width)
{
	if (VoronoiGenerator.IsValid())
	{
		VoronoiGenerator->MWidth = FMath::RoundToInt(Width);
	}
}

void UMapVisualization::SetResolution(const float Resolution)
{
	if (VoronoiGenerator.IsValid())
	{
		VoronoiGenerator->MResolution = FMath::RoundToInt(Resolution);
	}
}

void UMapVisualization::GenerateVoronoi()
{
	if (VoronoiGenerator.IsValid())
	{
		VoronoiGenerator->Generate();
	}
}
