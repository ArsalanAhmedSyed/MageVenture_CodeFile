#pragma once

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	EAT_Health UMETA(DisplayName = "Health"),
	EAT_Mana UMETA(DisplayName = "Mana"),
	EAT_Attack UMETA(DisplayName = "Attack"),
	EAT_Defence UMETA(DisplayName = "Defence")
};
