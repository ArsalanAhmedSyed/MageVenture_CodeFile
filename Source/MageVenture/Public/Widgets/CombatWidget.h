// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "..\Player\Structs\FAttribute.h"
#include "CombatWidget.generated.h"

UCLASS(BlueprintType)
class MAGEVENTURE_API UCombatWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/**
	 * Pass information to Key Stats Widget, to update the Max and current value of Mana & Health
	 * 
	 * @param HealthAttribute Contains Health Max & Current values
	 * @param ManaAttribute Contains Mana Max & Current values
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player | Attributes")
	void UpdateAttributes(const FAttribute& HealthAttribute,const FAttribute& ManaAttribute);
	void GetUpdatedAttributes(const TArray<FAttribute>& AllAttributes);

	/**
	 *	Update the level displayed
	 * 
	 * @param level Current level of the player.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player | Attributes")
	void UpdateLevelDisplay(int32 level);

	/**
	 * Pass information to Ability Slot Widget, to update the slots texture icon
	 * 
	 * @param AbilitySlot Slot to update
	 * @param SingularAbilityType The ability to display or update to
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player | Ability")
	void UpdateAbilitySlotTexture(EAbilitySlot AbilitySlot, ESingularAbility SingularAbilityType);

	/**
	 * Pass information to Key Stats Widget, to display current amount of Mana player has
	 * 
	 * @param UpdatedManaValue Players current mana value
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player | Attributes")
	void UpdateManaDisplay(float UpdatedManaValue);

	/**
	 * Pass information to Key Stats Widget, to display current amount of Health player has
	 * 
	 * @param UpdateHealthValue Players current health value
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player | Attributes")
	void UpdateHealthDisplay(float UpdateHealthValue);

	/**
	 * Should be used to enable / disable crosshair visibility
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void ToggleCrosshair();
};
