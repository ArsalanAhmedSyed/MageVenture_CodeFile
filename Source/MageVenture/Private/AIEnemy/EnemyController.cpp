// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy/EnemyController.h"
#include "AIEnemy/EnemyCharacter.h"
#include "AIEnemy/Actors/EnemySpawner.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyController::AEnemyController()
{
	PrimaryActorTick.bCanEverTick = false;
	bSetControlRotationFromPawnOrientation = true;
}

void AEnemyController::Initialise(AActor* SpawnLocationActor)
{
	RunBehaviorTree(_BehaviourTree);
	_BlackBoard = GetBlackboardComponent();

	if(AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(SpawnLocationActor))
	{
		#pragma region //AI Pawn Spawn detail
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FTransform Transform{};
		Transform.SetLocation(SpawnLocationActor->GetActorLocation());
		Transform.SetRotation(SpawnLocationActor->GetActorQuat());
		#pragma endregion //AI Pawn Spawn detail
		
		AEnemyCharacter* EnemyCharacter = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(_EnemyToSpawn, Transform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		EnemyCharacter->Initialise(EnemySpawner->GetEnemyData());
		UGameplayStatics::FinishSpawningActor(EnemyCharacter, Transform);
		EnemySpawner->Destroy();

		this->Possess(EnemyCharacter);
		this->AttachToPawn(GetPawn());
	}
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	_EnemyCharacter = Cast<AEnemyCharacter>(InPawn);
	if(_EnemyCharacter == nullptr) { return; }
	
	_EnemyCharacter->OnDeath.BindUObject(this, &ThisClass::HandleDeath);
	_EnemyCharacter->OnAlive.BindUObject(this, &ThisClass::HandleAlive);
	_EnemyCharacter->OnStateChange.BindUObject(this, &ThisClass::HandleStateChange);
}

bool AEnemyController::IsItPlayer(AActor* ActorDetected)
{
	if(_bIsAlive == false) { return false; }

	if(ActorDetected->ActorHasTag(_PlayerTag) && _IsPlayerAlive)
	{
		return true;
	}

	return false;
}

void AEnemyController::SetTarget(UObject* ObjectValue)
{
	_BlackBoard->SetValueAsObject(_TargetActorName, ObjectValue);
}

void AEnemyController::ClearTarget()
{
	_BlackBoard->ClearValue(_TargetActorName);
	_EnemyCharacter->SetHealthWidgetVisibility(false);
	HandleStateChange(EEnemyState::EES_Unoccupied);
}

void AEnemyController::HandleDeath()
{
	_bIsAlive = false;
	_EnemyCharacter->SetHealthWidgetVisibility(false);
	_BlackBoard->ClearValue(_TargetActorName);
}

void AEnemyController::HandleAlive()
{
	_bIsAlive = true;
}

void AEnemyController::HandleStateChange(EEnemyState EnemyState) const
{
	const uint8 EnemyStateUint8 = static_cast<UINT8>(EnemyState);
	_BlackBoard->SetValueAsEnum(_EnemyStateName, EnemyStateUint8);
}

void AEnemyController::HandlePlayerDead()
{
	if(IsTargetSet() == false) { return; }

	_IsPlayerAlive = false;
	_EnemyCharacter->SetHealthWidgetVisibility(false);
	_BlackBoard->ClearValue(_TargetActorName);
}

void AEnemyController::HandlePlayerAlive()
{
	_IsPlayerAlive = true;
}

bool AEnemyController::IsTargetSet()
{
	if(_BlackBoard->GetValueAsObject(_TargetActorName) != nullptr) { return true; }
	return false;
}


