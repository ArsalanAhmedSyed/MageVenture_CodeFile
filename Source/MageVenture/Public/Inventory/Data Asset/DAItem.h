// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/Structs/FItemData.h"
#include "Inventory/Structs/FItemDisplay.h"
#include "Inventory/Structs/FItemMarket.h"
#include "DAItem.generated.h"

class AItemSpawner;

UCLASS(Blueprintable)
class MAGEVENTURE_API UDAItem : public UDataAsset
{
	GENERATED_BODY()

public:

	/*
	 * Item Type And Data
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Data")
	FItemData ItemData{};

	/**
	 * Spawn Info
	 */
	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle))
	bool bCanEditSpawner{false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Spawn", meta=(EditCondition = "bCanEditSpawner == true"))
	TSubclassOf<AItemSpawner> ItemSpawner{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Spawn")
	TObjectPtr<UStaticMesh> ItemSpawnMesh{};

	/**
	 * Inventory Display
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Display", meta=(ToolTip="Display Information for the item in the inventory when the item is hovered over."))
	FItemDisplay ItemDisplay{};

	/**
	 * Market Data
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Market")
	bool CanBeSold{false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Market", meta=(EditCondition = "CanBeSold == true", EditConditionHides))
	FItemMarket ItemMarket{};
};
