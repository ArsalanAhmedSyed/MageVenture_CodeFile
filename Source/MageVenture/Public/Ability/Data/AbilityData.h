// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Strcuts/FPropertyScale.h"
#include "Ability/Actors/BaseAbility.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"

class UEffectComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class MAGEVENTURE_API UAbilityData : public UDataAsset
{
	GENERATED_BODY()
	
public:

#pragma region // Visual
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Visual")
	TObjectPtr<UStaticMesh> AbilityMesh{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Visual")
	TObjectPtr<UParticleSystem> ParticleEffect{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Visual")
	TObjectPtr<USoundBase> SoundEffect{};
#pragma endregion // Visual
	
#pragma region // General
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | General")
	float ManaConsumeAmount{};
	UPROPERTY(EditDefaultsOnly, Category = "Ability | General")
	FPropertyScale Damage{};
	UPROPERTY(EditDefaultsOnly, Category = "Ability | General", meta=(ToolTip="After how many seconds should the damage be applied again."))
	FPropertyScale DamagePerSeconds{};
	UPROPERTY(EditDefaultsOnly, Category = "Ability | General")
	FPropertyScale Radius{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | General", meta=(Tooltip="The life span of the ability or Timer before the ability is destroyed."))
	FPropertyScale Duration{};
#pragma endregion // General

#pragma region //Projectile
	UPROPERTY(EditDefaultsOnly, Category = "Ability | Projectile", meta=(ToolTip="Ture, if the actor has mesh or is projectile type."), meta=(InlineEditConditionToggle))
	bool bIsProjectile{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Projectile", meta=(EditCondition = "bIsProjectile == true"))
	FPropertyScale ProjectileSpeed{};
#pragma endregion //Projectile
	
#pragma region //Trace
	UPROPERTY(EditDefaultsOnly, Category = "Ability | Trace", meta=(Tooltip="True, if the actor can perform a trace"))
	bool bCanTrace{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Trace", meta=(EditCondition = "bCanTrace == true",EditConditionHides))
	FPropertyScale TraceRange{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Trace", meta=(EditCondition = "bCanTrace == true",EditConditionHides))
	FPropertyScale TraceRadius{};
#pragma endregion //Trace

#pragma region //Spawn
	UPROPERTY(EditDefaultsOnly, Category = "Ability | Spawn")
	bool bCanSpawnActor{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Spawn", meta=(EditCondition="bCanSpawnActor==true",EditConditionHides))
	TSubclassOf<ABaseAbility> ActorToSpawn{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Spawn", meta=(EditCondition="bCanSpawnActor==true",EditConditionHides))
	TObjectPtr<UAbilityData> ActorAbilityData{};
#pragma endregion //Spawn

#pragma region //Effect
	UPROPERTY(EditDefaultsOnly, Category = "Ability | Effect")
	bool bCanApplyEffect{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Effect", meta=(EditCondition="bCanApplyEffect==true",EditConditionHides))
	TSubclassOf<UActorComponent> AbilityEffect{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Effect", meta=(EditCondition="bCanApplyEffect==true",EditConditionHides))
	FPropertyScale DebuffDuration{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Effect", meta=(EditCondition="bCanApplyEffect==true",EditConditionHides), meta=(Tooltip="This can also be used for damage."))
	FPropertyScale DebuffAmount{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability | Effect", meta=(EditCondition="bCanApplyEffect==true",EditConditionHides), meta=(Tooltip="Set how much debuff to apply per second."))
	FPropertyScale DebuffPerSec{};
#pragma endregion //Effect
};
