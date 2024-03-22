// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BaseAbility.generated.h"

enum class EActorType : uint8;
class UNiagaraComponent;
class UAbilityData;

UCLASS()
class MAGEVENTURE_API ABaseAbility : public AActor
{
	GENERATED_BODY()

public:
	ABaseAbility();
	void Initialise(UAbilityData* AbilityData, int32 AbilityLevel);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	/**
	 * Play particle effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void PlayParticle();

	/**
	 * Play sound effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void PlaySoundEffect();

	/**
	 * Set ability mesh.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetMesh();

	/**
	 *  Returns projectile speed. 
	 * @return Speed of Projectile
	 */
	UFUNCTION(BlueprintPure, Category = "Ability")
	float GetProjectileSpeed() const;
	
	/**
	 * stores the value of actor hit to later apply damage to or effects.
	 * 
	 * @param ActorHit Store value of the actor that got hit by the ability.
	 * @return true, if actor hit is an enemy.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool SetActorHit(AActor* ActorHit);

	/**
	 *  Note: Must use this function first to add actors to the list
	 *  before calling any other functions related to array or list of actors hit.
	 * 
	 * @param ActorHit Set actor that overlapped or hit by the ability to a list.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetListOfActorHit(AActor* ActorHit);

	/**
	 * Get the actor that was hit earlier.
	 * 
	 * @return Actor that was previously hit.
	 */
	UFUNCTION(BlueprintPure, Category = "Ability")
	AActor* GetActorHit() const {return _ActorHit; }

	/**
	 * Note: This should only be used if you have added actors to the list by calling SetListOfActorHit() function.
	 * 
	 * @return All the Actors stored in the array
	 */
	
	UFUNCTION(BlueprintPure, Category = "Ability")
	TArray<AActor*> GetAllActorHit() const {return _ListOfActorsHit; }
	/**
	 * It performs calculation for damage output.
	 * Also calls the default Apply damage and AI damage sense.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ApplyDamageToActor();

	/**
	 * Applies damage to all the actors in the list of actors hit.
	 * Note: This should only be used if you have added actors to the list by calling SetListOfActorHit() function.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ApplyDamageToAllActors();
	
	/**
	 * Apply damage overtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ApplyOverTimeDamage();

	/**
	 *  Continuously apply Damage to all the actors in the list of actor hit.
	 *  Note: This should only be used if you have added actors to the list by calling SetListOfActorHit() function.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ApplyOverTimeDamageToAll();

	/**
	 * Stop apply damage overtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void CancelOverTimeDamage();

	/**
	 * This function removes an actor from the list of actor stored in the array of actors hit.
	 *  Note: This should only be used if you have added actors to the list by calling SetListOfActorHit() function.
	 *  
	 * @param RemoveDamageOnActor The actor to remove from the list of actor hit
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void CancelDamageOnActor(AActor* RemoveDamageOnActor);

	/**
	 * This will only hit a single Enemy
	 * 
	 * @return Return hit result of the sphere trace.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FHitResult PerformSphereTrace();

	/**
	 * This allows the ability to hit multiple enemies.
	 *  
	 * @return Return hit result of all the enemy actors hit by the sphere trace.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	TArray<FHitResult> PerformMultiSphereTrace();

	/**
	 * This can be used to place the actor on the ground and get position of the ground.
	 * 
	 * @param FromPosition Location from where the line trace should check for ground.
	 * @return Position of the ground.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FVector GetGroundPosition(FVector FromPosition);

	/**
	 * Get radius for the ability from data table.
	 * 
	 * @param RadiusType Enum to select specific radius from data table, this can be trace or projectile radius.
	 * @return radius value returned based on enum passed in.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	float GetAbilityRadius(EActorType RadiusType);

	/**
	 * Adds component to the actor that was hit.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ApplyEffect();

	/**
	 * Spawns actor.
	 * 
	 * @param SpawnLocation Location of where to spawn the actor.
	 * @param SpawnRotation Rotation of where the actor should face or move to.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SpawnActor(FVector SpawnLocation, FRotator SpawnRotation);

	/**
	 * Destroys the ability after the specify delay time has passed.
	 * 
	 * @param delay Set how long before the ability is destroyed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void DestroyActorWithDelay(float delay);
	void DestroyActorCall();

#pragma region // Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability Data | Visual")
	TObjectPtr<UStaticMeshComponent> _StaticMeshComponent{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability Data | Visual")
	TObjectPtr<UParticleSystemComponent> _CascadeParticleComponent{};
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> _ParentComponent{};
#pragma endregion // Components
	
private:
	
	TObjectPtr<UAbilityData> _AbilityData{};
	UPROPERTY()
	TObjectPtr<AActor> _ActorHit{};
	TArray<TObjectPtr<AActor>> _ListOfActorsHit{};
	UPROPERTY()
	TObjectPtr<USoundBase> _SoundEffect{};

	UPROPERTY(EditDefaultsOnly, Category = "Ability | Trace", meta=(ToolTip="This property is for single sphere trace channel"))
	TEnumAsByte<ETraceTypeQuery> _TraceTypeQuery{};
	UPROPERTY(EditDefaultsOnly, Category = "Ability | Trace", meta=(ToolTip="This property is for multi sphere trace for objects"))
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes{};
	UPROPERTY(EditDefaultsOnly, Category = "Ability | Trace")
	TEnumAsByte<EDrawDebugTrace::Type> _DrawDebugTrace{EDrawDebugTrace::None};
	
	FTimerHandle THDamageTimer{};
	FTimerHandle THDestroyTimer{};

	int32 _AbilityLevel{};
};
