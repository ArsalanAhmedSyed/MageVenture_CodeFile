#pragma once

#include "CoreMinimal.h"
#include "FAbilitiesLevel.generated.h"

USTRUCT(Blueprintable)
struct FAbilitiesLevel
{
	GENERATED_BODY()

	int32 Fire{1};
	int32 Ice{1};
	int32 Lightning{1};
	int32 Wind{1};
};
