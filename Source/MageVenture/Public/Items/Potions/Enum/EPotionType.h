#pragma once

UENUM(Blueprintable)
enum class EPotionType : uint8
{
	EPT_None UMETA(DisplayName = "None"),
	EPT_HealthRegen UMETA(DisplayName = "Health Regen"),
	EPT_ManaRegen UMETA(DisplayName = "Mana Regen")
};
