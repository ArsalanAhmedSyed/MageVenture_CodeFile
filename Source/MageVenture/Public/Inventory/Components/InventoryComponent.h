#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Structs/FGearSlot.h"
#include "Inventory/Structs/FInventorySlot.h"
#include "InventoryComponent.generated.h"

class UPlayerTransaction;

#pragma region // Delegates
DECLARE_DELEGATE_OneParam(FUpdateInventorySignature, const TArray<FInventorySlot>& /*UpdateInventory*/)
DECLARE_DELEGATE_OneParam(FOnGearEquippedSignature, const FGearSlot& /*GearEquipped*/)
DECLARE_DELEGATE_OneParam(FOnGearUnEquippedSignature, const FGearStats& /*RemoveGearStats*/)
DECLARE_DELEGATE_TwoParams(FOnGearsFoundSingature, const TArray<UDAItem*>& /*GearFound*/, EGearType /*GearType*/ )
#pragma endregion // Delegates

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAGEVENTURE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();
	void Initialise(int32 InventorySize);

#pragma region // Events
	FUpdateInventorySignature OnUpdateInventory;
	FOnGearEquippedSignature OnGearEquipped;
	FOnGearsFoundSingature OnGearsFound;
	FOnGearUnEquippedSignature OnGearUnEquipped;
#pragma endregion // Events
	
#pragma region // Item and Slot Update Functions
	bool CanAddItem(UDAItem* ItemToAdd);
	bool AddItem(UDAItem* Item);
	bool AddNewItem(UDAItem* Item);
	bool RemoveItem(int32 SlotIndex);
	bool RemoveItem(UDAItem* SellItem);
	void DropItem(int32 SlotIndex);
	void SwapItems(int32 SourceSlot, int32 CurrentSlot);
#pragma endregion // Item and Slot Update Functions

	void EquipGear(UDAItem* GearToEquip);
	void UnEquipGear(UDAItem* ItemToInventory, UDAItem* ItemToRemove);
	void FindGear(EGearType GearType);
	
private:

	TArray<FInventorySlot> _InventorySlots{};
	int32& GetCurrentStackSize ( int32 SlotIndex);

	int32 _Money{1000};
	
	int32 _GearSlotSize{3};
	TArray<FGearSlot> _GearSlots{};

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Data | Spawn", meta=(ToolTip="Set how far away from the player should the item spawn."))
	float _SpawnDistance{};
	
	/*Events*/
	void FireUpdateInventoryUI() const;
	void FireUpdateGearEquipped(const FGearSlot& GearEquipped) const;
	void FireGearFoundList(const TArray<UDAItem*>& GearFound, EGearType GearType) const;
	
	UPROPERTY(Transient)
	TObjectPtr<UPlayerTransaction> _PlayerTransaction{};

public:
	FORCEINLINE TArray<FInventorySlot>& GetInventorySlots() { return _InventorySlots; }
	FORCEINLINE int32 GetMoney() const { return _Money; }
	FORCEINLINE void SetMoney(const int32 Money) { _Money = Money; } 
	FORCEINLINE UPlayerTransaction* GetPlayerTransaction() const { return _PlayerTransaction; }
};

