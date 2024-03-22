
#include "AIEnemy/Component/EnemyAttributeComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UEnemyAttributeComponent::UEnemyAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyAttributeComponent::Initialise(const TArray<FAttribute>& EnemyAttributes)
{
	if(EnemyAttributes.IsEmpty() == true)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Enemy Attribute list is empty! \n Fill in the enemy data asset")));
		return;
	}
	
	_ListOfAttributes = EnemyAttributes;

	for (FAttribute& Attribute : _ListOfAttributes)
	{
		Attribute.Initialise();
	}
}

FAttribute& UEnemyAttributeComponent::SetAttributeValue(EAttributeType AttributeToGet)
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

FAttribute UEnemyAttributeComponent::GetAttribute(EAttributeType AttributeToGet)
{
	for (const FAttribute Attribute : _ListOfAttributes)
	{
		if(Attribute.Attribute == AttributeToGet)
		{
			return Attribute;
		}
	}
	return FAttribute();
}

void UEnemyAttributeComponent::ResetHealth()
{
	if(_ListOfAttributes.IsEmpty() == true) {return;}
	SetAttributeValue(EAttributeType::EAT_Health).CurrentValue = SetAttributeValue(EAttributeType::EAT_Health).MaxValue;
}

void UEnemyAttributeComponent::ReduceHealth(float DecrementAmount)
{
	if(_ListOfAttributes.IsEmpty() == true) {return;}
	
	float& CurrentHealthValue = SetAttributeValue(EAttributeType::EAT_Health).CurrentValue;
	CurrentHealthValue = FMath::Max(CurrentHealthValue - DecrementAmount, 0);
}


