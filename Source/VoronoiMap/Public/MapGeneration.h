// @author Devin DeMatto \n I am just trying out some silly stuff :P

#pragma once

#include "CoreMinimal.h"
#include "InteractiveMap.h"
#include "MapGeneration.generated.h"

/**
 *
 */
UCLASS()
class VORONOIMAP_API UMapGeneration : public UInteractiveMap
{
	GENERATED_BODY()

public:
	/// Constructor   
	explicit UMapGeneration(const FObjectInitializer& ObjectInitializer) : UInteractiveMap(ObjectInitializer) {};

};
