
#pragma once

#include "CoreMinimal.h"
#include "Ability/AbilityComponentInterface.h"
#include "Components/ActorComponent.h"
#include "EffectComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class MAGEVENTURE_API UEffectComponent : public UActorComponent, public IAbilityComponentInterface
{
	GENERATED_BODY()

protected:
	virtual void Initialise(float DebuffDuration, float DebuffAmount, float DebuffRate, AActor* Owner) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | Component")
	void Begin();

	UPROPERTY(BlueprintReadOnly, Category = "Ability | Component")
	float _DebuffDuration{};
	UPROPERTY(BlueprintReadOnly, Category = "Ability | Component")
	float _DebuffAmount{};
	UPROPERTY(BlueprintReadOnly, Category = "Ability | Component")
	float _DebuffRate{};
	UPROPERTY(BlueprintReadOnly, Category = "Ability | Component")
	AActor* _DebuffOwner{};

	UFUNCTION(BlueprintCallable, Category = "Ability | Component")
	void ApplyEffectDamage() const;

	FTimerHandle TH_Destroyer{};
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability | Component")
	void OnComponentDurationEnd();
};
