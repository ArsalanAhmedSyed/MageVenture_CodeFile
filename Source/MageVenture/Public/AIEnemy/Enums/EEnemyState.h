#pragma once

UENUM(Blueprintable)
enum class EEnemyState : uint8
{
	EES_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EES_Occupied UMETA(DisplayName = "Occupied"),
	EES_Attack UMETA(DisplayName = "Attack"),
	EES_Dead UMETA(DisplayName = "Dead")
};
