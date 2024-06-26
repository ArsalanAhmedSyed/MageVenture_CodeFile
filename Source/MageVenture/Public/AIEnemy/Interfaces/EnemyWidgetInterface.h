// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UEnemyWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MAGEVENTURE_API IEnemyWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/**
	 * Used to update the health bar value on the enemy
	 * 
	 * @param Percent Contains the bar fill amount value
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Eenemy")
	void UpdateBar(float Percent);
};
