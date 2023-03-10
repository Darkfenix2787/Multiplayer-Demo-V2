// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "TaskDefinitions.h"
#include "GameFramework/PlayerState.h"
#include "TaskPlayerState.generated.h"

class ATaskCharacter;
/**
 * This is the player state for the player
 */
UCLASS()
class CTFTASK_API ATaskPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:

	/** Character Reference */
	UPROPERTY()
	ATaskCharacter* CharacterRef;	

	UPROPERTY(Replicated, Transient)
	ETeam Team;

	UPROPERTY(ReplicatedUsing = OnRep_IsInputEnabled , Transient)
	uint8 bIsInputEnabled : 1;
	

public:

	ATaskPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION(Client, Unreliable)
	void Client_CharacterCameraFade(const bool bIsFadeIn);
	
	/** Team Getter	 */
	UFUNCTION(BlueprintCallable)
	ETeam GetTeam() const { return Team; }

	/** Team Setter */
	UFUNCTION(BlueprintCallable)
	void SetTeam(const ETeam NewTeam);
	
	UFUNCTION()
	void OnRep_IsInputEnabled();

	UFUNCTION()
	void EnableCharacterInput(const bool IsEnable);
};
