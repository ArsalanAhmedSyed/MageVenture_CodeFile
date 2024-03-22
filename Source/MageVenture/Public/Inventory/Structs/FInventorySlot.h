#pragma once

#include "CoreMinimal.h"
#include "Inventory/Data Asset/DAItem.h"
#include"FInventorySlot.generated.h"

USTRUCT(Blueprintable)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta=(ToolTip = "Current number of items stored in the slot."))
	int32 _CurrentStackSize{};
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UDAItem> _Item{};

	bool operator==(const FInventorySlot& ItemToCompare) const 
	{
		return this->_Item == ItemToCompare._Item;
	}
};


