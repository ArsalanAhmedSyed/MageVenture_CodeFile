// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityComponentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAbilityComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MAGEVENTURE_API IAbilityComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Initialise(float DebuffDuration, float DebuffAmount, float DebuffRate, AActor* Owner) = 0;
};
