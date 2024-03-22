#pragma once

#include "CoreMinimal.h"
#include "Items/Potions/Enum/EPotionType.h"
#include "FPotionData.generated.h"

USTRUCT(Blueprintable)
struct FPotionData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Potion")
	EPotionType PotionType{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Potion", meta=(Tooltip ="Set how much should this potion regenerate."))
	int32 EffectValue{};
	
};
