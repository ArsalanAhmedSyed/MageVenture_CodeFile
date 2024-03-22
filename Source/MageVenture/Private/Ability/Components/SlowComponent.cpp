
#include "Ability/Components/SlowComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

USlowComponent::USlowComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USlowComponent::Initialise(float DebuffDuration, float DebuffAmount, float DebuffRate, AActor* Owner)
{
	_DebuffAmount = DebuffAmount;
	_DebuffDuration = DebuffDuration;
	_DebuffRate = DebuffRate;
	_DebuffOwner = Owner;

	Begin();
}

float USlowComponent::GetDebuffAmount()
{
	return _DebuffAmount;
}

float USlowComponent::GetDebuffDuration()
{
	return _DebuffDuration; 
}


