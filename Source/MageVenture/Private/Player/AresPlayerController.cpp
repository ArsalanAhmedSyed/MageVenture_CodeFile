
#include "Player/AresPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Other/FunctionsLibrary.h"
#include "widgets/DeadWidget.h"
#include "Player/AresCharacter.h"
#include "Widgets/CombatWidget.h"
#include "Widgets/PlayerWidget.h"

AAresPlayerController::AAresPlayerController() {}

void AAresPlayerController::Initialise()
{
	if(_PlayerWidget == nullptr || _CombatWidget == nullptr || _DeadWidget == nullptr || _PauseWidget == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Not All Widgets have been assigned")));
		return;
	}
	
	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(_IMCPlayer,0);
	}
	
	_AresCharacter = Cast<AAresCharacter>(GetPawn());
	
	_PlayerWidgetSpawned = CreateWidget<UPlayerWidget>(this, _PlayerWidget);
	_CombatWidgetSpawned = CreateWidget<UCombatWidget>(this, _CombatWidget);

	if(_AresCharacter == nullptr || _PlayerWidgetSpawned == nullptr || _CombatWidgetSpawned == nullptr) { return; }
	
	_PlayerWidgetSpawned->AddToViewport(1);
	_PlayerWidgetSpawned->SetVisibility(ESlateVisibility::Collapsed);
	_bIsInventoryOpen = false;
	_CombatWidgetSpawned->AddToViewport();

#pragma region // Ares character Event binding
	_AresCharacter->OnUpdatedInventory.BindUObject(this, &ThisClass::UpdateInventoryUI);
	_AresCharacter->OnNewGearEquipped.BindUObject(this, &ThisClass::HandleUpdateGearEquipped);
	_AresCharacter->OnListGearsFound.BindUObject(this, &ThisClass::HandleGearFoundUpdateUI);
	_AresCharacter->OnAttributesUpdated.BindUObject(this, &ThisClass::HandleAttributesUpdated);
	_AresCharacter->OnInteractHideInventory.BindUObject(this, &ThisClass::HandleHideInventoryWidget);
	_AresCharacter->OnAbilitySlotUpdate.BindUObject(this, &ThisClass::HandleAbilitySlotUpdate);
	_AresCharacter->OnRequestUpdateMana.BindUObject(this, &ThisClass::HandleUpdateMana);
	_AresCharacter->OnRequestUpdateHealth.BindUObject(this, &ThisClass::HandleUpdateHealth);
	_AresCharacter->OnPlayerDead.BindUObject(this, &ThisClass::HandlePlayerDead);
	_AresCharacter->OnCharacterLevelUp.BindUObject(this, &ThisClass::HandleCharacterLevelUp);
	_AresCharacter->OnCastingToggle.BindUObject(this, &ThisClass::HandleCastingToggle);
#pragma endregion // Ares character Event binding

#pragma region //Bind to player widget events
	_PlayerWidgetSpawned->OnRequestItemDrop.AddDynamic(this, &ThisClass::HandleOnRequestDropItem);
	_PlayerWidgetSpawned->OnRequestSwapItem.AddDynamic(this, &ThisClass::HandleOnRequestSwapItem);
	_PlayerWidgetSpawned->OnRequestEquipGear.AddDynamic(this, &ThisClass::HandleOnRequestEquipGear);
	_PlayerWidgetSpawned->OnRequestFindGear.AddDynamic(this, &ThisClass::HandleOnRequestFindGear);
	_PlayerWidgetSpawned->OnRequestConsumeItem.AddDynamic(this, &ThisClass::HandleOnRequestConsumeItem);
	_PlayerWidgetSpawned->OnRequestAbilityLevelUp.AddDynamic(this, &ThisClass::HandleOnRequestAbilityLevelUp);
#pragma endregion //Bind to player widget events
	
	_AresCharacter->Initialise();
	_PlayerWidgetSpawned->InitialiseWidget(_AresCharacter->GetAllAttributes(), _AresCharacter->GetInventorySlots());
	_CombatWidgetSpawned->GetUpdatedAttributes(_AresCharacter->GetAllAttributes());

	UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_GameOnly,_CombatWidgetSpawned);
}

void AAresPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		/*Basic Character Control*/
		EnhancedInputComponent->BindAction(_MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveAction);
		EnhancedInputComponent->BindAction(_LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookAction);
		EnhancedInputComponent->BindAction(_JumpAction, ETriggerEvent::Triggered, this, &ThisClass::HandleJumpAction);
		EnhancedInputComponent->BindAction(_SprintAction, ETriggerEvent::Triggered, this, &ThisClass::HandleSprintAction);

		/*Inventory & Interaction*/
		EnhancedInputComponent->BindAction(_InteractAction, ETriggerEvent::Triggered, this, &ThisClass::HandleInteractAction);
		EnhancedInputComponent->BindAction(_ToggleInventory, ETriggerEvent::Triggered, this, &ThisClass::HandleToggleInventoryAction);
		EnhancedInputComponent->BindAction(_Pause, ETriggerEvent::Triggered, this, &ThisClass::HandlePause);
		
		/*Ability Actions*/
		EnhancedInputComponent->BindAction(_CastingMode, ETriggerEvent::Triggered, this, &ThisClass::HandleCastingMode);
		EnhancedInputComponent->BindAction(_SwapAbilityAction, ETriggerEvent::Triggered, this, &ThisClass::HandleSwapAbilityAction);
		EnhancedInputComponent->BindAction(_SelectPrimary, ETriggerEvent::Triggered, this, &ThisClass::HandleSelectPrimary);
		EnhancedInputComponent->BindAction(_SelectPrimary, ETriggerEvent::Completed, this, &ThisClass::HandleAbilityAttack);
		EnhancedInputComponent->BindAction(_SelectPrimary, ETriggerEvent::Canceled, this, &ThisClass::HandleAbilityAttack);
		EnhancedInputComponent->BindAction(_SelectSecondary, ETriggerEvent::Triggered, this, &ThisClass::HandleSelectSecondary);
	}
}

void AAresPlayerController::HandleMoveAction(const FInputActionValue& Value)
{
	_AresCharacter->PerformMovement(Value);
}

void AAresPlayerController::HandleLookAction(const FInputActionValue& Value)
{
	_AresCharacter->PerformLook(Value);
}

void AAresPlayerController::HandleJumpAction(const FInputActionValue& Value)
{
	_AresCharacter->PerformJump(Value);
}

void AAresPlayerController::HandleSprintAction(const FInputActionValue& Value)
{
	_AresCharacter->PerformSprint(Value);
}

void AAresPlayerController::HandleInteractAction(const FInputActionValue& Value)
{
	_AresCharacter->PerformInteract(Value);	
}

void AAresPlayerController::HandleSwapAbilityAction(const FInputActionValue& Value)
{
	_AresCharacter->PerformSwapAbility(Value);
}

void AAresPlayerController::HandleSelectPrimary(const FInputActionValue& Value)
{
	if(_bIsInventoryOpen == false)
	{
		_AresCharacter->PerformSelectPrimary(Value);
	}
}

void AAresPlayerController::HandleSelectSecondary(const FInputActionValue& Value)
{
	_AresCharacter->PerformSelectSecondary(Value);
}

void AAresPlayerController::HandleAbilityAttack(const FInputActionValue& Value)
{
	_AresCharacter->PerformInitiateAttack(Value);
}

void AAresPlayerController::HandleCastingMode()
{
	_AresCharacter->PerformCastingMode();
}

void AAresPlayerController::HandlePause()
{
	if(_bIsGamePaused == false)
	{
		#pragma region //Pause Game
		if(_PauseWidget == nullptr) { return; }
		_PauseWidgetSpawned = CreateWidget<UUserWidget>(this, _PauseWidget);
		if(_PauseWidgetSpawned)
		{
			SetPause(true);

			UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_GameAndUI, _PauseWidgetSpawned);
		
			_PlayerWidgetSpawned->SetVisibility(ESlateVisibility::Hidden);
			_CombatWidgetSpawned->SetVisibility(ESlateVisibility::Hidden);
			_PauseWidgetSpawned->AddToViewport();

			_bIsGamePaused = true;
		}
		#pragma endregion //Pause Game
	}
	else
	{
		#pragma region //Unpause Game
		if(_PauseWidgetSpawned->IsInViewport())
		{
			SetPause(false);

			UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_GameOnly, nullptr);

			_PauseWidgetSpawned->RemoveFromParent();
			_CombatWidgetSpawned->SetVisibility(ESlateVisibility::Visible);

			_bIsGamePaused = false;
		}
		#pragma endregion //Unpause Game	
	}
}

void AAresPlayerController::HandleToggleInventoryAction(const FInputActionValue& Value)
{
	if(_bIsInventoryOpen == false)
	{
		_PlayerWidgetSpawned->SetVisibility(ESlateVisibility::Visible);

		UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_GameAndUI, _PlayerWidgetSpawned);
		UpdateInventoryUI(_AresCharacter->GetInventorySlots());
	}
	else
	{
		UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_GameOnly, _PlayerWidgetSpawned);
		_PlayerWidgetSpawned->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	_bIsInventoryOpen = !_bIsInventoryOpen;
}

void AAresPlayerController::UpdateInventoryUI(const TArray<FInventorySlot>& UpdatedInventory)
{
	_PlayerWidgetSpawned->UpdateInventory(UpdatedInventory);
}

void AAresPlayerController::HandleUpdateGearEquipped(const FGearSlot& GearEquipped)
{
	_PlayerWidgetSpawned->UpdateGearEquippedUI(GearEquipped);
}

void AAresPlayerController::HandleGearFoundUpdateUI(const TArray<UDAItem*>& GearFound, EGearType GearType)
{
	_PlayerWidgetSpawned->GearFoundUpdateUI(GearFound, GearType);
}

void AAresPlayerController::HandleAttributesUpdated(const TArray<FAttribute>& Attributes)
{
	_PlayerWidgetSpawned->UpdateAttributeUI(Attributes);
	_CombatWidgetSpawned->GetUpdatedAttributes(Attributes);
}

void AAresPlayerController::HandleHideInventoryWidget()
{
	/* No need to change the input mode,
	 * it is being done inside the VendorActor class*/
	
	if(_PlayerWidgetSpawned->IsInViewport())
	{
		_PlayerWidgetSpawned->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AAresPlayerController::HandleAbilitySlotUpdate(EAbilitySlot AbilitySlotToUpdate, ESingularAbility SingularAbilityType)
{
	_CombatWidgetSpawned->UpdateAbilitySlotTexture(AbilitySlotToUpdate, SingularAbilityType);
}

void AAresPlayerController::HandleUpdateMana(float UpdatedManaValue)
{
	_CombatWidgetSpawned->UpdateManaDisplay(UpdatedManaValue);
}

void AAresPlayerController::HandleUpdateHealth(float UpdatedHealthValue)
{
	_CombatWidgetSpawned->UpdateHealthDisplay(UpdatedHealthValue);
}

void AAresPlayerController::HandleCharacterLevelUp(int32 level, const TArray<FAttribute>& UpdatedAttributes)
{
	_PlayerWidgetSpawned->EnableAbilityLevelUp();
	_PlayerWidgetSpawned->UpdateAttributeUI(UpdatedAttributes);

	_CombatWidgetSpawned->UpdateLevelDisplay(level);
	_CombatWidgetSpawned->GetUpdatedAttributes(UpdatedAttributes);
}

void AAresPlayerController::HandleCastingToggle()
{
	_CombatWidgetSpawned->ToggleCrosshair();
}

void AAresPlayerController::HandleOnRequestAbilityLevelUp(ESingularAbility AbilityToLevelUp)
{
	_AresCharacter->OnRequestAbilityLevelUp(AbilityToLevelUp);
}

void AAresPlayerController::HandleOnRequestDropItem(int32 SlotIndex)
{
	_AresCharacter->OnRequestDropItem(SlotIndex);
}

void AAresPlayerController::HandleOnRequestSwapItem(int32 SourceIndex, int32 DestinationIndex)
{
	_AresCharacter->OnRequestSwapItem(SourceIndex, DestinationIndex);
}

void AAresPlayerController::HandleOnRequestEquipGear(UDAItem* GearToEquip)
{
	_AresCharacter->OnRequestEquipGear(GearToEquip);
}

void AAresPlayerController::HandleOnRequestFindGear(EGearType FindGearType)
{
	_AresCharacter->OnRequestFindGear(FindGearType);
}

void AAresPlayerController::HandlePlayerDead()
{
	Subsystem->RemoveMappingContext(_IMCPlayer);

	_DeadWidgetSpawned = CreateWidget<UDeadWidget>(this, _DeadWidget);
	_DeadWidgetSpawned->OnRequestRespawn.AddDynamic(this, &ThisClass::HandleRespawnRequest);

	//Remove any existing widget in the viewport
	if(_CombatWidgetSpawned->IsInViewport()) { _CombatWidgetSpawned->SetVisibility(ESlateVisibility::Collapsed); }
	if(_PlayerWidgetSpawned->IsInViewport()) { _PlayerWidgetSpawned->SetVisibility(ESlateVisibility::Collapsed); _bIsInventoryOpen = false; }

	UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_UIOnly, _DeadWidgetSpawned);
	_DeadWidgetSpawned->AddToViewport();

	//Fire event call to Game mode
	OnPlayerDeadState.ExecuteIfBound();
}

void AAresPlayerController::HandleRespawnRequest()
{
	if(_DeadWidgetSpawned->IsInViewport())
	{
		_DeadWidgetSpawned->RemoveFromParent();
		_DeadWidgetSpawned->Destruct();
	}

	_CombatWidgetSpawned->SetVisibility(ESlateVisibility::Visible);
	_AresCharacter->OnRequestRespawn();
	//Set default input
	Subsystem->AddMappingContext(_IMCPlayer, 0);
	
	UFunctionsLibrary::ChangeInputMode(this, this, EInputModeType::EIMT_GameOnly, nullptr);

	//Fire event to Game mode
	OnPlayerRespawnState.ExecuteIfBound();
}

void AAresPlayerController::HandleOnRequestConsumeItem(int32 SlotIndex, FPotionData PotionData)
{
	_AresCharacter->OnRequestConsumePotion(SlotIndex, PotionData);
}