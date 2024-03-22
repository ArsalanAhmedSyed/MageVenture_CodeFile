
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Actor/ItemSpawner.h"
#include "Inventory/Structs/FItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Vendor/PlayerTransaction.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	_GearSlots.SetNum(_GearSlotSize);
	_PlayerTransaction = NewObject<UPlayerTransaction>(this, UPlayerTransaction::StaticClass() ,TEXT("Player Transaction"));
}

void UInventoryComponent::Initialise(int32 InventorySize)
{
	_InventorySlots.SetNum(InventorySize);
	//Initialise Each Gear slot with specific Type
	_GearSlots[0]._GearType = EGearType::EGT_Head;
	_GearSlots[1]._GearType = EGearType::EGT_Chest;
	_GearSlots[2]._GearType = EGearType::EGT_Leg;

	if(_PlayerTransaction)
		_PlayerTransaction->Initialise(this);
}

#pragma region //Inventory  Related Functions

bool UInventoryComponent::CanAddItem(UDAItem* ItemToAdd)
{
	bool bEmptySlotFound{false};
	
	for(int32 i = 0; i < _InventorySlots.Num(); i++)
	{
		if(_InventorySlots[i]._Item == ItemToAdd)
		{
			//If the slot has space then return true
			if(_InventorySlots[i]._Item->ItemData.MaxStackSize < GetCurrentStackSize(i))
			{
				return true;
			}
			
		}
		
		if(bEmptySlotFound == false && GetCurrentStackSize(i) <= 0)
		{
			bEmptySlotFound = true;
		}
	}

	return bEmptySlotFound;
}

bool UInventoryComponent::AddItem(UDAItem* Item)
{
	bool bMatchFound{false};
	for(int32 i = 0; i < _InventorySlots.Num(); i++)
	{
		if(_InventorySlots[i]._Item == Item)
		{
			if(GetCurrentStackSize(i) < _InventorySlots[i]._Item->ItemData.MaxStackSize)
			{
				//Add the Item to the slot 
				_InventorySlots[i]._CurrentStackSize++;
				bMatchFound = true;
				break;
			}
		}
	}

	if(bMatchFound == true) { return true; }
	
	//If no matching item found, then add the item in to new slot
	 return AddNewItem(Item);
}

bool UInventoryComponent::AddNewItem(UDAItem* Item)
{
	bool bItemAdded{false};
	
	for(int32 i = 0; i < _InventorySlots.Num(); i++)
	{
		if(GetCurrentStackSize(i) == 0)
		{
			//Add Item in the slot
			_InventorySlots[i]._Item = Item;
			_InventorySlots[i]._CurrentStackSize++;
			bItemAdded = true;
			break;
		}
	}
	return bItemAdded;
}

bool UInventoryComponent::RemoveItem(int32 SlotIndex)
{
	bool bItemRemoved{false};
	
	if(SlotIndex > _InventorySlots.Num()) { return bItemRemoved; }

	int32& ItemStackSize = GetCurrentStackSize(SlotIndex);

	if(ItemStackSize > 0)
	{
		ItemStackSize--;
		if(ItemStackSize <= 0)
		{
			_InventorySlots[SlotIndex]._Item = nullptr;
		}

		bItemRemoved = true;
	}

	return  bItemRemoved;
}

bool UInventoryComponent::RemoveItem(UDAItem* SellItem)
{
	if(SellItem == nullptr) { return false; }

	bool bItemRemoved{false};
	
	for ( int32 i = 0; i < _InventorySlots.Num(); i++)
	{
		if(_InventorySlots[i]._Item == SellItem)
		{
			if(RemoveItem(i))
			{
				bItemRemoved = true;
				break;
			}
		}
	}

	return bItemRemoved;
}

void UInventoryComponent::DropItem(int32 SlotIndex)
{
	if(SlotIndex > _InventorySlots.Num()) { return; }

	if(GetCurrentStackSize(SlotIndex) > 0)
	{
		UDAItem* ItemData = _InventorySlots[SlotIndex]._Item;

		#pragma region // Spawn Parameter
		const ESpawnActorCollisionHandlingMethod CollisionHandlingMethod =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AActor* Owner = GetOwner();
		APawn* Instigator = Owner->GetInstigatorController()->GetPawn();
		FTransform Transform;
		Transform.SetLocation( Owner->GetActorLocation() + (Owner->GetActorForwardVector() * _SpawnDistance) );
		const TSubclassOf<AItemSpawner> ActorToSpawn = ItemData->ItemSpawner;
		#pragma endregion // Spawn Parameter
		
		const TObjectPtr<AItemSpawner> ActorSpawned = GetWorld()->SpawnActorDeferred<AItemSpawner>(ActorToSpawn,  Transform, Owner, Instigator, CollisionHandlingMethod);
		ActorSpawned->Initialise(ItemData);
		UGameplayStatics::FinishSpawningActor(ActorSpawned, Transform);

		if(RemoveItem(SlotIndex))
		{
			FireUpdateInventoryUI();
		}
	}
}

void UInventoryComponent::SwapItems(int32 SourceSlot, int32 CurrentSlot)
{
	_InventorySlots.Swap(SourceSlot, CurrentSlot);
	FireUpdateInventoryUI();
}

int32& UInventoryComponent::GetCurrentStackSize(int32 SlotIndex)
{
	return _InventorySlots[SlotIndex]._CurrentStackSize;
}
#pragma endregion //Inventory  Related Functions

#pragma region //Gear Related Functions
void UInventoryComponent::EquipGear(UDAItem* GearToEquip)
{
	if(GearToEquip == nullptr) { return; }
	
	for (FGearSlot& GearSlot : _GearSlots)
	{
		if(GearSlot._GearType == GearToEquip->ItemData.GearData.GearType)
		{
			UDAItem* OldGear{};
			OldGear = GearSlot._GearItem;
			
			GearSlot._GearItem = GearToEquip;
			//Add old gear back to inventory & Remove the new gear item equipped.
			UnEquipGear(OldGear, GearToEquip);
			FireUpdateGearEquipped(GearSlot);
			break;
		}
	}
}

void UInventoryComponent::UnEquipGear(UDAItem* ItemToInventory, UDAItem* ItemToRemove)
{
	bool bItemRemoved{false};
	bool bItemAdded{false};

	bItemRemoved = RemoveItem(ItemToRemove);

	//If old gear item was not empty
	if(ItemToInventory != nullptr)
	{
		//Add old equipped gear back to inventory
		bItemAdded = AddItem(ItemToInventory);
		
		//Fire Event to remove old gear stats
		const FGearStats GearStats = ItemToInventory->ItemData.GearData.Attributes;
		OnGearUnEquipped.ExecuteIfBound(GearStats);
	}

	if(bItemRemoved || bItemAdded)
	{
		FireUpdateInventoryUI();
	}
}

void UInventoryComponent::FindGear(EGearType GearType)
{
	TArray<UDAItem*> ItemFound{};

	/**
	 * Find type of gear from inventroy that is already equipped.
	 * This is to not display that same item to the UI.
	 */
	UDAItem* EquippedGear{};
	for (auto ExistingGear : _GearSlots)
	{
		if(ExistingGear._GearType == GearType)
		{
			EquippedGear = ExistingGear._GearItem;
			break;
		}
	}
	
#pragma region // Find the list of gear related to the type
	for (int32 i = 0; i < _InventorySlots.Num(); i++)
	{
		if(_InventorySlots[i]._Item == nullptr) { continue; }

		FItemData Data = _InventorySlots[i]._Item->ItemData;
		if(Data.ItemType == EItemType::EIT_Gear)
		{
			if(Data.GearData.GearType == GearType)
			{
				if(_InventorySlots[i]._Item != EquippedGear)
				{
					ItemFound.AddUnique(_InventorySlots[i]._Item);
					//(TODO:VS this needs to be changed and called at the end of the for loop but due to a weird bug it can not be done)
				}
			}
		}
	}
#pragma endregion // Find the list of gear related to the type
	
	FireGearFoundList(ItemFound, GearType);
	return;
	
}

void UInventoryComponent::FireUpdateGearEquipped(const FGearSlot& GearEquipped) const
{
	OnGearEquipped.ExecuteIfBound(GearEquipped);
}

void UInventoryComponent::FireGearFoundList(const TArray<UDAItem*>& GearFound, EGearType GearType) const
{
	OnGearsFound.Execute(GearFound, GearType);
}
#pragma endregion //Gear Related Functions

void UInventoryComponent::FireUpdateInventoryUI() const
{
	//Fire Event to update UI inventory slots
	OnUpdateInventory.ExecuteIfBound(_InventorySlots);
}
