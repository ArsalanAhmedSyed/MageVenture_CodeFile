#pragma once


UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	EAT_None UMETA(DisplayName = "None"),
	EAT_Singular  UMETA(DisplayName = "Singular Type"),
	EAT_Combo  UMETA(DisplayName = "Combo Type")
};

UENUM(BlueprintType)
enum class ESingularAbility : uint8
{
	ESA_None UMETA(DisplayName = "None"),
	ESA_Fire UMETA(DisplayName = "Fire"),
	ESA_Ice UMETA(DisplayName = "Ice"),
	ESA_Lightning UMETA(DisplayName = "Lightning"),
	ESA_Wind UMETA(DisplayName = "Wind"),
};

UENUM(BlueprintType)
enum class EComboAbility : uint8
{
	ECA_None UMETA(DisplayName = "None"),
	ECA_ScatterFlame  UMETA(DisplayName = "ScatterFlame"),
	ECA_FlamingTornado  UMETA(DisplayName = "FlamingTornado"),
	ECA_ScatterIce  UMETA(DisplayName = "ScatterIce"),
	ECA_BurstLightning UMETA(DisplayName = "BurstLightning")
};