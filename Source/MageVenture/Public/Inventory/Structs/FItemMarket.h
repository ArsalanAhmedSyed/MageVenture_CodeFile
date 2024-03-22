#pragma once

#include "CoreMinimal.h"
#include "FItemMarket.generated.h"

USTRUCT(BlueprintType)
struct FItemMarket
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Market")
	int32 BuyPrice{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Market")
	int32 SellPrice{};
};
