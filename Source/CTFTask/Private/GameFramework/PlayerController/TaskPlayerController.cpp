// This is a task for gamescan made by Daniel Corredor.


#include "GameFramework/PlayerController/TaskPlayerController.h"
#include "UI/TaskHUD.h"

class UTaskOnlineSessionsSubsystem;
/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::BeginPlay()
{
	Super::BeginPlay();
	TaskHUD = Cast<ATaskHUD>(GetHUD());
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::SetHUDHealth(const float Health, const float MaxHealth)
{
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;
	if(TaskHUD && MaxHealth != 0.f)
	{
		const float HealthPercent = Health / MaxHealth;
		TaskHUD->SetHealth(HealthPercent);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::SetHUDFlagVisibility(const bool bIsVisible)
{
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;
	if(TaskHUD)
	{
		TaskHUD->SetFlagVisibility(bIsVisible);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;	
	if (TaskHUD)
	{
		TaskHUD->SetRedTeamScore(RedScore);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;	
	if (TaskHUD)
	{
		TaskHUD->SetBlueTeamScore(BlueScore);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::SetHUDTimer(float Time)
{
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;
	if (TaskHUD)
	{
		TaskHUD->SetTimer(Time);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::SetHUDAnnouncement(const FString AnnouncementText)
{
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;
	if (TaskHUD)
	{
		TaskHUD->SetAnnouncement(AnnouncementText);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerController::UpdateHUDDead(const bool bIsDead)
{
	SetIgnoreMoveInput(bIsDead);
	SetIgnoreLookInput(bIsDead);
	DisableInput(this);
	
	TaskHUD = TaskHUD == nullptr ? Cast<ATaskHUD>(GetHUD()) : TaskHUD;
	if (TaskHUD)
	{
		TaskHUD->SetDead(bIsDead);
	}
}