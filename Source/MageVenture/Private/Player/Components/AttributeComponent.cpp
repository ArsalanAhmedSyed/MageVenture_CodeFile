
#include "Player/Components/AttributeComponent.h"
#include "Inventory/Structs/FGearStats.h"
#include "Items/Potions/Structs/FPotionData.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::Initialise(const TArray<FAttribute>& ListOfAttributes, FPropertyScale EXPRequired)
{
	_EXPRequired = EXPRequired;
	_ListOfAttributes = ListOfAttributes;
	for (FAttribute& Attribute : _ListOfAttributes)
	{
		Attribute.Initialise();
	}
}

void UAttributeComponent::GainExp(float ExpAmount)
{
	_CurrentExp += ExpAmount;
	const float EXPRequired = _EXPRequired.GetValueAtLevel(_CurrentLevel);
	if(_CurrentExp >= EXPRequired)
	{
		//Set exceeded amount of exp back to current level, if there is any.
		_CurrentExp -= EXPRequired;
		_CurrentLevel++;
		
		for (FAttribute& Attribute : _ListOfAttributes)
		{
			Attribute.LevelUpProperties(_CurrentLevel);
		}
		
		OnCharacterLevelUp.ExecuteIfBound(_CurrentLevel, _ListOfAttributes);
	}
}

void UAttributeComponent::ReduceMana(float ConsumeAmount)
{
	float& CurrentManaValue = SetAttributeValue(EAttributeType::EAT_Mana).CurrentValue;
	CurrentManaValue = FMath::Max(CurrentManaValue - ConsumeAmount, 0);
	FireUpdateMana();
}

void UAttributeComponent::ReduceHealth(float DamageAmount)
{
	float& CurrentHealthValue = SetAttributeValue(EAttributeType::EAT_Health).CurrentValue;
	CurrentHealthValue = FMath::Max(CurrentHealthValue - DamageAmount, 0);
	FireUpdateHealth();
}

void UAttributeComponent::ResetManaHealthValue()
{
	SetAttributeValue(EAttributeType::EAT_Health).CurrentValue = SetAttributeValue(EAttributeType::EAT_Health).MaxValue;
	SetAttributeValue(EAttributeType::EAT_Mana).CurrentValue = SetAttributeValue(EAttributeType::EAT_Mana).MaxValue;
}

bool UAttributeComponent::ConsumeItem(FPotionData PotionData)
{
	bool bPotionConsumed{false};
	
	switch (PotionData.PotionType)
	{
	case EPotionType::EPT_HealthRegen:
		 if((ApplyConsumePotion(PotionData.EffectValue, SetAttributeValue(EAttributeType::EAT_Health))))
		 {
		 	FireUpdateHealth();
			bPotionConsumed = true;
		 }
	break;
	case EPotionType::EPT_ManaRegen:
		if(ApplyConsumePotion(PotionData.EffectValue, SetAttributeValue(EAttributeType::EAT_Mana)))
		{
			FireUpdateMana();
			bPotionConsumed = true;
		}
	break;
	default: break;
	}

	return bPotionConsumed;
}

FAttribute& UAttributeComponent::SetAttributeValue(EAttributeType AttributeToGet)
{
	for (FAttribute& Attribute : _ListOfAttributes)
	{
		if(Attribute.Attribute == AttributeToGet)
		{
			return Attribute;
		}
	}
	
	throw std::runtime_error("Attribute not found");
}

bool UAttributeComponent::ApplyConsumePotion(float PotionValue, FAttribute& Attribute)
{
	bool bPotionConsumed;
	
	//Return if health is full
	if(Attribute.CurrentValue == Attribute.MaxValue) { return bPotionConsumed = false; }
	
	Attribute.CurrentValue  +=  PotionValue;
	Attribute.CurrentValue  = FMath::Clamp(Attribute.CurrentValue ,0.f , Attribute.MaxValue);

	return bPotionConsumed = true;
}

void UAttributeComponent::GearUnEquipped(const FGearStats RemoveGearStats)
{
	SetAttributeValue(EAttributeType::EAT_Attack).MaxValue	-= RemoveGearStats.AttackStat;
	SetAttributeValue(EAttributeType::EAT_Defence).MaxValue -= RemoveGearStats.DefenceStat;
	SetAttributeValue(EAttributeType::EAT_Health).MaxValue	-= RemoveGearStats.HealthStat;
	SetAttributeValue(EAttributeType::EAT_Mana).MaxValue	-= RemoveGearStats.ManaStat;

	SetAttributeValue(EAttributeType::EAT_Health).CurrentValue	-= RemoveGearStats.HealthStat;
	SetAttributeValue(EAttributeType::EAT_Mana).CurrentValue	-= RemoveGearStats.ManaStat;
	
	//Fire Event calls to update the UI
	FireUpdateAttributes();
}

void UAttributeComponent::GearEquipped(const FGearStats AddGearStats)		
{
	//Apply new Gear stats
	SetAttributeValue(EAttributeType::EAT_Attack).MaxValue	+= AddGearStats.AttackStat;
	SetAttributeValue(EAttributeType::EAT_Defence).MaxValue += AddGearStats.DefenceStat;
	SetAttributeValue(EAttributeType::EAT_Health).MaxValue	+= AddGearStats.HealthStat;
	SetAttributeValue(EAttributeType::EAT_Mana).MaxValue	+= AddGearStats.ManaStat;

	SetAttributeValue(EAttributeType::EAT_Health).CurrentValue	+= AddGearStats.HealthStat;
	SetAttributeValue(EAttributeType::EAT_Mana).CurrentValue	+= AddGearStats.ManaStat;

	//Fire Event calls to update the UI
	FireUpdateAttributes();
}

FAttribute UAttributeComponent::GetAttribute(EAttributeType AttributeToGet)
{
	for (const FAttribute Attribute : _ListOfAttributes)
	{
		if(Attribute.Attribute == AttributeToGet)
		{
			return Attribute;
		}
	}
	
	throw std::runtime_error("Attribute not found");
}

void UAttributeComponent::FireUpdateAttributes() const
{
	OnAttributeUpdated.ExecuteIfBound(_ListOfAttributes);
}

void UAttributeComponent::FireUpdateHealth()
{
	OnHealthValueUpdate.ExecuteIfBound(SetAttributeValue(EAttributeType::EAT_Health).CurrentValue);
}

void UAttributeComponent::FireUpdateMana()
{
	OnManaValueUpdate.ExecuteIfBound(SetAttributeValue(EAttributeType::EAT_Mana).CurrentValue);
}

