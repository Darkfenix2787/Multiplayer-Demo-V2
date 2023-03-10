// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "TaskDefinitions.h"
#include "TaskGameMode.h"
#include "TaskGameplayGameMode.generated.h"

class ATaskGameState;
/**
 * GameMode Class for the Gameplay Level
 */
UCLASS()
class CTFTASK_API ATaskGameplayGameMode : public ATaskGameMode
{
	GENERATED_BODY()

protected:	
	
	/** Time that will pass when the players arrive to the gameplay map to start the game */
	float WaitTimeToStarGame;

	/** Play time for the game  */
	float GameTime;

	/** Delta for the timers */
	float DeltaTime;

	/** Match Seconds */
	int32 Seconds;

	/** Match Minutes */
	int32 Minutes;

	/** Match Time to display */
	FString TimeText;

	/** This Flag indicates that the game has started */
	bool HasGameStarted;

	/** Quantity Of Player Starts */
	UPROPERTY(EditDefaultsOnly)
	int32 PlayerStartQuantity;

	/** Red Team Player State Tag Used to find where the player starts */
	UPROPERTY(EditDefaultsOnly)
	FString RedTeamPlayerStartTag;

	/** Red Team Player State Tag Used to find where the player starts */
	UPROPERTY(EditDefaultsOnly)
	FString BlueTeamPlayerStartTag;

	/** Game Time TimerHandle*/
	FTimerHandle CountDown_TimerHandle;

	/** A GameState Reference */
	ATaskGameState* GameStateRef;

	UPROPERTY(EditDefaultsOnly)
	FName PathToMainMenu;

public:
	
	ATaskGameplayGameMode();

	virtual void Logout(AController* Exiting) override;	

	virtual void HandleMatchHasStarted() override;

	virtual void InitSeamlessTravelPlayer(AController* NewController) override;

	/** This function is overriden to handle the character values after it restarts */
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;

	/** Function to Init the Character */
	void InitCharacter();

	/** Function to return to main menu */
	void ReturnToMainMenu();

	/** Function to enable/disable the player's input */
	void EnablePlayerInput(const bool bIsEnable) const;

	/** Function that handle the Game time flow */
	void CountDownTimer();

	/** Function to respawn a eliminated character */
	void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);

	/** Returns a random player start */
	AActor* GetRandomPlayerStart(AController* NewController);

	/** Handle the player Captured flag behaviour */
	void PlayerCapturedFlag(ETeam ZoneTeam) const;
};
