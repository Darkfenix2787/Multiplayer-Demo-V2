// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TaskGameState.generated.h"

class ATaskPlayerState;

/**
 * The GameState handle the current state of the game
 */
UCLASS()
class CTFTASK_API ATaskGameState : public AGameState
{
	GENERATED_BODY()

public:

	// Properties

	/** Stores the Red team players */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<ATaskPlayerState*> RedTeam;

	/** Stores the Blue team players */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<ATaskPlayerState*> BlueTeam;

	/** Stores the Red Team Score  */
	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore;
	
	/** Stores the Blue Team Score */
	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore;

	/** Stores the MatchTime */
	UPROPERTY(ReplicatedUsing = OnRep_MatchTime)
	float MatchTime;

	/** Match announcement to be displayed */
	UPROPERTY(ReplicatedUsing = OnRep_MatchAnnouncement)
	FString MatchAnnouncement;

	// Functions

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Function to handle the Red Team Score */
	void RedTeamScores();

	/** Function to handle the Blue Team Score */
	void BlueTeamScores();

	// Function to handle the match time
	void SetMatchTime(const float Time);

	float GetMatchTime() const { return MatchTime; }

	void SetMatchAnnouncement (const FString AnnouncementText);

	// Function to handle the End of the game
	void EndGame();	

protected:
	
	UFUNCTION()
	void OnRep_RedTeamScore() const;

	UFUNCTION()
	void OnRep_BlueTeamScore() const;

	UFUNCTION()
	void OnRep_MatchTime() const;

	UFUNCTION()
	void OnRep_MatchAnnouncement() const;
	
};
