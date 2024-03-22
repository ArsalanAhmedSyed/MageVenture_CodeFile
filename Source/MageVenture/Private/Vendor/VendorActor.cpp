#include "Vendor/VendorActor.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Other/FunctionsLibrary.h"
#include "Player/Enums/EInputModeType.h"
#include "Vendor/FVendorItem.h"
#include "Vendor/PlayerTransaction.h"
#include "Vendor/Data Assets/VendorData.h"
#include "Vendor/Widget/VendorWidget.h"

AVendorActor::AVendorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	_SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	_CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(_SkeletalMeshComponent);
	
	_CapsuleComponent->SetupAttachment(GetRootComponent());
	_SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	
	_CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_CapsuleComponent->SetCollisionObjectType(ECC_WorldStatic);
	_CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AVendorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if(_VendorData == nullptr) { return; }

	_SkeletalMeshComponent->SetSkeletalMesh(_VendorData->SkeletalMesh);
	_SkeletalMeshComponent->OverrideAnimationData(_VendorData->IdleAnimation);
}

void AVendorActor::Interact(UPlayerTransaction* PlayerTransaction)
{
	if(PlayerTransaction == nullptr) { return; }
	
	_PlayerTransaction = PlayerTransaction;
	_ItemInStore = _VendorData->ItemsInStore;
	
	_PlayerController =  UGameplayStatics::GetPlayerController(this, 0);
	if(_PlayerController == nullptr) { return; }

	_VendorWidgetSpawned = CreateWidget<UVendorWidget>(_PlayerController, _VendorWidget);
	if(_VendorWidgetSpawned)
	{
		/*Events*/
		_VendorWidgetSpawned->OnExistVendor.AddDynamic(this, &ThisClass::HandleExistWidget);
		_VendorWidgetSpawned->OnRequestSellItem.AddDynamic(this, &ThisClass::HandleSellItem);
		_VendorWidgetSpawned->OnRequestBuyItem.AddDynamic(this, &ThisClass::HandleBuyItem);

		_VendorWidgetSpawned->AddToViewport();
		UFunctionsLibrary::ChangeInputMode(this, _PlayerController, EInputModeType::EIMT_UIOnly, _VendorWidgetSpawned);
		_VendorWidgetSpawned->Initialise(_PlayerTransaction->GetInventorySlots(), _ItemBought, _ItemInStore, _PlayerTransaction->GetPlayerMoney());
	}
}

void AVendorActor::HandleSellItem(FVendorItem ItemToSell, bool bIsStoreSection)
{
	if(_PlayerTransaction->CanBuy(ItemToSell.Item) == false) { return; }

	if(_PlayerTransaction->RequestBuyItem(ItemToSell.Item) == false) { return; }

	if(bIsStoreSection)
	{
		_VendorWidgetSpawned->UpdateInventoryWidget(_PlayerTransaction->GetInventorySlots());
		_VendorWidgetSpawned->UpdateMoneyWidget(_PlayerTransaction->GetPlayerMoney());
		return;
	}
	
	/*If the SellItem was request from the bought section widget,
	 *then Remove the item from the slot*/
	for (int32 i = 0; i < _ItemBought.Num(); i++)
	{
		if(_ItemBought[i].Item != ItemToSell.Item) { continue; }
		
		_ItemBought[i].NumberOfItem--;
		if(_ItemBought[i].NumberOfItem > 0) { break; }
		
		_ItemBought.RemoveAt(i);
	}

	UpdateAllWidget();
}

void AVendorActor::HandleBuyItem(FVendorItem ItemToBuy)
{
	if(ItemToBuy.Item == nullptr) { return; }

	if(_PlayerTransaction->RequestSellItem(ItemToBuy.Item) == false) { return; }
	
	bool bItemAdded{false};

#pragma region // Increment Existing Item
	/**
	 * Find item from the Bought Section and
	 * Increment the number of item in the slot
	 */
	if(_ItemBought.IsEmpty() == false)
	{
		for (FVendorItem& VendorItem : _ItemBought)
		{
			if(VendorItem.Item == ItemToBuy.Item)
			{
				//Adding Item to the slot
				VendorItem.NumberOfItem++;
				bItemAdded = true;
				break;
			}
		}
	}
#pragma endregion // Increment Existing Item
	
#pragma region // Add new item to the bought section
	/**
	 * If no matching Item was found then create
	 * a new slot for the item to be added
	 */
	if(bItemAdded == false)
	{
		FVendorItem ItemToAdd;
		ItemToAdd.Item = ItemToBuy.Item;
		ItemToAdd.NumberOfItem++;
		
		_ItemBought.AddUnique(ItemToAdd);
	}
#pragma endregion // Add new item to the bought section

	UpdateAllWidget();
}

void AVendorActor::HandleExistWidget()
{
	if(_VendorWidgetSpawned == nullptr || _VendorWidgetSpawned->IsInViewport() == false || _PlayerController == nullptr) { return; }
	
	UFunctionsLibrary::ChangeInputMode(this, _PlayerController, EInputModeType::EIMT_GameOnly, nullptr);
	_VendorWidgetSpawned->RemoveFromParent();
}

void AVendorActor::UpdateAllWidget() const
{
	_VendorWidgetSpawned->UpdateBoughtSection(_ItemBought);
	_VendorWidgetSpawned->UpdateInventoryWidget(_PlayerTransaction->GetInventorySlots());
	_VendorWidgetSpawned->UpdateMoneyWidget(_PlayerTransaction->GetPlayerMoney());
}


