#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/AttributeComponent.h"
#include "DataAsset/PlayerData.h"
#include "Enums/ECharacterState.h"
#include "Interfaces/AttributeInterface.h"
#include "Inventory/Structs/FGearSlot.h"
#include "Inventory/Structs/FInventorySlot.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AresCharacter.generated.h"

class UAbilityComponent;
class UInventoryComponent;
class UAttributeComponent;
class UCameraComponent;
class USpringArmComponent;
class UCharacterMovementComponent;

enum class EAbilitySlot : uint8;
enum class ESingularAbility : uint8;

#pragma  region // Delegates
DECLARE_DELEGATE_OneParam(FUpdateInventorySignature, const TArray<FInventorySlot>&)
DECLARE_DELEGATE_OneParam(FOnGearEquippedSignature, const FGearSlot&)
DECLARE_DELEGATE_TwoParams(FOnGearsFoundSingature, const TArray<UDAItem*>&, EGearType)
DECLARE_DELEGATE(FOnInteractHideInventorySignature)

DECLARE_DELEGATE_OneParam(FOnAttributesUpdatedSignature, const TArray<FAttribute>& )
DECLARE_DELEGATE_OneParam(FOnValueChange, float)
DECLARE_DELEGATE(FOnPlayerDeadSignature)
DECLARE_DELEGATE_TwoParams(FOnCharacterLevelUpSignature, int32, const TArray<FAttribute>& )

DECLARE_DELEGATE_TwoParams(FOnAbilitySlotUpdateSignature, EAbilitySlot, ESingularAbility)

DECLARE_DELEGATE(FOnToggleSignature)
#pragma  endregion // Delegates

UCLASS()
class MAGEVENTURE_API AAresCharacter : public ACharacter, public IAttributeInterface
{
	GENERATED_BODY()

public:
	AAresCharacter();
	void Initialise();

#pragma region // Events
	FUpdateInventorySignature OnUpdatedInventory;
	FOnGearEquippedSignature OnNewGearEquipped;
	FOnGearsFoundSingature OnListGearsFound;
	FOnInteractHideInventorySignature OnInteractHideInventory;

	FOnAttributesUpdatedSignature OnAttributesUpdated;
	FOnValueChange OnRequestUpdateHealth;
	FOnPlayerDeadSignature OnPlayerDead;
	FOnValueChange OnRequestUpdateMana;
	FOnCharacterLevelUpSignature OnCharacterLevelUp;

	FOnAbilitySlotUpdateSignature OnAbilitySlotUpdate{};

	FOnToggleSignature OnCastingToggle;
#pragma endregion // Events

#pragma region // Controller Requests
	void OnRequestDropItem(const int32 SlotIndex) const;
	void OnRequestSwapItem(int32 SourceIndex, int32 DestinationIndex) const;
	void OnRequestEquipGear(UDAItem* GearToEquip) const;
	void OnRequestFindGear(EGearType FindGearType) const;
	void OnRequestAbilityLevelUp(ESingularAbility AbilityToLevelUp) const;
	void OnRequestConsumePotion(int32 SlotIndex, FPotionData PotionData) const;
	void OnRequestRespawn();

	/*Getter Functions to Initialise the Attributes and inventory UI*/ 
	const TArray<FAttribute>& GetAllAttributes() const;
	TArray<FInventorySlot>& GetInventorySlots() const; 
#pragma endregion // Controller Requests

#pragma region // Controllers Input Action 
	void PerformMovement(const FInputActionValue& Value);
	void PerformLook(const FInputActionValue& Value);
	void PerformJump(const FInputActionValue& Value);
	void PerformSprint(const FInputActionValue& Value);
	void PerformSelectPrimary(const FInputActionValue& Value);
	void PerformInteract(const FInputActionValue& Value);
	void PerformSwapAbility(const FInputActionValue& Value) const;
	void PerformSelectSecondary(const FInputActionValue& Value) const;
	void PerformInitiateAttack(const FInputActionValue& Value);
	void PerformCastingMode();
#pragma endregion // Controllers Input Action
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,AActor* DamageCauser) override;
	void StartCameraTimer();

	/*Interfaces*/
	virtual FAttribute GetInterfaceAttribute(EAttributeType AttributeType) override;
	virtual void GiveEXP(float EXPAmount) override { _AttributeComponent->GainExp(EXPAmount); }
private:

#pragma region // Components
	UPROPERTY(VisibleAnywhere, DisplayName = "Component")
	TObjectPtr<USpringArmComponent> _SpringArmComponent{};
	UPROPERTY(VisibleAnywhere, DisplayName = "Component")
	TObjectPtr<UCameraComponent> _CameraComponent{};
	UPROPERTY(VisibleAnywhere, DisplayName = "Component")
	TObjectPtr<UAttributeComponent> _AttributeComponent{};
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryComponent> _InventoryComponent{};
	UPROPERTY(VisibleAnywhere, DisplayName = "Component")
	TObjectPtr<UAbilityComponent> _AbilityComponent{};
#pragma endregion // Components

#pragma region // Event Handle
	void HandleUpdatedInventory(const TArray<FInventorySlot>& UpdatedInventory) const;
	void HandleUpdateGearEquipped(const FGearSlot& GearEquipped) const;
	void HandleGearUnEquipped(const FGearStats& OldGearStats) const;
	void HandleGearFoundUpdateUI(const TArray<UDAItem*>& GearFound, EGearType GearType) const;
	void HandleAttributesUpdated(const TArray<FAttribute>& Attributes) const;
	void HandleAbilitySlotUpdate(EAbilitySlot AbilitySlotToUpdate, ESingularAbility SingularAbilityType) const;
	void HandleConsumeMana(float ConsumeAmount) const;
	void HandleUpdateHealth(float CurrentHealth);
	void HandleUpdateMana(float CurrentMana) const;
	void HandleCharacterLevelUp(int32 Level, const TArray<FAttribute>& UpdatedAttributes) const;
	UFUNCTION()
	void HandleMontageNotifies( FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	UFUNCTION()
	void HandleMontageEnd(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion // Event Handle
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
    TObjectPtr<UPlayerData> _PlayerData{};
	ECharacterState _CharacterState{};
	bool _bIsAlive{ true };
	FVector _RespawnLocation{};

#pragma region //Casting
	void EnableCastingMode();
	void DisableCastingMode();
	void ChangeRotationMode() const;
	bool _bIsInCastingMode{};
	bool _bIsAttacking{};
#pragma endregion //Casting

#pragma region // Movement Speed Timer & Change rate
	bool _bIsSprinting{false};
	TObjectPtr<UCharacterMovementComponent> _CharacterMovementComponent{};
	float _ChangeWalkSpeed{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Timer", meta=(ToolTip="Determind how fast the character switches its movement (Movement such as sprinting and walking)."))
	float _ChangeWalkLerpRate{0.1f};
	
	FTimerHandle _ChangeSpeedTimerHandle{};
	float _MovementTimerRate{0.01f};
	void SpeedChangeLerp();
#pragma endregion // Movement Speed change

#pragma region //Camera Zoom Timer & Change rate
	float _DefaultTargetArmLength{};
	FVector _DefaultSocketPosition{};
	
	FTimerHandle _CameraZoomTimerHandle;
	float _CameraTimerRate{0.01f};
	
	void ChangeCameraPosition();
	bool LerpCameraPosition(float NewTargetArmLength, const FVector& NewSocketOffset);
#pragma endregion //Camera Zoom Timer & Change rate

#pragma region // Interaction properties
	UPROPERTY(EditDefaultsOnly, Category = "Player | Interaction", meta=(ToolTip="Set how far away from the player should trace start."))
	float _ForwardDistance{20.f};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Interaction")
	float _TraceHeight{40.f};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Interaction")
	float _TraceRadius{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Interaction", meta=(ToolTip="This should be set to the interaction trace response"))
	TEnumAsByte<ETraceTypeQuery> _TraceResponseType{};
	UPROPERTY(EditDefaultsOnly, Category = "Player | Interaction")
	TEnumAsByte<EDrawDebugTrace::Type> _DrawDebugTrace{};
#pragma endregion // Interaction properties

public:
	FORCEINLINE ECharacterState GetCharacterState() { return _CharacterState; }
};

