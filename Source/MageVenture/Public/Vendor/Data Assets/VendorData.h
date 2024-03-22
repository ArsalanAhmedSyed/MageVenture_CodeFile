#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VendorData.generated.h"

struct FVendorItem;
class UDAItem;

UCLASS()
class MAGEVENTURE_API UVendorData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Vendor | Visual")
	TObjectPtr<USkeletalMesh> SkeletalMesh{};

	UPROPERTY(EditDefaultsOnly, Category = "Vendor | Visual")
	TObjectPtr<UAnimSequence> IdleAnimation{};

	UPROPERTY(EditDefaultsOnly, Category = "Vendor | Items", meta=(Tooltip = "Select the list of Items the vendor should sell."))
	TArray<FVendorItem> ItemsInStore{};
};
