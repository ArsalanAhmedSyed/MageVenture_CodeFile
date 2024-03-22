// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Player/Enums/ECharacterState.h"
#include "AresAnimInstance.generated.h"


class AAresCharacter;

UCLASS()
class MAGEVENTURE_API UAresAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Player | Movement")
	float _Speed{};
	UPROPERTY(BlueprintReadOnly, Category = "Player | Movement", DisplayName="IsFalling")
	bool _bIsFalling{};
	UPROPERTY(BlueprintReadOnly, Category = "Player | Movement", DisplayName="IsMoving")
	bool _bIsMoving{};
	UPROPERTY(BlueprintReadOnly, Category = "Player | Movement", DisplayName="Character State")
	ECharacterState _CurrentCharacterState{};

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<AAresCharacter> _AresCharacter{};
private:
};
