#pragma once

#include "CoreMinimal.h"
#include "Ability/Actors/BaseAbility.h"
#include "Ability/Enums/AbilityTypes.h"
#include "Ability/Strcuts/FAbilitiesLevel.h"
#include "Components/ActorComponent.h"
#include "Ability/Enums/AbilitySlot.h"
#include "Ability/Strcuts/FComboAbilityData.h"
#include "Ability/Strcuts/FSingularAbilityData.h"
#include "AbilityComponent.generated.h"

DECLARE_DELEGATE_TwoParams(FOnAbilitySlotUpdateSignature, EAbilitySlot, enum ESingularAbility)
DECLARE_DELEGATE_OneParam(FOnAbilityPerformedSignature, float)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAGEVENTURE_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	FOnAbilitySlotUpdateSignature OnAbilitySlotUpdate{};
	FOnAbilityPerformedSignature OnAbilityPerformed{};
	
	UAbilityComponent();

	void Initialise(float CoolDownTimer, const TArray<FSingularAbilityData>& ListOfSingularAbilityData, const TArray<FComboAbilityData>& ListOfComboAbilityData);

	bool HasPrimaryMana(float CurrentMana);
	void SetHoverSlot(const int32& HoverIndex);
	bool SetPrimarySlot(float CurrentMana);
	void SetSecondarySlot();
	//Reset All slots except from the HoverSlot
	void ResetAllSlots();

	EAbilityType GetAttackType(float currentMana);
	void PerformAttack(EAbilityType AbilityType, float CurrentMana, const FVector& SpawnLocation, const FVector& SpawnDirection);
	
	void AbilityLevelUp(ESingularAbility AbilityToLevelUp);
	
private:
	
#pragma region //Abilities Cooldown
	bool _bPrimarySelected{false};
	bool _bPerformingAttack{false};
	FTimerHandle _THCoolDown{};
	bool _bIsOnCoolDown{false};
	float _CoolDownTimer{};
	
	void StartCoolDownTimer();
	void OnCoolDownComplete();
#pragma endregion //Abilities Cooldown

#pragma region //Abilities Slots & Types
	int32 _CurrentHoverAbility{};
	int32 _CurrentHoverIndex{0};
	
	ESingularAbility _HoverAbilitySlot{ESingularAbility::ESA_None};
	ESingularAbility _PrimaryAbilitySlot{ESingularAbility::ESA_None};
	ESingularAbility _SecondaryAbilitySlot{ESingularAbility::ESA_None};

	EComboAbility _ComboAbilityType{EComboAbility::ECA_None};
#pragma endregion //Abilities Slots & Types

	UPROPERTY()
	TArray<FSingularAbilityData> _ListOfSingularAbilities{};
	FSingularAbilityData _SingularAbilityFound{};
	UPROPERTY()
	TArray<FComboAbilityData> _ListOfComboAbilities{};
	FComboAbilityData _ComboAbilityFound{};

	FVector _SpawnLocation{};
	FVector _SpawnDirection{};
	FAbilitiesLevel _AbilitiesLevel{};
	int32 GetAbilityLevel() const;
	void PerformComboAttack(const float CurrentMana);
	void PerformSingularAttack(const float CurrentMana);
	bool FindComboAbility();
	void SpawnAbility(TSubclassOf<ABaseAbility> AbilityToSpawn, UAbilityData* AbilityData, float ConsumeManaAmount, int32 AbilityLevel) const;

	void FireOnAbilitySlotUpdate(EAbilitySlot AbilitySlotToUpdate, ESingularAbility SingularAbilityType) const;
};
