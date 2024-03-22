#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_None UMETA(DisplayName = "None"),
	EIT_Potion UMETA(DisplayName = "Potion"),
	EIT_Gear UMETA(DisplayName = "Gear"),
	EIT_TrashLoot UMETA(DisplayName = "Trash Loot")
};
