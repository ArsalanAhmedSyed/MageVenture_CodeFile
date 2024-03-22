// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeadWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestRespawnSignature);

UCLASS(Blueprintable)
class MAGEVENTURE_API UDeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, Category = "Player")
	FOnRequestRespawnSignature OnRequestRespawn;
};
