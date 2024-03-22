// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "VendorActor.generated.h"

class UVendorData;
class UCapsuleComponent;
class UVendorWidget;
struct FVendorItem;

UCLASS()
class MAGEVENTURE_API AVendorActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	AVendorActor();

protected:
	virtual void Interact(UPlayerTransaction* PlayerTransaction) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
private:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> _SkeletalMeshComponent{};
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> _CapsuleComponent{};
	
	UPROPERTY()
	UPlayerTransaction* _PlayerTransaction{};
	UPROPERTY()
	APlayerController* _PlayerController{};
	
	UPROPERTY(EditAnywhere, Category = "Vendor")
	TObjectPtr<UVendorData> _VendorData{};
	
	UPROPERTY()
	TArray<FVendorItem> _ItemInStore{};
	UPROPERTY()
	TArray<FVendorItem> _ItemBought{};
	UFUNCTION()
	void HandleSellItem(FVendorItem ItemToSell, bool bIsStoreSection);
	UFUNCTION()
	void HandleBuyItem(FVendorItem ItemToBuy);
	void UpdateAllWidget() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Vendor | Widget")
	TSubclassOf<UVendorWidget> _VendorWidget{};
	TObjectPtr<UVendorWidget> _VendorWidgetSpawned{};
	
	UFUNCTION()
	void HandleExistWidget();
};
