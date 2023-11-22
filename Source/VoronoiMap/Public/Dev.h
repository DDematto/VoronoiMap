// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
UCLASS()
class VORONOIMAP_API Dev : public AActor
{
private:
	/// Width of Map
	int mWidth = 100;

	/// Height of Map
	int mHeight = 100;

public:
	Dev();
	~Dev();

};
