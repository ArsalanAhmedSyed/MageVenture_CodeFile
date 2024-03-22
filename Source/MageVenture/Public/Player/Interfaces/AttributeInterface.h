
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Player/Structs/FAttribute.h"
#include "Player/Enums/EAttributeType.h"
#include "AttributeInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAttributeInterface : public UInterface
{
	GENERATED_BODY()
};


class MAGEVENTURE_API IAttributeInterface
{
	GENERATED_BODY()

public:
	virtual FAttribute GetInterfaceAttribute(EAttributeType AttributeType) = 0;
	virtual void GiveEXP(float EXPAmount) {};
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsAlive();
};
