
#include "Vendor/PlayerTransaction.h"
#include "Inventory/Components/InventoryComponent.h"

UPlayerTransaction::UPlayerTransaction() { }

void UPlayerTransaction::Initialise(UInventoryComponent* InventoryComponent)
{
	_InventoryComponent = InventoryComponent;
}

bool UPlayerTransaction::CanBuy(UDAItem* Item) const
{
	if(_InventoryComponent->CanAddItem(Item))
	{
		const int32 Money = _InventoryComponent->GetMoney();
		if(Money >= Item->ItemMarket.BuyPrice) { return true; }
	}

	return false;
}

bool UPlayerTransaction::RequestBuyItem(UDAItem* ItemBought) const
{
	bool bTransactionSuccess{false};
	
	if(_InventoryComponent->AddItem(ItemBought))
	{
		/*Decrement Players money*/
		const int32 CurrentMoney = _InventoryComponent->GetMoney();
		const int32 ItemPrice = ItemBought->ItemMarket.BuyPrice;
		const int32 UpdatedMoney = FMath::Clamp(CurrentMoney - ItemPrice,0, CurrentMoney);
		_InventoryComponent->SetMoney(UpdatedMoney);

		bTransactionSuccess = true;
	}

	return bTransactionSuccess;
}

bool UPlayerTransaction::RequestSellItem(UDAItem* ItemToSell) const
{
	bool bTransactionSuccess{false};

	if(_InventoryComponent->RemoveItem(ItemToSell))
	{
		/*Give player money for the item sold*/
		const int32 UpdatedMoney = _InventoryComponent->GetMoney() + ItemToSell->ItemMarket.SellPrice;
		_InventoryComponent->SetMoney(UpdatedMoney);
		bTransactionSuccess = true;
	}

	return bTransactionSuccess;
}

const TArray<FInventorySlot>& UPlayerTransaction::GetInventorySlots() const
{
	return _InventoryComponent->GetInventorySlots();
}
