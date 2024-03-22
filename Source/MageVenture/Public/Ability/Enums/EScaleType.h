#pragma once

UENUM(Blueprintable)
enum class EScaleType : uint8
{
	EST_Linear UMETA(DisplayName = "Linear Scale"),
	EST_Exponential UMETA(DisplayName = "Exponential Scale")
};
