
#include "Player/AresCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Items/Interfaces/Pickable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Components/AbilityComponent.h"
#include "Player/Components/AttributeComponent.h"
#include "Vendor/InteractInterface.h"

AAresCharacter::AAresCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	_SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm Component"));
	_CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comnponnet"));
	_AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute Component"));
	_InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	_AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("Ability Component"));

	_SpringArmComponent->SetupAttachment(GetMesh());
	_CameraComponent->SetupAttachment(_SpringArmComponent);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	Tags.Add(FName("Player"));
	
	_CharacterMovementComponent = GetCharacterMovement();
	_CharacterState = ECharacterState::ECS_Default;
	_CharacterMovementComponent->bOrientRotationToMovement = true;
	_CharacterMovementComponent->bUseControllerDesiredRotation = false;
	
	_DefaultTargetArmLength = 350.f;
	_DefaultSocketPosition = FVector(0, 50.f, 20.f);

	_SpringArmComponent->TargetArmLength = _DefaultTargetArmLength;
	_SpringArmComponent->SocketOffset = _DefaultSocketPosition;
}

void AAresCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(_PlayerData == nullptr || _CharacterMovementComponent == nullptr) { return; }
	
	_CharacterMovementComponent->MaxWalkSpeed = _PlayerData->_WalkSpeed;	
}

void AAresCharacter::Initialise()
{
	if(_AttributeComponent == nullptr || _InventoryComponent == nullptr || _AbilityComponent == nullptr) { return; }
	
#pragma region // Bind to Events
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::HandleMontageNotifies);
		AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::HandleMontageEnd);
	}
	
	_InventoryComponent->OnUpdateInventory.BindUObject(this, &AAresCharacter::HandleUpdatedInventory);
	_InventoryComponent->OnGearEquipped.BindUObject(this, &AAresCharacter::HandleUpdateGearEquipped);
	_InventoryComponent->OnGearUnEquipped.BindUObject(this, &AAresCharacter::HandleGearUnEquipped);
	_InventoryComponent->OnGearsFound.BindUObject(this, &AAresCharacter::HandleGearFoundUpdateUI);

	_AttributeComponent->OnAttributeUpdated.BindUObject(this, &AAresCharacter::HandleAttributesUpdated);
	_AttributeComponent->OnHealthValueUpdate.BindUObject(this, &AAresCharacter::HandleUpdateHealth);
	_AttributeComponent->OnManaValueUpdate.BindUObject(this,&AAresCharacter::HandleUpdateMana);
	_AttributeComponent->OnCharacterLevelUp.BindUObject(this, &AAresCharacter::HandleCharacterLevelUp);
	
	_AbilityComponent->OnAbilitySlotUpdate.BindUObject(this, &AAresCharacter::HandleAbilitySlotUpdate);
	_AbilityComponent->OnAbilityPerformed.BindUObject(this, &AAresCharacter::HandleConsumeMana);
#pragma endregion // Bind to Events
	
	_AttributeComponent->Initialise(_PlayerData->_ListOfAttributes, _PlayerData->EXPRequired);
	_InventoryComponent->Initialise(_PlayerData->_InventorySize);
	_AbilityComponent->Initialise(_PlayerData->_CoolDownTimer, _PlayerData->_ListOfSingularAbility, _PlayerData->_ListOfComboAbility);
	
	//Set respawn location as the start location
	_RespawnLocation = GetActorLocation();
}

float AAresCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
								 AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(_bIsAlive == true)
	{
		_AttributeComponent->ReduceHealth(DamageAmount);
	}
	
	return DamageAmount;
}

void AAresCharacter::EnableCastingMode()
{
	if(_bIsInCastingMode == true || _CharacterMovementComponent->IsFalling() == true || _bIsAlive == false) { return; }

	_bIsInCastingMode = true;
	_CharacterState = ECharacterState::ECS_Casting;
	
	StartCameraTimer();
	ChangeRotationMode();

	FTimerManager& TimerManager = GetWorldTimerManager();
	if(TimerManager.IsTimerActive( _ChangeSpeedTimerHandle))
	{
		TimerManager.ClearTimer(_ChangeSpeedTimerHandle);
	}
	
	_bIsSprinting = false;
	_CharacterMovementComponent->MaxWalkSpeed = _PlayerData->_WalkSpeed;

	OnCastingToggle.ExecuteIfBound();
}

void AAresCharacter::DisableCastingMode()
{
	if(_bIsInCastingMode == false || _bIsAttacking == true) { return; }

	_AbilityComponent->ResetAllSlots();
	_CharacterState = ECharacterState::ECS_Default;
	StartCameraTimer();
	ChangeRotationMode();
	_bIsInCastingMode = false;

	OnCastingToggle.ExecuteIfBound();
}

void AAresCharacter::ChangeRotationMode() const
{
	switch(_CharacterState)
	{
	case ECharacterState::ECS_Default:
		_CharacterMovementComponent->bOrientRotationToMovement = true;
		_CharacterMovementComponent->bUseControllerDesiredRotation = false;
		break;
	case ECharacterState::ECS_Casting:
		_CharacterMovementComponent->bOrientRotationToMovement = false;
		_CharacterMovementComponent->bUseControllerDesiredRotation = true;
		break;
	default:
		break;
	}
}

void AAresCharacter::SpeedChangeLerp()
{
	float& CurrentWalkSpeed = _CharacterMovementComponent->MaxWalkSpeed;
	CurrentWalkSpeed = FMath::LerpStable(CurrentWalkSpeed, _ChangeWalkSpeed, _ChangeWalkLerpRate);

	if(FMath::IsNearlyEqual(CurrentWalkSpeed, _ChangeWalkSpeed, 20.f))
	{
		CurrentWalkSpeed = _ChangeWalkSpeed;
		GetWorld()->GetTimerManager().ClearTimer(_ChangeSpeedTimerHandle);
	}
}

void AAresCharacter::StartCameraTimer()
{
	if(GetWorldTimerManager().IsTimerActive(_CameraZoomTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(_CameraZoomTimerHandle);
	}
	
	GetWorldTimerManager().SetTimer(_CameraZoomTimerHandle, this, &ThisClass::ChangeCameraPosition, _CameraTimerRate, true);
}

FAttribute AAresCharacter::GetInterfaceAttribute(EAttributeType AttributeType)
{
	 return _AttributeComponent->GetAttribute(AttributeType);
}

bool AAresCharacter::LerpCameraPosition(float NewTargetArmLength, const FVector& NewSocketOffset)
{
	float& CurrentTargetArmLength = _SpringArmComponent->TargetArmLength;
	FVector& CurrentSocketOffset = _SpringArmComponent->SocketOffset;
	
	CurrentTargetArmLength = FMath::LerpStable(CurrentTargetArmLength, NewTargetArmLength, _PlayerData->_ChangeCameraLerpRate);
	CurrentSocketOffset = FMath::LerpStable(CurrentSocketOffset, NewSocketOffset, _PlayerData->_ChangeCameraLerpRate);
	if(FMath::IsNearlyEqual(CurrentTargetArmLength, NewTargetArmLength))
	{
		GetWorldTimerManager().ClearTimer(_CameraZoomTimerHandle);
		return true;
	}
	return false;
}

void AAresCharacter::ChangeCameraPosition()
{
	if(_CharacterState == ECharacterState::ECS_Casting)
	{
		LerpCameraPosition(_PlayerData->_CastingTargetArmLength, _PlayerData->_CastingSocketPosition);
	}
	else if(_CharacterState == ECharacterState::ECS_Default)
	{
		LerpCameraPosition(_DefaultTargetArmLength, _DefaultSocketPosition);
	}
}

#pragma region // Input Actions
void AAresCharacter::PerformMovement(const FInputActionValue& Value)
{
	const FVector2D MovementValue = Value.Get<FVector2D>();
	const FRotator ControllerYawValue = FRotator(0.0f,GetControlRotation().Yaw, 0.0f);

	const FVector ControllerForwardVector = FRotationMatrix(ControllerYawValue).GetUnitAxis(EAxis::X);
	const FVector ControllerRightVector = FRotationMatrix(ControllerYawValue).GetUnitAxis(EAxis::Y);

	if(MovementValue.X != 0)
	{
		AddMovementInput(ControllerForwardVector, MovementValue.X);
	}

	if(MovementValue.Y != 0)
	{
		AddMovementInput(ControllerRightVector, MovementValue.Y);
	}
}

void AAresCharacter::PerformLook(const FInputActionValue& Value)
{
	const FVector2D LookDirectionValue = Value.Get<FVector2D>();

	AddControllerPitchInput(LookDirectionValue.Y);
	AddControllerYawInput(LookDirectionValue.X);
}

void AAresCharacter::PerformJump(const FInputActionValue& Value)
{
	if(_bIsAlive == false) { return; }
	
	if(_CharacterState == ECharacterState::ECS_Default)
	{
		Jump();
	}
}

void AAresCharacter::PerformSprint(const FInputActionValue& Value)
{
	if(_bIsAlive == false) { return; }
	if(_CharacterState == ECharacterState::ECS_Casting) { return; }
	
	_ChangeWalkSpeed = (_bIsSprinting == true) ? _PlayerData->_WalkSpeed : _PlayerData->_SpringSpeed;
	_bIsSprinting = !_bIsSprinting;

	GetWorld()->GetTimerManager().SetTimer(_ChangeSpeedTimerHandle, this, &ThisClass::SpeedChangeLerp, _MovementTimerRate, true);
}

void AAresCharacter::PerformSelectPrimary(const FInputActionValue& Value)
{
	if(_CharacterMovementComponent->IsFalling() == true || _bIsAlive == false) { return; }
	
	if(_AbilityComponent->SetPrimarySlot(_AttributeComponent->GetAttribute(EAttributeType::EAT_Mana).CurrentValue) == false) { return; }
	EnableCastingMode();
}

void AAresCharacter::PerformInteract(const FInputActionValue& Value)
{
	if(_bIsAlive == false) { return; }

#pragma region //Setup Sphere Trace Parameter
	FHitResult OutHit;
	FVector StartLocation = GetActorLocation() + GetActorForwardVector() * _ForwardDistance;
	FVector EndLocation = StartLocation + GetActorUpVector() *- _TraceHeight;
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);
#pragma endregion //Setup Sphere Trace Parameter

	if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, _TraceRadius, _TraceResponseType, false, ActorToIgnore, _DrawDebugTrace, OutHit, true))
	{
		if(IPickable* Pickable = Cast<IPickable>(OutHit.GetActor()))
		{
			if(_InventoryComponent->AddItem(Pickable->PickupItem()))
			{
				OutHit.GetActor()->Destroy();
				return;
			}
		}
		else if(IInteractInterface* InteractInterface = Cast<IInteractInterface>(OutHit.GetActor()))
		{
			OnInteractHideInventory.ExecuteIfBound();
			InteractInterface->Interact(_InventoryComponent->GetPlayerTransaction());
			return;
		}
	}
}

void AAresCharacter::PerformSwapAbility(const FInputActionValue& Value) const
{
	if(_bIsAlive == false) { return; }
	_AbilityComponent->SetHoverSlot(Value.Get<float>());
}

void AAresCharacter::PerformSelectSecondary(const FInputActionValue& Value) const
{
	_AbilityComponent->SetSecondarySlot();
}

void AAresCharacter::PerformInitiateAttack(const FInputActionValue& Value)
{
	if(_bIsAttacking == true || _bIsAlive == false || _CharacterState == ECharacterState::ECS_Default) { return; }
	
	const EAbilityType AbilityType = _AbilityComponent->GetAttackType(_AttributeComponent->GetAttribute(EAttributeType::EAT_Mana).CurrentValue);

	if(AbilityType == EAbilityType::EAT_Singular)
	{
		PlayAnimMontage(_PlayerData->SingularAttackMontage , 1);
		_bIsAttacking = true;
	}
	else if(AbilityType == EAbilityType::EAT_Combo)
	{
		PlayAnimMontage(_PlayerData->ComboAttackMontage, 1);
		_bIsAttacking = true;
	}
}

void AAresCharacter::PerformCastingMode()
{
	(_bIsInCastingMode) ? DisableCastingMode() : EnableCastingMode();
}
#pragma endregion // Input Actions

#pragma region // Handle Events
void AAresCharacter::HandleMontageNotifies(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if(NotifyName == FName("DeathEnd"))
	{
		GetMesh()->SetVisibility(false);
		OnPlayerDead.ExecuteIfBound();
		return;
	}

	if(NotifyName == FName("CastSpell"))
	{
		if(_CharacterState == ECharacterState::ECS_Casting)
		{
			const EAbilityType AbilityToPerform = (BranchingPointPayload.SequenceAsset == _PlayerData->SingularAttackMontage)
			? EAbilityType::EAT_Singular : EAbilityType::EAT_Combo;
	
			const FVector SpawnLocation =  GetMesh()->GetSocketLocation(FName("Casting_Socket"));
			const FVector CameraForward = _CameraComponent->GetForwardVector();
			_AbilityComponent->PerformAttack(AbilityToPerform, _AttributeComponent->GetAttribute(EAttributeType::EAT_Mana).CurrentValue, SpawnLocation, CameraForward);
			return;
		}
	}
}

void AAresCharacter::HandleMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	_bIsAttacking = false;
}

void AAresCharacter::HandleUpdatedInventory(const TArray<FInventorySlot>& UpdatedInventory) const
{
	OnUpdatedInventory.ExecuteIfBound(UpdatedInventory);
}

void AAresCharacter::HandleUpdateGearEquipped(const FGearSlot& GearEquipped) const
{
	OnNewGearEquipped.ExecuteIfBound(GearEquipped);
	_AttributeComponent->GearEquipped(GearEquipped._GearItem->ItemData.GearData.Attributes);
}

void AAresCharacter::HandleGearUnEquipped(const FGearStats& OldGearStats) const
{
	_AttributeComponent->GearUnEquipped(OldGearStats);
}

void AAresCharacter::HandleGearFoundUpdateUI(const TArray<UDAItem*>& GearFound, EGearType GearType) const
{
	OnListGearsFound.ExecuteIfBound(GearFound, GearType);
}

void AAresCharacter::HandleAbilitySlotUpdate(EAbilitySlot AbilitySlotToUpdate, ESingularAbility SingularAbilityType) const
{
	OnAbilitySlotUpdate.ExecuteIfBound(AbilitySlotToUpdate, SingularAbilityType);
}

void AAresCharacter::HandleAttributesUpdated(const TArray<FAttribute>& Attributes) const
{
	OnAttributesUpdated.ExecuteIfBound(Attributes);
}

void AAresCharacter::HandleConsumeMana(float ConsumeAmount) const
{
	_AttributeComponent->ReduceMana(ConsumeAmount);
}

void AAresCharacter::HandleUpdateHealth(float CurrentHealth)
{
	OnRequestUpdateHealth.ExecuteIfBound(CurrentHealth);
	
	if(CurrentHealth <= 0)
	{
		_AbilityComponent->ResetAllSlots();
		
		if(_bIsInCastingMode)
		{ 	
			_bIsAttacking = false;
			DisableCastingMode();
		}
		
		_bIsAlive = false;
		//Disable character's movement
		_CharacterMovementComponent->SetMovementMode(MOVE_None);
		PlayAnimMontage(_PlayerData->_DeathMontage, 1);
	}
}

void AAresCharacter::HandleUpdateMana(float CurrentMana) const
{
	OnRequestUpdateMana.ExecuteIfBound(CurrentMana);
}

void AAresCharacter::HandleCharacterLevelUp(int32 Level, const TArray<FAttribute>& UpdatedAttributes) const
{
	OnCharacterLevelUp.ExecuteIfBound(Level, UpdatedAttributes);
}
#pragma endregion // // Handle Events

#pragma region // Controller requests

void AAresCharacter::OnRequestDropItem(const int32 SlotIndex) const
{
	_InventoryComponent->DropItem(SlotIndex);
}

void AAresCharacter::OnRequestSwapItem(int32 SourceIndex, int32 DestinationIndex) const
{
	_InventoryComponent->SwapItems(SourceIndex, DestinationIndex);
}

void AAresCharacter::OnRequestEquipGear(UDAItem* GearToEquip) const
{
	_InventoryComponent->EquipGear(GearToEquip);
}

void AAresCharacter::OnRequestFindGear(EGearType FindGearType) const
{
	_InventoryComponent->FindGear(FindGearType);
}

void AAresCharacter::OnRequestRespawn()
{
	_bIsAlive = true;
	GetMesh()->SetVisibility(true);

	//Enable character to move
	_CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	_AttributeComponent->ResetManaHealthValue();
	//Fire Event call to update UI 
	OnRequestUpdateHealth.ExecuteIfBound(_AttributeComponent->GetAttribute(EAttributeType::EAT_Health).CurrentValue);
	OnRequestUpdateMana.ExecuteIfBound(_AttributeComponent->GetAttribute(EAttributeType::EAT_Mana).CurrentValue);
	
	SetActorLocation(_RespawnLocation);
}

void AAresCharacter::OnRequestConsumePotion(int32 SlotIndex, FPotionData PotionData) const
{
	if(_AttributeComponent->ConsumeItem(PotionData))
	{
		if(_InventoryComponent->RemoveItem(SlotIndex))
		{
			OnUpdatedInventory.ExecuteIfBound(_InventoryComponent->GetInventorySlots());
		}
	}
}

void AAresCharacter::OnRequestAbilityLevelUp(ESingularAbility AbilityToLevelUp) const
{
	_AbilityComponent->AbilityLevelUp(AbilityToLevelUp);
}
#pragma endregion // Controller requests

const TArray<FAttribute>& AAresCharacter::GetAllAttributes() const
{
	return _AttributeComponent->GetAllAttributes();
}

TArray<FInventorySlot>& AAresCharacter::GetInventorySlots() const
{
	return _InventoryComponent->GetInventorySlots();
}

