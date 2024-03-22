// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/Data Asset/DAItem.h"
#include "EnemyData.generated.h"


struct FAttribute;
/**
 * 
 */
UCLASS(Blueprintable)
class MAGEVENTURE_API UEnemyData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Visual")
	TObjectPtr<USkeletalMesh> SkeletalMesh{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Visual")
	TObjectPtr<UAnimMontage> AttackMontage{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Visual")
	TObjectPtr<UAnimMontage> DeathMontage{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Visual", meta=(ToolTip="The Blueprint Animation the Enemy character should use."))
	TObjectPtr<UClass> Animation{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | General")
	float MovementSpeed{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | General")
	float DamageAmount{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | General")
	float GiveExp{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Attributes")
	TArray<FAttribute> Attributes{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Item Drop")
	float ItemDropRate{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Item Drop", meta=(ToolTip="The List of items this enemy can drop."))
	TArray<TObjectPtr<UDAItem>> ItemDropList{};
	
};
