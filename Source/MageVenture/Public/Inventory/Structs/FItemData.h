#pragma once

#include "CoreMinimal.h"
#include "FGearData.h"
#include "Items/Potions/Structs/FPotionData.h"
#include "Inventory/Enums/EItemType.h"
#include "FItemData.generated.h"


USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Data")
	EItemType ItemType{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Data", meta=(ToolTip="Set how much an item can stack in one slot."))
	int32 MaxStackSize{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Potion", meta=(EditCondition= "ItemType == EItemType::EIT_Potion", EditConditionHides))
	FPotionData PotionData{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Gear", meta=(EditCondition= "ItemType == EItemType::EIT_Gear", EditConditionHides))
	FGearData GearData{};	
};