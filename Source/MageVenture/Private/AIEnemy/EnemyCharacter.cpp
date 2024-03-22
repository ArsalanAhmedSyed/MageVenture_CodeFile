
#include "AIEnemy/EnemyCharacter.h"
#include "AIEnemy/Component/EnemyAttributeComponent.h"
#include "AIEnemy/DataAssets/EnemyData.h"
#include "AIEnemy/Enums/EEnemyState.h"
#include "AIEnemy/Interfaces/EnemyWidgetInterface.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/Actor/ItemSpawner.h"
#include "Inventory/Data Asset/DAItem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Other/FunctionsLibrary.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	_AttackCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Attack Collider Component"));
	_EnemyAttributeComponent = CreateDefaultSubobject<UEnemyAttributeComponent>(TEXT("Attribute Component"));
	_HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Widget Component"));
	
	_AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_HealthWidgetComponent->SetupAttachment(GetRootComponent());
	SetHealthWidgetVisibility(false);
	_EnemyState = EEnemyState::EES_Unoccupied;
	AutoPossessAI = EAutoPossessAI::Disabled;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AEnemyCharacter::Initialise(UEnemyData* EnemyData)
{
	_EnemyData = EnemyData;
	_EnemyAttributeComponent->Initialise(_EnemyData->Attributes);

	OnTakeAnyDamage.AddDynamic(this, &ThisClass::Damage);
	_AttackCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if(_EnemyData)
	{
		_AttackMontage = _EnemyData->AttackMontage;
		_DeathMontage = _EnemyData->DeathMontage;
		
		GetCharacterMovement()->MaxWalkSpeed = _EnemyData->MovementSpeed;
		GetMesh()->SetAnimInstanceClass(_EnemyData->Animation);
		GetMesh()->SetSkeletalMeshAsset(_EnemyData->SkeletalMesh);

		const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
		_AttackCollider->AttachToComponent(GetMesh(), AttachmentTransformRules, FName("Attack_Hand"));
	}
	
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::HandleMontageEnded);
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::HandleMontageNotifies);
	}
}

void AEnemyCharacter::ToggleAttackCollider(ECollisionEnabled::Type SetCollisionType)
{
	_AttackCollider->SetCollisionEnabled(SetCollisionType);
}

void AEnemyCharacter::Attack()
{
	if(_EnemyState != EEnemyState::EES_Unoccupied || _AttackMontage == nullptr) { return; }

	ChangeStateAndUpdate(EEnemyState::EES_Attack);
	GetMesh()->GetAnimInstance()->Montage_Play( _AttackMontage, 1);
}

void AEnemyCharacter::SetHealthWidgetVisibility(bool bSetVisible) const
{
	_HealthWidgetComponent->SetVisibility(bSetVisible);
}

void AEnemyCharacter::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag(FName("Player")))
	{
		//If the actor was already hit then return
		if(_ActorHit == OtherActor) { return; }

		_ActorHit = OtherActor;

		const float FinalDamage = UFunctionsLibrary::CalculateDamage(_EnemyData->DamageAmount, this, _ActorHit);
		UGameplayStatics::ApplyDamage(_ActorHit, FinalDamage, this->GetController(), this,UDamageType::StaticClass());
	}
}

FAttribute AEnemyCharacter::GetInterfaceAttribute(EAttributeType AttributeType)
{
	 return _EnemyAttributeComponent->GetAttribute(AttributeType); 
}

void AEnemyCharacter::UpdateHealthBarWidget() const
{
	if(_HealthWidgetComponent->GetWidget()->Implements<UEnemyWidgetInterface>())
	{
		const float MaxHealthValue = _EnemyAttributeComponent->GetAttribute(EAttributeType::EAT_Health).MaxValue;
		const float CurrentHealthValue = _EnemyAttributeComponent->GetAttribute(EAttributeType::EAT_Health).CurrentValue;
		const float HealthBarPercent = CurrentHealthValue / MaxHealthValue;
		IEnemyWidgetInterface::Execute_UpdateBar(_HealthWidgetComponent->GetWidget(), HealthBarPercent);
	}
}

void AEnemyCharacter::HandleMontageNotifies(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if(BranchingPointPayload.SequenceAsset == _EnemyData->AttackMontage)
	{
		_ActorHit = nullptr;
		(NotifyName == FName("EnableAttackCollider")) ? ToggleAttackCollider(ECollisionEnabled::QueryOnly) : ToggleAttackCollider(ECollisionEnabled::NoCollision);
	}
	
	if(NotifyName == FName("DeathEnd"))
	{
		ToggleComponentActive(true, ECollisionEnabled::NoCollision);
		ChangeStateAndUpdate(EEnemyState::EES_Occupied);
		GetWorldTimerManager().SetTimer(THRespawnTimer,this, &ThisClass::RespawnEnemy, _RespawnRate, false);
	}
}

bool AEnemyCharacter::IsAlive_Implementation()
{
	return _bIsAlive;
}

void AEnemyCharacter::Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(_EnemyState == EEnemyState::EES_Dead || _EnemyAttributeComponent == nullptr) { return; }

	_EnemyAttributeComponent->ReduceHealth(Damage);

	SetHealthWidgetVisibility(true);
	UpdateHealthBarWidget();

	if(_EnemyAttributeComponent->GetAttribute(EAttributeType::EAT_Health).CurrentValue <= 0)
	{
		GetMesh()->GetAnimInstance()->Montage_Play( _DeathMontage, 1);
		_bIsAlive = false;
		ToggleAttackCollider(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ChangeStateAndUpdate(EEnemyState::EES_Dead);
		
		if(IAttributeInterface* AttributeInterface = Cast<IAttributeInterface>(DamageCauser))
		{
			AttributeInterface->GiveEXP(_EnemyData->GiveExp);
		}


		if(UKismetMathLibrary::RandomBoolWithWeight(_EnemyData->ItemDropRate / 100))
		{
			#pragma region //Spawn Random Item
			const int32 ItemDropNum = FMath::RandHelper(_EnemyData->ItemDropList.Num());
			
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Instigator = this->GetInstigator();
			SpawnParameters.Owner = this;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			const TObjectPtr<AItemSpawner> ItemSpawned = GetWorld()->SpawnActor<AItemSpawner>(_EnemyData->ItemDropList[ItemDropNum]->ItemSpawner,GetActorLocation(),GetActorRotation(), SpawnParameters);  
			ItemSpawned->Initialise(_EnemyData->ItemDropList[ItemDropNum]);
			#pragma endregion //Spawn Random Item
		}

		OnDeath.ExecuteIfBound();
	}
}

void AEnemyCharacter::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(_AttackMontage == Montage && bInterrupted == false)
	{
		ChangeStateAndUpdate(EEnemyState::EES_Unoccupied);
		_ActorHit == nullptr;
	}
}

void AEnemyCharacter::RespawnEnemy()
{
	_bIsAlive = true;

	GetCharacterMovement()->MaxWalkSpeed = _EnemyData->MovementSpeed;
	_EnemyAttributeComponent->ResetHealth();
	UpdateHealthBarWidget();
	
	ToggleComponentActive(false, ECollisionEnabled::QueryAndPhysics);

	//Let controller know that AI is alive
	OnAlive.ExecuteIfBound();
	ChangeStateAndUpdate(EEnemyState::EES_Unoccupied);
}

void AEnemyCharacter::ToggleComponentActive(bool SetHidden, ECollisionEnabled::Type SetCollision) const
{
	GetCapsuleComponent()->SetCollisionEnabled(SetCollision);
	GetMesh()->SetHiddenInGame(SetHidden);
}

void AEnemyCharacter::ChangeStateAndUpdate(EEnemyState EnemyState)
{
	_EnemyState = EnemyState;
	OnStateChange.ExecuteIfBound(_EnemyState);
}

void AEnemyCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);

	OutLocation = GetPawnViewLocation();
	OutRotation = GetActorRotation();
}


