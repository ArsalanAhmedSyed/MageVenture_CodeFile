#pragma once

#include "CoreMinimal.h"
#include "FItemDIsplay.generated.h"

USTRUCT(BlueprintType)
struct FItemDisplay
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Display")
	FText Name{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Display")
	FText Description{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item | Display")
	TObjectPtr<UTexture2D> Thumbnail{};
};
