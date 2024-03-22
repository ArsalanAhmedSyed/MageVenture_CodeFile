// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Components/EffectComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Other/FunctionsLibrary.h"


void UEffectComponent::Initialise(float DebuffDuration, float DebuffAmount, float DebuffRate, AActor* Owner)
{
	_DebuffAmount = DebuffAmount;
	_DebuffDuration = DebuffDuration;
	_DebuffRate = DebuffRate;
	_DebuffOwner = Owner;

	Begin();

	GetWorld()->GetTimerManager().SetTimer(TH_Destroyer, this, &ThisClass::OnComponentDurationEnd, _DebuffDuration, true);
}

void UEffectComponent::ApplyEffectDamage() const
{
	const float FinalDamage = UFunctionsLibrary::CalculateDamage(_DebuffAmount, _DebuffOwner, GetOwner());
	AController* Controller = _DebuffOwner->GetInstigatorController();
	
	UGameplayStatics::ApplyDamage(GetOwner(), FinalDamage, Controller, _DebuffOwner, UDamageType::StaticClass());
}
