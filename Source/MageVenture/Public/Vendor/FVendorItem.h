#pragma once

#include "CoreMinimal.h"
#include "FVendorItem.generated.h"

class UDAItem;

USTRUCT(Blueprintable)
struct FVendorItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Vendor")
	UDAItem* Item{};
	UPROPERTY(BlueprintReadOnly, Category = "Vendor")
	int32 NumberOfItem{};

	bool operator==(const FVendorItem& ItemToCompare) const
	{
		return this->Item == ItemToCompare.Item;
	}
};
