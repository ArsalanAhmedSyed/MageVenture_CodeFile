#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class UEnemyData;

UCLASS()
class MAGEVENTURE_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(EditAnywhere, Category = "Enemy")
	TObjectPtr<UEnemyData> _EnemyData{};

	UPROPERTY(VisibleAnywhere, Category = "Enemy | Component")
	TObjectPtr<USkeletalMeshComponent> _SkeletalMeshComponent{};

public:
	FORCEINLINE UEnemyData* GetEnemyData() { return _EnemyData;}
};
