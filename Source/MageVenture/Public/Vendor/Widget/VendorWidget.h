// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Vendor/FVendorItem.h"
#include "VendorWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitVendorSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestSellItemSignature, FVendorItem, ItemToSell, bool, bIsStoreItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestBuyItemSignature, FVendorItem, ItemToBuy);

UCLASS(Blueprintable)
class MAGEVENTURE_API UVendorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, Category = "Vendor | Events")
	FOnExitVendorSignature OnExistVendor;
	UPROPERTY(BlueprintCallable, Category = "Vendor | Events")
	FOnRequestSellItemSignature OnRequestSellItem;
	UPROPERTY(BlueprintCallable, Category = "Vendor | Events")
	FOnRequestBuyItemSignature OnRequestBuyItem;


	/**
	 * Initialise the widget with players inventory information & Vendors store items.
	 * 
	 * @param PlayerInventory Contains player's items in the inventory data.
	 * @param ItemBought Initialise the List of Item in the bought section.
	 * @param ItemInStore Initialise the List of item in the store section.
	 * @param PlayerMoney Contains player's Money.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Vendor Data | Events")
	void Initialise(const TArray<FInventorySlot>& PlayerInventory, const TArray<FVendorItem>& ItemBought, const TArray<FVendorItem>& ItemInStore, int32 PlayerMoney);

	/**
	 * Update Bought section with new item added to the list.
	 * 
	 * @param UpdatedSlots Contains bought section item list
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Vendor Data | Events")
	void UpdateBoughtSection(const TArray<FVendorItem>& UpdatedSlots);

	/**
	 * Update the inventory widget after the transaction. 
	 * 
	 * @param UpdatedSlots Updated player's inventory slots.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Vendor Data | Events")
	void UpdateInventoryWidget(const TArray<FInventorySlot>& UpdatedSlots);

	/**
	 * Update the Money player currently has after making a transaction.
	 * 
	 * @param DisplayMoney Contains the Money player has.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Vendor Data | Events")
	void UpdateMoneyWidget(int32 DisplayMoney);	
};
