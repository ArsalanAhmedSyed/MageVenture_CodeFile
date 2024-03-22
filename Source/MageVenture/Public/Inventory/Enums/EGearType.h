#pragma once

UENUM(Blueprintable)
enum class EGearType : uint8
{
	EGT_None UMETA(DisplayName = "None Gear"),
	EGT_Head UMETA(DisplayName = "Head Gear"),
	EGT_Chest UMETA(DisplayName = "Chest Gear"),
	EGT_Leg UMETA(DisplayName = "Leg Gear")
};
