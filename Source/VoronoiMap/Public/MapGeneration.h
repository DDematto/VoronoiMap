// @author Devin DeMatto \n I am just trying out some silly stuff :P

#pragma once

#include "CoreMinimal.h"
#include "MapViewer.h"
#include "MapGeneration.generated.h"

/**
 *
 */
UCLASS()
class VORONOIMAP_API UMapGeneration : public UMapViewer
{
	GENERATED_BODY()

public:
	/// Constructor   
	UMapGeneration(const FObjectInitializer& ObjectInitializer) : UMapViewer(ObjectInitializer) {};

};
