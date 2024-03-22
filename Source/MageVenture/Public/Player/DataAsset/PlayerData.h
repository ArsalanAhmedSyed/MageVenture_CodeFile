// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Strcuts/FComboAbilityData.h"
#include "Ability/Strcuts/FSingularAbilityData.h"
#include "Engine/DataAsset.h"
#include "Player/Structs/FAttribute.h"
#include "PlayerData.generated.h"


UCLASS()
class MAGEVENTURE_API UPlayerData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Player | Montages")
	TObjectPtr<UAnimMontage> SingularAttackMontage{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Montages")
	TObjectPtr<UAnimMontage> ComboAttackMontage{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Montages")
	TObjectPtr<UAnimMontage> _DeathMontage{};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Inventory", meta=(ToolTip="Set Number of inventory slots."))
	int32 _InventorySize{10};
	
	UPROPERTY(EditDefaultsOnly, Category = "Player | Movement", meta=(ToolTip="Set movement speed for walking."))
	float _WalkSpeed{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Movement", meta=(ToolTip="Set movement speed for Sprinting"))
	float _SpringSpeed{};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Casting", meta=(ToolTip="Camera Zoom in lenght"))
	float _CastingTargetArmLength{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Casting", meta=(ToolTip="Camera position around the player"))
	FVector _CastingSocketPosition{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Casting", meta=(ToolTip="Set how fast the camera should adjust."))
	float _ChangeCameraLerpRate{0.1f};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Stats")
	FPropertyScale EXPRequired{};
	
	UPROPERTY(EditDefaultsOnly, Category = "Player | Stats",meta=(ToolTip="Set characters base stats."))
	TArray<FAttribute> _ListOfAttributes{};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Abilities", meta=(ToolTip="Set how long the ability should be on cooldown after casting."))
	float _CoolDownTimer{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Abilities", meta=(ToolTip="Add list of abilities related to singular or primary element ability"))
	TArray<FSingularAbilityData> _ListOfSingularAbility{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Abilities", meta=(ToolTip="Add list of abilities related to Combo ability"))
	TArray<FComboAbilityData> _ListOfComboAbility{};
	
};
