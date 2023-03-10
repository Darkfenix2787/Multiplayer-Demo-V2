// This is a task for gamescan made by Daniel Corredor.
#pragma once

// This will handle the projects definitions

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_RedTeam UMETA(DisplayName = "RedTeam"),
	ET_BlueTeam UMETA(DisplayName = "BlueTeam"),
	ET_NoTeam UMETA(DisplayName = "NoTeam"),
	ET_MaxTeam UMETA(DisplayName = "DeaultMAX")
};