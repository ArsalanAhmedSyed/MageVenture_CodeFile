
#include "ProjectSetup/MageVentureGM.h"
#include "AIEnemy/EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AresPlayerController.h"

AMageVentureGM::AMageVentureGM() { }

void AMageVentureGM::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if(_EnemyControllerToSpawn == nullptr ||_EnemySpawnLocation == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Properites in BPA_MageVentureGM are not filled!"),
			true, true, FLinearColor::Yellow, 10);
	}
}
	
void AMageVentureGM::BeginPlay()
{
	Super::BeginPlay();

#pragma region //Initialise Player Controller
	AActor* ControllerFound = UGameplayStatics::GetActorOfClass(this, PlayerControllerClass);
	
	AAresPlayerController* AresPlayerController =  Cast<AAresPlayerController>(ControllerFound);
	if(AresPlayerController)
	{
		AresPlayerController->OnPlayerDeadState.BindUObject(this, &ThisClass::HandleOnPlayerDead);
		AresPlayerController->OnPlayerRespawnState.BindUObject(this, &ThisClass::HandleOnPlayerRespawn);
		
		AresPlayerController->Initialise();
	}
#pragma endregion //Initialise Player Controller

#pragma region //Initialise AI Characters

	TArray<AActor*> SpawnLocation{};
	UGameplayStatics::GetAllActorsOfClass(this, _EnemySpawnLocation,SpawnLocation);
	
	for (auto SpawnLocationFound : SpawnLocation)
	{
		AEnemyController* EnemyController = GetWorld()->SpawnActor<AEnemyController>(_EnemyControllerToSpawn, FVector(), FRotator());
		_EnemyControllers.AddUnique(EnemyController);
		EnemyController->Initialise(SpawnLocationFound);
	}
	
#pragma endregion //Initialise AI Characters

}

void AMageVentureGM::HandleOnPlayerDead()
{
	for (const auto EnemyController : _EnemyControllers)
	{
		EnemyController->HandlePlayerDead();
	}
}

void AMageVentureGM::HandleOnPlayerRespawn()
{
	for (const auto EnemyController : _EnemyControllers)
	{
		EnemyController->HandlePlayerAlive();
	}
}
