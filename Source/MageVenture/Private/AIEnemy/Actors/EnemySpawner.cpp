
#include "AIEnemy/Actors/EnemySpawner.h"

#include "AIEnemy/DataAssets/EnemyData.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	_SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SetRootComponent(_SkeletalMeshComponent);
}

void AEnemySpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(_EnemyData && _SkeletalMeshComponent)
	{
		_SkeletalMeshComponent->SetSkeletalMesh(_EnemyData->SkeletalMesh);
	}
}

