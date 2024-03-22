#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Structs/FGearStats.h"
#include "Player/Structs/FAttribute.h"
#include "AttributeComponent.generated.h"

struct FPotionData;

DECLARE_DELEGATE_OneParam(FOnAttributeUpdatedSignature, const TArray<FAttribute>&)
DECLARE_DELEGATE_OneParam(FOnValueUpdateSignature, float)
DECLARE_DELEGATE_TwoParams(FOnLevelUpSignature, int32, const TArray<FAttribute>& UpdatedAttributes)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAGEVENTURE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

#pragma region // Events...
	FOnAttributeUpdatedSignature OnAttributeUpdated;
	FOnLevelUpSignature OnCharacterLevelUp;
	FOnValueUpdateSignature OnManaValueUpdate;
	FOnValueUpdateSignature OnHealthValueUpdate;
#pragma endregion // Events...
	
	UAttributeComponent();
	void Initialise(const TArray<FAttribute>& ListOfAttributes, FPropertyScale EXPRequired);
	
	void GearEquipped(const struct FGearStats AddGearStats);
	void GearUnEquipped(const struct FGearStats RemoveGearStats);

	void ReduceMana(float ConsumeAmount);
	void ReduceHealth(float DamageAmount);
	bool ConsumeItem(FPotionData PotionData);
	void ResetManaHealthValue();
	
	void GainExp(float ExpAmount);
	
private:
	TArray<FAttribute> _ListOfAttributes{};
	FAttribute& SetAttributeValue(EAttributeType Attribute);
	bool ApplyConsumePotion(float PotionValue, FAttribute& Attribute);

	int32 _CurrentLevel{1};
	FPropertyScale _EXPRequired{};
	float _CurrentExp{};

	/*Events*/
	void FireUpdateAttributes() const;
	void FireUpdateHealth();
	void FireUpdateMana();
	
public:
	FAttribute GetAttribute(EAttributeType AttributeToGet);
	
	const TArray<FAttribute>& GetAllAttributes() { return _ListOfAttributes; }
};
