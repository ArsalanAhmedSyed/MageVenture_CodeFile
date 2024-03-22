#pragma once


UENUM(Blueprintable)
enum class EActorType : uint8
{
	EAT_Projectile UMETA(DisplayName = "Projectile"),
	EAT_Trace UMETA(DisplayName = "Trace")
};
