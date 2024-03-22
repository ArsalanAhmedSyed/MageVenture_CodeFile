// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Enums/EAttributeType.h"
#include "Player/Structs/FAttribute.h"
#include "EnemyAttributeComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAGEVENTURE_API UEnemyAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyAttributeComponent();

	void Initialise(const TArray<FAttribute>& EnemyAttributes);

	void ResetHealth();
	void ReduceHealth(float DecrementAmount);
private:
	TArray<FAttribute> _ListOfAttributes{};
	FAttribute& SetAttributeValue(EAttributeType Attribute);

public:
	FAttribute GetAttribute(EAttributeType AttributeToGet);
};
