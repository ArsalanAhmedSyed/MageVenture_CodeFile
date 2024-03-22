// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/AbilityComponentInterface.h"
#include "Components/ActorComponent.h"
#include "SlowComponent.generated.h"


class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class MAGEVENTURE_API USlowComponent : public UActorComponent, public IAbilityComponentInterface
{
	GENERATED_BODY()

public:
	USlowComponent();

protected:
	virtual void Initialise(float DebuffDuration, float DebuffAmount, float DebuffRate, AActor* Owner) override;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Begin();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDebuffAmount();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDebuffDuration(); 
	
private:
	float _DebuffDuration{};
	float _DebuffAmount{};
	float _DebuffRate{};
	AActor* _DebuffOwner{};

	void StartLifeSpanTimer();
	void RemoveComponent();
	FTimerHandle THLifeSpan{};
};
