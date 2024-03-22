// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/EEnemyState.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "EnemyController.generated.h"

class AEnemyCharacter;

UCLASS()
class MAGEVENTURE_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyController();
	void Initialise(AActor* SpawnLocationActor);

	void HandlePlayerDead();
	void HandlePlayerAlive();
	
	/**
	 * Checks if the Blackboard target actor value is set.
	 * 
	 * @return true, if Blackboard TargetActor value is set.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool IsTargetSet();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	/**
	 * Checks if the actor detected is a player
	 * 
	 * @param ActorDetected The actor that is detected.
	 * @return True, if the detected actor was a player.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy")
	bool IsItPlayer(AActor* ActorDetected);
	
	/**
	 * Set the blackboard Target actor variable. 
	 * 
	 * @param ObjectValue Actor to set as Target actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetTarget(UObject* ObjectValue);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ClearTarget();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	UBehaviorTree* _BehaviourTree{};

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<AEnemyCharacter> _EnemyToSpawn{};
	TObjectPtr<AEnemyCharacter> _EnemyCharacter{};
	
	void HandleDeath();
	void HandleAlive();
	void HandleStateChange(EEnemyState EnemyState) const;
	
	bool _bIsAlive{true};
	bool _IsPlayerAlive{true};
	FName _PlayerTag{"Player"};

	TObjectPtr<UBlackboardComponent> _BlackBoard{};
	FName _TargetActorName{"TargetActor"};
	FName _EnemyStateName{"EnemyState"};
	
};
