#pragma once

#include "CoreMinimal.h"
#include "Ability/Enums/EScaleType.h"
#include "FPropertyScale.generated.h"

USTRUCT(Blueprintable)
struct FPropertyScale
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data | Scale")
	EScaleType ScaleType{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data | Scale")
	float ScaleFactor{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Data | Scale")
	float BaseValue{};
	

	float GetValueAtLevel(int8 Level) const
	{
		if(ScaleType == EScaleType::EST_Linear)
		{
			 return  FMath::Abs((ScaleFactor * Level) + BaseValue);
		}
		else if(ScaleType == EScaleType::EST_Exponential)
		{
			 return  FMath::Abs(BaseValue * FMath::Pow(ScaleFactor,Level));
		}
		
		return INDEX_NONE;
	}
};
