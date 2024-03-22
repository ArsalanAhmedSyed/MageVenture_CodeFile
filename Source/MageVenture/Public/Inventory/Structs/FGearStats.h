#pragma once

#include "CoreMinimal.h"
#include "FGearStats.generated.h"

USTRUCT(BlueprintType)
struct FGearStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Gear | Stats")
	int32 HealthStat{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Gear | Stats")
	int32 ManaStat{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Gear | Stats")
	int32 DefenceStat{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Gear | Stats")
	int32 AttackStat{};
};
