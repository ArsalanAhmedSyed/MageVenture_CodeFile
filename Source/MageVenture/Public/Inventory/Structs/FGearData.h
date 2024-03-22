#pragma once

#include "CoreMinimal.h"
#include "FGearStats.h"
#include "Inventory/Enums/EGearType.h"
#include "FGearData.generated.h"

USTRUCT(Blueprintable)
struct FGearData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Gear" )
	EGearType GearType{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | Gear Stats")
	FGearStats Attributes{};
};
