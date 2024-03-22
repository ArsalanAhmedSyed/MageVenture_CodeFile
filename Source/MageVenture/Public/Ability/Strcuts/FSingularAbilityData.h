#pragma once

#include "CoreMinimal.h"
#include "Ability/Enums/AbilityTypes.h"
#include "Ability/Data/AbilityData.h"
#include "FSingularAbilityData.generated.h"

USTRUCT(Blueprintable)
struct FSingularAbilityData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data")
	ESingularAbility SingularAbilityType{ESingularAbility::ESA_None};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data")
	TSubclassOf<ABaseAbility> SpawnAbilityActor{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data")
	TObjectPtr<UAbilityData> AbilityData{nullptr};	
};
