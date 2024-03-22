#pragma once

#include "CoreMinimal.h"
#include "Inventory/Enums/EGearType.h"
#include "FGearSlot.generated.h"

class UDAItem;

USTRUCT(Blueprintable)
struct FGearSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Item Data | Gear")
	EGearType _GearType{};

	UPROPERTY(BlueprintReadOnly, Category = "Item Data | Gear")
	UDAItem* _GearItem{};

	bool operator==(const FGearSlot& ItemToCompare) const 
	{
		return this->_GearItem == ItemToCompare._GearItem;
	}
};
