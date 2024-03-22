// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animation/AresAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AresCharacter.h"

void UAresAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	_AresCharacter = Cast<AAresCharacter>(TryGetPawnOwner());
}

void UAresAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(_AresCharacter == nullptr) { return; }

	_Speed = _AresCharacter->GetVelocity().Size();
	_bIsFalling = _AresCharacter->GetCharacterMovement()->IsFalling();
	_bIsMoving = _AresCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	_CurrentCharacterState = _AresCharacter->GetCharacterState();
}
