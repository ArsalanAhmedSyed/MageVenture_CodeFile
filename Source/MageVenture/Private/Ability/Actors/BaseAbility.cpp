
#include "Ability/Actors/BaseAbility.h"
#include "Ability/AbilityComponentInterface.h"
#include "Ability/Data/AbilityData.h"
#include "Ability/Enums/EActorType.h"
#include "Kismet/GameplayStatics.h"
#include "Other/FunctionsLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/AISense_Damage.h"
#include "Player/Interfaces/AttributeInterface.h"

ABaseAbility::ABaseAbility()
{
	PrimaryActorTick.bCanEverTick = false;

	_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	_CascadeParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Paritcle Component"));
	_ParentComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SetRootComponent(_ParentComponent);
	
	_CascadeParticleComponent->SetupAttachment(GetRootComponent());
	_StaticMeshComponent->SetupAttachment(GetRootComponent());
}

void ABaseAbility::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(_AbilityData == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Warning Ability Data Asset has not been set")),
			true, true, FLinearColor::Yellow, -1, FName("1"));
		return;
	}
	
	if(_StaticMeshComponent == nullptr || _CascadeParticleComponent == nullptr)
	{ 
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Components are not setup")));
		return;
	}
	
	_StaticMeshComponent->SetStaticMesh(_AbilityData->AbilityMesh);
	_CascadeParticleComponent->SetTemplate(_AbilityData->ParticleEffect);

	if(_AbilityData->Duration.GetValueAtLevel(_AbilityLevel) != 0)
	{
		this->InitialLifeSpan = _AbilityData->Duration.GetValueAtLevel(_AbilityLevel);
	}

	if(_AbilityData->Radius.BaseValue != 0)
	{
		const FVector NewScale = _StaticMeshComponent->GetRelativeScale3D() * _AbilityData->Radius.GetValueAtLevel(_AbilityLevel);
		_StaticMeshComponent->SetRelativeScale3D(NewScale); 
	}
}

void ABaseAbility::Initialise(UAbilityData* AbilityData, int32 AbilityLevel)
{
	_AbilityData = AbilityData;
	_AbilityLevel = AbilityLevel;
}

void ABaseAbility::PlayParticle()
{
	_CascadeParticleComponent->Activate(true);
}

void ABaseAbility::PlaySoundEffect()
{
	UGameplayStatics::PlaySound2D(this, _AbilityData->SoundEffect);
}

void ABaseAbility::SetMesh()
{
	_StaticMeshComponent->SetStaticMesh(_AbilityData->AbilityMesh);
}

float ABaseAbility::GetProjectileSpeed() const
{
	if(_AbilityData == nullptr) { return float(); }
	return _AbilityData->ProjectileSpeed.GetValueAtLevel(_AbilityLevel);
}

bool ABaseAbility::SetActorHit(AActor* ActorHit)
{
	if(ActorHit == nullptr) { return false; }
	if(ActorHit->ActorHasTag(FName("Enemy")))
	{
		_ActorHit = ActorHit;
		return true;
	}
	
	return false;
}

void ABaseAbility::SetListOfActorHit(AActor* ActorHit)
{
	if(SetActorHit(ActorHit))
	{
		_ListOfActorsHit.AddUnique(_ActorHit);
	}
}

void ABaseAbility::ApplyDamageToActor()
{
	if(_ActorHit == nullptr) { return; }

	const float FinalDamage = UFunctionsLibrary::CalculateDamage(_AbilityData->Damage.GetValueAtLevel(_AbilityLevel) ,GetOwner(),_ActorHit);
	//If no damage was applied
	if(FinalDamage <= -1)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Failed to Apply Damage")), true, true, FLinearColor::Red, 10);
		return;
	}
	
	UGameplayStatics::ApplyDamage(_ActorHit, FinalDamage, GetOwner()->GetInstigatorController(),GetOwner(),UDamageType::StaticClass());
	UAISense_Damage::ReportDamageEvent(this, _ActorHit, GetOwner(), FinalDamage, GetOwner()->GetActorLocation(), FVector());
}

void ABaseAbility::ApplyDamageToAllActors()
{
	for (const auto ActorHit : _ListOfActorsHit)
	{
		_ActorHit = ActorHit;
		ApplyDamageToActor();
	}
}

void ABaseAbility::ApplyOverTimeDamage()
{
	GetWorldTimerManager().SetTimer(THDamageTimer, this, &ABaseAbility::ApplyDamageToActor, _AbilityData->DamagePerSeconds.GetValueAtLevel(_AbilityLevel), true);
}

void ABaseAbility::ApplyOverTimeDamageToAll()
{
	GetWorldTimerManager().SetTimer(THDamageTimer, this, &ABaseAbility::ApplyDamageToAllActors, _AbilityData->DamagePerSeconds.GetValueAtLevel(_AbilityLevel), true);
}

void ABaseAbility::CancelOverTimeDamage()
{
	if(THDamageTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(THDamageTimer);
	}
}

void ABaseAbility::CancelDamageOnActor(AActor* RemoveDamageOnActor)
{
	if(RemoveDamageOnActor == nullptr) { return; }

	//Setting actor hit here so that ability effect is applied to the actor at the end.
	_ActorHit = RemoveDamageOnActor;
	
	_ListOfActorsHit.Remove(RemoveDamageOnActor);
}

FHitResult ABaseAbility::PerformSphereTrace()
{
#pragma region // Trace Params
	const FVector ActorLocation = GetActorLocation();
	TArray<AActor*> ActorToIgnore{};
	ActorToIgnore.Add(this);
	ActorToIgnore.Add(GetOwner());
	FHitResult HitResult;
#pragma endregion // Trace Params
	
	UKismetSystemLibrary::SphereTraceSingle(this, ActorLocation, ActorLocation + GetActorForwardVector() * _AbilityData->TraceRange.GetValueAtLevel(_AbilityLevel),
	_AbilityData->TraceRadius.GetValueAtLevel(_AbilityLevel), _TraceTypeQuery, false,ActorToIgnore, _DrawDebugTrace,
	HitResult, true);

	return HitResult;
}

TArray<FHitResult> ABaseAbility::PerformMultiSphereTrace()
{
#pragma region // Trace Params
	const FVector ActorLocation = GetActorLocation();
	TArray<AActor*> ActorsToIgnore{};
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> HitResults{};
#pragma endregion // Trace Params
	
	UKismetSystemLibrary::SphereTraceMultiForObjects(this, ActorLocation, ActorLocation + GetActorForwardVector() * _AbilityData->TraceRange.GetValueAtLevel(_AbilityLevel),
		_AbilityData->TraceRadius.GetValueAtLevel(_AbilityLevel), ObjectTypes, false, ActorsToIgnore, _DrawDebugTrace, HitResults, true);
	
	return HitResults;
}

FVector ABaseAbility::GetGroundPosition(FVector FromPosition)
{
#pragma region // Trace Params
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);
	ActorToIgnore.Add(GetOwner());
	ActorToIgnore.Add(_ActorHit);
	FHitResult HitResult;
#pragma endregion // Trace Params

	//Perform line trace downwards
	if(UKismetSystemLibrary::LineTraceSingle(this, FromPosition,FromPosition + FVector::UpVector * -3000,
			TraceTypeQuery1,  false, ActorToIgnore, _DrawDebugTrace, HitResult, true))
	{
		return HitResult.ImpactPoint;
	}

	return FVector();
}

float ABaseAbility::GetAbilityRadius(EActorType RadiusType)
{
	switch(RadiusType)
	{
	case EActorType::EAT_Projectile:
		return _AbilityData->Radius.GetValueAtLevel(_AbilityLevel);
	case EActorType::EAT_Trace:
		return _AbilityData->TraceRadius.GetValueAtLevel(_AbilityLevel);
	default:
		return _AbilityData->Radius.GetValueAtLevel(_AbilityLevel);
	}
}

void ABaseAbility::ApplyEffect()
{
	if(_ActorHit == nullptr) { return; }
	if(_ActorHit->Implements<UAttributeInterface>())
	{
		const bool isActorAlive = IAttributeInterface::Execute_IsAlive(_ActorHit);
		if(isActorAlive == false) { return; }
	}

	UActorComponent* ActorAdded = _ActorHit->AddComponentByClass(_AbilityData->AbilityEffect,false, FTransform(),false);
	if(IAbilityComponentInterface* AbilityComponentInterface = Cast<IAbilityComponentInterface>(ActorAdded))
	{
		AbilityComponentInterface->Initialise(_AbilityData->DebuffDuration.GetValueAtLevel(_AbilityLevel),
			_AbilityData->DebuffAmount.GetValueAtLevel(_AbilityLevel), _AbilityData->DebuffPerSec.GetValueAtLevel(_AbilityLevel),GetOwner());
	}
}

void ABaseAbility::SpawnActor(FVector SpawnLocation, FRotator SpawnRotation)
{
#pragma region // Spawning Parameter 
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = GetOwner()->GetInstigator();
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FTransform SpawnTransform{};
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());
#pragma endregion // Spawning Parameter
	
	ABaseAbility* SpawnedActor = GetWorld()->SpawnActorDeferred<ABaseAbility>(_AbilityData->ActorToSpawn, SpawnTransform, Owner, Owner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	SpawnedActor->Initialise(_AbilityData->ActorAbilityData, _AbilityLevel);
	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);
}

void ABaseAbility::DestroyActorWithDelay(float delay)
{
	GetWorldTimerManager().SetTimer(THDestroyTimer, this, &ThisClass::DestroyActorCall, delay, false);
}

void ABaseAbility::DestroyActorCall()
{
	this->Destroy();
}


