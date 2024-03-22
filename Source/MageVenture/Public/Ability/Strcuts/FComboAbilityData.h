#pragma once

#include "CoreMinimal.h"
#include "Ability/Enums/AbilityTypes.h"
#include "Ability/Data/AbilityData.h"
#include "FComboAbilityData.generated.h"

class ABaseAbility;

USTRUCT(Blueprintable)
struct FComboAbilityData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data")
	EComboAbility ComboAbilityType {EComboAbility::ECA_None};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data")
	TSubclassOf<ABaseAbility> SpawnAbilityActor {nullptr};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data")
	TObjectPtr<UAbilityData> AbilityData {nullptr};	
};
