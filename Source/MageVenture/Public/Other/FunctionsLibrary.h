// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionsLibrary.generated.h"

enum class EInputModeType : uint8;

UCLASS()
class MAGEVENTURE_API UFunctionsLibrary : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Takes the attackers Attack stat and defenders Defence stat
	 * to calculate how much damage should be applied.
	 * 
	 * @param DamageAmount  Amount of Damage to apply
	 * @param Attacker The Actor that attacked
	 * @param Defender The Actor that got attacked
	 * @return The final amount of damage to apply
	 */
	UFUNCTION(BlueprintCallable, Category = "Public Damage Functions")
	static float CalculateDamage(float DamageAmount, AActor* Attacker, AActor* Defender);

	static 	void ChangeInputMode(const UObject* WorldObject, APlayerController* PlayerController, EInputModeType InputModeType, UUserWidget* WidgetToFocus);

};
