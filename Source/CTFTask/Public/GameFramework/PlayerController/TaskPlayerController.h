// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TaskPlayerController.generated.h"

class ATaskHUD;
/**
 * Character Player Controller
 */
UCLASS()
class CTFTASK_API ATaskPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY()
	ATaskHUD * TaskHUD;	

public:

	// UI Setters
	void SetHUDHealth(const float Health, const float MaxHealth);

	void SetHUDFlagVisibility(const bool bIsVisible);

	void SetHUDRedTeamScore(int32 RedScore);
	
	void SetHUDBlueTeamScore(int32 BlueScore);

	void SetHUDTimer(float Time);
	
	void SetHUDAnnouncement(const FString AnnouncementText);

	void UpdateHUDDead(const bool bIsDead);

	void DestroyClientSession();
	
};
