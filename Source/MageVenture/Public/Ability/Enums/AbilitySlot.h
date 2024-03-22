#pragma once

UENUM(Blueprintable)
enum class  EAbilitySlot : uint8 
{
	EAS_Primary UMETA(DisplayName = "Primary"),
	EAS_Hover UMETA(DisplayName = "Hover"),
	EAS_Secondary UMETA(DisplayName = "Secondary")
};
