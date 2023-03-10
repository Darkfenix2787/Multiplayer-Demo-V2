// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModes/TaskGameMode.h"
#include "TaskLobbyGameMode.generated.h"

/**
 * GameMOde Class for the Lobby level
 */
UCLASS()
class CTFTASK_API ATaskLobbyGameMode : public ATaskGameMode
{
	GENERATED_BODY()

public:

	ATaskLobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	/** Function to set the team for each player */
	void SetTeam(AController* PlayerController);

protected:

	/** Function to travel to the Game when the players are ready */
	void TravelToGame();

	/** Function to keep the time when the first players arrives to the lobby  */
	void WaitForPlayer();

	/** Time that the first player will wait until a second player joins */
	float WaitTimeForPlayer;

	/** Timer DeltaTime */
	float DeltaTime;

	/** WaitPlayer Timer Handle */
	FTimerHandle WaitPlayerTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	FString PathToMainMenu{TEXT("")};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString PathToGame{TEXT("")};
};