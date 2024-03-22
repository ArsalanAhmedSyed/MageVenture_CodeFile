
#include "Inventory/Actor/ItemSpawner.h"
#include "Inventory/Data Asset/DAItem.h"

AItemSpawner::AItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(_StaticMeshComponent);
}

void AItemSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetItemMesh();
}

void AItemSpawner::Initialise(UDAItem* ItemData)
{
	_ItemDataType = ItemData;
	SetItemMesh();
}

UDAItem* AItemSpawner::PickupItem()
{
	return 	_ItemDataType;
}

void AItemSpawner::SetItemMesh() const
{
	if(_StaticMeshComponent == nullptr || _ItemDataType == nullptr) { return; }

	if(_ItemDataType->ItemSpawnMesh)
	{
		_StaticMeshComponent->SetStaticMesh(_ItemDataType->ItemSpawnMesh);
	}
}

