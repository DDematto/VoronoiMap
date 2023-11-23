// MapVisualization.h

#pragma once

#include "CoreMinimal.h"
#include "UVoronoiGen.h"
#include "Blueprint/UserWidget.h"
#include "MapVisualization.generated.h"

UCLASS()
class VORONOIMAP_API UMapVisualization : public UUserWidget
{
	GENERATED_BODY()

private:
	// Reference to the Voronoi generator
	TWeakObjectPtr<UVoronoiGen> VoronoiGenerator;

public:
	// References to the sliders and button in the Blueprint
	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Voronoi Data")
	class USlider* HeightSlider;

	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Voronoi Data")
	class USlider* WidthSlider;

	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Voronoi Data")
	class USlider* ResolutionSlider;

	UPROPERTY(meta = (BindWidget), EditAnywhere, Category = "Voronoi Data")
	class UButton* GenerateButton;



	// Functions to handle slider value changes and button click
	UFUNCTION(BlueprintCallable, Category = "Voronoi")
	void SetHeight(float Height);

	UFUNCTION(BlueprintCallable, Category = "Voronoi")
	void SetWidth(float Width);

	UFUNCTION(BlueprintCallable, Category = "Voronoi")
	void SetResolution(float Resolution);

	UFUNCTION(BlueprintCallable, Category = "Voronoi")
	void GenerateVoronoi();

protected:
	virtual void NativeConstruct() override;
};
