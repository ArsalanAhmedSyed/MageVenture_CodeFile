#pragma once

#include "CoreMinimal.h"
#include "Player/Enums/EAttributeType.h"
#include "Ability/Strcuts/FPropertyScale.h"
#include "FAttribute.generated.h"

USTRUCT(Blueprintable)
struct FAttribute
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	FPropertyScale BaseValue;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute", meta =(DisplayPriority="0"))
	EAttributeType Attribute{};

	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	float MaxValue{};
	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	float CurrentValue{};
	
	void Initialise()
	{
		MaxValue = BaseValue.BaseValue;
		CurrentValue = MaxValue;
	}

	void LevelUpProperties(int32 Level)
	{
		MaxValue -= BaseValue.BaseValue;
		
		const float IncrementValue = BaseValue.GetValueAtLevel(Level);

		MaxValue += IncrementValue;
		CurrentValue = MaxValue;
	}
};
