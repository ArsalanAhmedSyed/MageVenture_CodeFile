
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Inventory/Structs/FInventorySlot.h"
#include "Structs/FAttribute.h"
#include "Enums/EInputModeType.h"
#include "AresPlayerController.generated.h"

class UCombatWidget;
class UDeadWidget;
class UPlayerWidget;
class AAresCharacter;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

enum class EAbilitySlot : uint8;
enum class ESingularAbility : uint8;

DECLARE_DELEGATE(FOnPawnAliveStateChangeSignature)

UCLASS(Blueprintable)
class MAGEVENTURE_API AAresPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAresPlayerController();
	void Initialise();

	FOnPawnAliveStateChangeSignature OnPlayerDeadState{};
	FOnPawnAliveStateChangeSignature OnPlayerRespawnState{};

protected:
	
	virtual void SetupInputComponent() override;
	
#pragma region // Enhance Input
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputMappingContext* _IMCPlayer{};

	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _MoveAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _LookAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _JumpAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _SprintAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _CastAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _InteractAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _ToggleInventory{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _SwapAbilityAction{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _CastingMode{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _SelectPrimary{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _SelectSecondary{};
	UPROPERTY(EditDefaultsOnly, Category= "Player Data | Enhance Input")
	UInputAction* _Pause{};
#pragma endregion // Enhance Input
	
private:
	
	TObjectPtr<AAresCharacter> _AresCharacter{};
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem{};

#pragma region // Widgets
	UPROPERTY(EditDefaultsOnly, Category = "Player | Widget")
	TSubclassOf<UPlayerWidget> _PlayerWidget{};
	TObjectPtr<UPlayerWidget> _PlayerWidgetSpawned{};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Widget")
	TSubclassOf<UCombatWidget> _CombatWidget{};
	TObjectPtr<UCombatWidget> _CombatWidgetSpawned{};
	bool _bIsInventoryOpen{false};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Widget")
	TSubclassOf<UDeadWidget> _DeadWidget{};
	TObjectPtr<UDeadWidget> _DeadWidgetSpawned{};

	UPROPERTY(EditDefaultsOnly, Category = "Player | Widget")
	TSubclassOf<UUserWidget> _PauseWidget{};
	TObjectPtr<UUserWidget> _PauseWidgetSpawned{};
	bool _bIsGamePaused{};
#pragma endregion // Widgets

#pragma region // Input Action
	void HandleMoveAction(const FInputActionValue& Value);
	void HandleLookAction(const FInputActionValue& Value);
	void HandleJumpAction(const FInputActionValue& Value);
	void HandleSprintAction(const FInputActionValue& Value);
	void HandleToggleInventoryAction(const FInputActionValue& Value);
	void HandleInteractAction(const FInputActionValue& Value);
	void HandleSwapAbilityAction(const FInputActionValue& Value);
	void HandleSelectPrimary(const FInputActionValue& Value);
	void HandleSelectSecondary(const FInputActionValue& Value);
	void HandleAbilityAttack(const FInputActionValue& Value);
	void HandleCastingMode();
	void HandlePause();
#pragma endregion // Input Action

#pragma region // Events Handle
	void UpdateInventoryUI(const TArray<FInventorySlot>& UpdatedInventory);
	void HandleUpdateGearEquipped(const struct FGearSlot& GearEquipped);
	void HandleGearFoundUpdateUI(const TArray<UDAItem*>& GearFound, EGearType GearType);
	void HandleAttributesUpdated(const TArray<FAttribute>& Attributes);
	void HandleHideInventoryWidget();
	void HandleAbilitySlotUpdate(EAbilitySlot AbilitySlotToUpdate, ESingularAbility SingularAbilityType);
	void HandleUpdateMana(float UpdatedManaValue);
	void HandleUpdateHealth(float UpdatedHealthValue);
	void HandlePlayerDead();
	void HandleCharacterLevelUp(int32 level, const TArray<FAttribute>& UpdatedAttributes);
	void HandleCastingToggle();
	
	/* Request from blueprint widgets*/
	UFUNCTION()
	void HandleOnRequestDropItem(int32 SlotIndex);
	UFUNCTION()
	void HandleOnRequestSwapItem(int32 SourceIndex, int32 DestinationIndex);
	UFUNCTION()
	void HandleOnRequestEquipGear( UDAItem* GearToEquip);
	UFUNCTION()
	void HandleOnRequestFindGear(EGearType FindGearType);
	UFUNCTION()
	void HandleRespawnRequest();
	UFUNCTION()
	void HandleOnRequestConsumeItem(int32 SlotIndex, FPotionData PotionData);
	UFUNCTION()
	void HandleOnRequestAbilityLevelUp(ESingularAbility AbilityToLevelUp);
#pragma endregion // Events Handle
};

