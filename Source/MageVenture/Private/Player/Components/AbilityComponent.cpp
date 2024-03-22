

#include "Player/Components/AbilityComponent.h"
#include "Ability/Data/AbilityData.h"
#include "Ability/Enums/AbilitySlot.h"
#include "Ability/Strcuts/FComboAbilityData.h"
#include "Ability/Strcuts/FSingularAbilityData.h"
#include "Kismet/GameplayStatics.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAbilityComponent::Initialise(float CoolDownTimer, const TArray<FSingularAbilityData>& ListOfSingularAbilityData, const TArray<FComboAbilityData>& ListOfComboAbilityData)
{
	_CoolDownTimer = CoolDownTimer;
	_ListOfSingularAbilities = ListOfSingularAbilityData;
	_ListOfComboAbilities = ListOfComboAbilityData;

	_HoverAbilitySlot = _ListOfSingularAbilities[_CurrentHoverAbility].SingularAbilityType;
	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Hover, _HoverAbilitySlot);
}

void UAbilityComponent::SetHoverSlot(const int32& HoverIndex)
{
	//Increment to next slot
	_CurrentHoverIndex += HoverIndex;

	/*Keeping the hover index value between min and max*/
	const int32 MaxList = _ListOfSingularAbilities.Num();
	if(_CurrentHoverIndex > MaxList - 1)
	{
		_CurrentHoverIndex = 0;
	}
	else if(_CurrentHoverIndex < 0)
	{
		_CurrentHoverIndex = MaxList - 1;
	}

	_HoverAbilitySlot = _ListOfSingularAbilities[_CurrentHoverIndex].SingularAbilityType;

	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Hover, _HoverAbilitySlot);
}

bool UAbilityComponent::SetPrimarySlot(float CurrentMana)
{
	
	if(_bIsOnCoolDown == true || HasPrimaryMana(CurrentMana) == false || _bPrimarySelected == true) { return false; }
	
	_bPrimarySelected = true;
	_PrimaryAbilitySlot = _HoverAbilitySlot;
	
	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Primary, _PrimaryAbilitySlot);

	return true;
}

bool UAbilityComponent::HasPrimaryMana(float CurrentMana)
{
	const float ManaToConsume = _ListOfSingularAbilities[_CurrentHoverIndex].AbilityData->ManaConsumeAmount;
	return (ManaToConsume <= CurrentMana);
}


void UAbilityComponent::SetSecondarySlot()
{
	if(_PrimaryAbilitySlot == ESingularAbility::ESA_None || _bIsOnCoolDown == true || _HoverAbilitySlot == _PrimaryAbilitySlot) { return; }

	_SecondaryAbilitySlot = _HoverAbilitySlot;
	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Secondary,_SecondaryAbilitySlot);
}

void UAbilityComponent::ResetAllSlots()
{
	if(_bPrimarySelected == false) { return; }
	
	_bPrimarySelected = false;
	_ComboAbilityType = EComboAbility::ECA_None;
	_PrimaryAbilitySlot = ESingularAbility::ESA_None;
	_SecondaryAbilitySlot = ESingularAbility::ESA_None;

	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Primary, _PrimaryAbilitySlot);
	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Secondary, _SecondaryAbilitySlot);
}

EAbilityType UAbilityComponent::GetAttackType(float currentMana)
{
	if(_bIsOnCoolDown == true  || _PrimaryAbilitySlot == ESingularAbility::ESA_None) { return EAbilityType::EAT_None; }

	if(_SecondaryAbilitySlot != ESingularAbility::ESA_None) 
	{
		if(FindComboAbility() == false)
		{
			ResetAllSlots();
			return EAbilityType::EAT_None;
		}
		
		for (const auto Element : _ListOfComboAbilities)
		{
			if(Element.ComboAbilityType == _ComboAbilityType)
			{
				if(Element.AbilityData->ManaConsumeAmount <= currentMana)
				{
					_ComboAbilityFound = Element;
					return EAbilityType::EAT_Combo;
				}
				else
				{
					ResetAllSlots();
					return EAbilityType::EAT_None;
				}
			}
		}
	}

	/*Check for singular ability if Secondary ability slot was empty*/
	for (const auto Element : _ListOfSingularAbilities)
	{
		if(Element.SingularAbilityType == _PrimaryAbilitySlot)
		{
			_SingularAbilityFound = Element;
			return (Element.AbilityData->ManaConsumeAmount <= currentMana) ? EAbilityType::EAT_Singular : EAbilityType::EAT_None; 
		}
	}

	ResetAllSlots();
	return EAbilityType::EAT_None;
}
bool UAbilityComponent::FindComboAbility()
{
	if(_PrimaryAbilitySlot == ESingularAbility::ESA_Fire && _SecondaryAbilitySlot == ESingularAbility::ESA_Wind)
	{
		_ComboAbilityType = EComboAbility::ECA_ScatterFlame;
		return true;
	}
	else if(_PrimaryAbilitySlot == ESingularAbility::ESA_Ice && _SecondaryAbilitySlot == ESingularAbility::ESA_Wind)
	{
		_ComboAbilityType = EComboAbility::ECA_ScatterIce;
		return true;
	}
	else if(_PrimaryAbilitySlot == ESingularAbility::ESA_Wind && _SecondaryAbilitySlot == ESingularAbility::ESA_Fire)
	{
		_ComboAbilityType = EComboAbility::ECA_FlamingTornado;
		return true;
	}
	else if(_PrimaryAbilitySlot == ESingularAbility::ESA_Lightning && _SecondaryAbilitySlot == ESingularAbility::ESA_Ice)
	{
		_ComboAbilityType = EComboAbility::ECA_BurstLightning;
		return true;
	}

	return false;
}

void UAbilityComponent::PerformAttack(EAbilityType AbilityType, float CurrentMana, const FVector& SpawnLocation, const FVector& SpawnDirection)
{
	if(AbilityType == EAbilityType::EAT_None) { return; }

	_SpawnLocation = SpawnLocation;
	_SpawnDirection = SpawnDirection;
	
	(AbilityType == EAbilityType::EAT_Singular) ? PerformSingularAttack(CurrentMana) : PerformComboAttack(CurrentMana);
}

void UAbilityComponent::PerformSingularAttack(const float CurrentMana)
{
	if(_SingularAbilityFound.AbilityData == nullptr) { return; }
	
	const float ManaToConsume = _SingularAbilityFound.AbilityData->ManaConsumeAmount;
	
	const int32 AbilityLevel = GetAbilityLevel();
	if(AbilityLevel != INDEX_NONE)
	{
		SpawnAbility(_SingularAbilityFound.SpawnAbilityActor, _SingularAbilityFound.AbilityData, ManaToConsume, AbilityLevel);
	}
				
	StartCoolDownTimer();
	
	_PrimaryAbilitySlot = ESingularAbility::ESA_None;
	FireOnAbilitySlotUpdate(EAbilitySlot::EAS_Primary, _PrimaryAbilitySlot);
}

void UAbilityComponent::PerformComboAttack(const float CurrentMana)
{
	if(_ComboAbilityFound.AbilityData == nullptr) { return; }

	const float ManaToConsume = _ComboAbilityFound.AbilityData->ManaConsumeAmount;

	const int32 AbilityLevel = GetAbilityLevel();
	if(AbilityLevel != INDEX_NONE)
	{
		SpawnAbility(_ComboAbilityFound.SpawnAbilityActor, _ComboAbilityFound.AbilityData, ManaToConsume, AbilityLevel);
	}
	//Except from Hover slot, reset all the slots.
	ResetAllSlots();
	
	StartCoolDownTimer();
}

void UAbilityComponent::SpawnAbility(TSubclassOf<ABaseAbility> AbilityToSpawn, UAbilityData* AbilityData, float ConsumeManaAmount, int32 AbilityLevel) const
{
	if(AbilityToSpawn)
	{
		#pragma region // Spawning Parameters
		AActor* Owner = GetOwner();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = Owner->GetInstigator();
		SpawnParameters.Owner = Owner;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		FTransform SpawnTransform{};
		SpawnTransform.SetLocation(_SpawnLocation + GetOwner()->GetActorForwardVector() * 50.f);
		SpawnTransform.SetRotation(_SpawnDirection.Rotation().Quaternion());
		#pragma endregion // Spawning Parameters

		ABaseAbility* SpawnedActor = GetWorld()->SpawnActorDeferred<ABaseAbility>(AbilityToSpawn, SpawnTransform, Owner, Owner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		SpawnedActor->Initialise(AbilityData, AbilityLevel);
		UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);

		OnAbilityPerformed.ExecuteIfBound(ConsumeManaAmount);
	}
}

void UAbilityComponent::StartCoolDownTimer()
{
	_bIsOnCoolDown = true;
	GetWorld()->GetTimerManager().SetTimer(_THCoolDown, this, &ThisClass::OnCoolDownComplete, _CoolDownTimer, false);
}

void UAbilityComponent::OnCoolDownComplete()
{
	_bIsOnCoolDown = false;
	_bPrimarySelected = false;
}

void UAbilityComponent::AbilityLevelUp(ESingularAbility AbilityToLevelUp)
{
	switch(AbilityToLevelUp)
	{
	case ESingularAbility::ESA_Fire:
		_AbilitiesLevel.Fire++;
		break;
	case ESingularAbility::ESA_Ice:
		_AbilitiesLevel.Ice++;
		break;
	case ESingularAbility::ESA_Lightning:
		_AbilitiesLevel.Lightning++;
		break;
	case ESingularAbility::ESA_Wind:
		_AbilitiesLevel.Wind++;
		break;
	default:
		break;
	}
}

int32 UAbilityComponent::GetAbilityLevel() const
{
	switch(_PrimaryAbilitySlot)
	{
	case ESingularAbility::ESA_Fire:
		return _AbilitiesLevel.Fire;
	case ESingularAbility::ESA_Ice:
		return _AbilitiesLevel.Ice;
	case ESingularAbility::ESA_Lightning:
		return _AbilitiesLevel.Lightning;
	case ESingularAbility::ESA_Wind:
		return _AbilitiesLevel.Wind;
		default:
			break;
	}

	return INDEX_NONE;
}

void UAbilityComponent::FireOnAbilitySlotUpdate(EAbilitySlot AbilitySlotToUpdate, ESingularAbility SingularAbilityType) const
{
	OnAbilitySlotUpdate.ExecuteIfBound(AbilitySlotToUpdate, SingularAbilityType);
}
