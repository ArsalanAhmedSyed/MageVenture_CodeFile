#pragma once

#include "CoreMinimal.h"
#include "Ability/Enums/AbilitySlot.h"
#include "Ability/Enums/AbilityTypes.h"
#include "FAbilityEvent.generated.h"

USTRUCT(Blueprintable)
struct FAbilityEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ability Component")
	EAbilitySlot _AbilitySlotToUpdate {EAbilitySlot::EAS_Hover};
	UPROPERTY(BlueprintReadOnly, Category = "Ability Component")
	ESingularAbility _SingularAbilityType {ESingularAbility::ESA_None};
	UPROPERTY(BlueprintReadOnly, Category = "Ability Component")
	float _ManaConsumed {0};
};
