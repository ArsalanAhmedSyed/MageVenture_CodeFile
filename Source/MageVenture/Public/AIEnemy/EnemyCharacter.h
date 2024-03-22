// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/Interfaces/AttributeInterface.h"
#include "AIEnemy/Component/EnemyAttributeComponent.h"
#include "EnemyCharacter.generated.h"

class AItemSpawner;
class UDAItem;
class UWidgetComponent;
class UEnemyData;
enum class EEnemyState : uint8;

DECLARE_DELEGATE(FOnLifeChangeSignature);
DECLARE_DELEGATE_OneParam(FOnStateChangeSignature, EEnemyState);

UCLASS()
class MAGEVENTURE_API AEnemyCharacter : public ACharacter, public IAttributeInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	void Initialise(UEnemyData* EnemyData);

	FOnLifeChangeSignature OnDeath{};
	FOnLifeChangeSignature OnAlive{};
	FOnStateChangeSignature OnStateChange{};

	/**
	 * Enable or disable the attack collider.
	 * 
	 * Enabling will allow the enemy to apply damage
	 * Disabling will prevent the attack from taking place
	 * 
	 * @param SetCollisionType Toggle Attack Collider
	 */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ToggleAttackCollider(ECollisionEnabled::Type SetCollisionType);
	
	/**
	 * Play attack montage
	 */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void Attack();

	void SetHealthWidgetVisibility(bool bSetVisible) const;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	/*Interfaces*/
	virtual FAttribute GetInterfaceAttribute(EAttributeType AttributeType) override;
	virtual bool IsAlive_Implementation() override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

private:

#pragma region // Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Data", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> _AttackCollider{};
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnemyAttributeComponent> _EnemyAttributeComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> _HealthWidgetComponent{};
#pragma endregion // Components

#pragma region // Montages
	UPROPERTY()
	UAnimMontage* _AttackMontage{};
	UPROPERTY()
	UAnimMontage* _DeathMontage{};
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion // Montages

#pragma region // Respawn Properties
	FTimerHandle THRespawnTimer{};
	void RespawnEnemy();
	FVector RespawnLocation{};
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Properties")
	float _RespawnRate{5.f};

	bool _bIsAlive{true};
#pragma endregion // Respawn Properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyData> _EnemyData{};
	EEnemyState _EnemyState{};
	void ChangeStateAndUpdate(EEnemyState EnemyState);

	UFUNCTION()
	void Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UPROPERTY()
	TObjectPtr<AActor> _ActorHit{};
	void ToggleComponentActive(bool SetHidden, ECollisionEnabled::Type SetCollision) const;
	void UpdateHealthBarWidget() const;

	UFUNCTION()
	void HandleMontageNotifies(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	
public:
	UFUNCTION(BlueprintCallable)
	EEnemyState GetEnemyState() { return _EnemyState; }
};
