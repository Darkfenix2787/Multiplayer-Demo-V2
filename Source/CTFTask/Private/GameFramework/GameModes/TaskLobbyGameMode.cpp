// This is a task for gamescan made by Daniel Corredor.


#include "GameFramework/GameModes/TaskLobbyGameMode.h"

#include "TaskDefinitions.h"
#include "Engine/TaskOnlineSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameState/TaskGameState.h"
#include "GameFramework/PlayerState/TaskPlayerState.h"
#include "Kismet/GameplayStatics.h"


/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskLobbyGameMode::ATaskLobbyGameMode()
{
	WaitTimeForPlayer = 180.f;
	DeltaTime = 0.1f;
	PathToMainMenu = TEXT("/Game/Maps/MainMenu");
	PathToGame = TEXT("/Game/Maps/GamePlay");	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SetTeam(NewPlayer);
	
	const int32 NumOfPlayers = GetNumPlayers();
	const UGameInstance* GameInstance = GetGameInstance();
	
	const UWorld* World = GetWorld();
	if(World)
	{
		World->GetTimerManager().SetTimer(WaitPlayerTimerHandle,this,&ThisClass::WaitForPlayer,DeltaTime,true);
	
		if(IsValid(GameInstance))
		{
			PathToGame = FString::Printf(TEXT("%s?listen"), *PathToGame);
			PathToMainMenu = FString::Printf(TEXT("%s?listen"), *PathToMainMenu);
		
			const UTaskOnlineSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UTaskOnlineSessionsSubsystem>();
			check(Subsystem);
	
			if(NumOfPlayers == Subsystem->DesiredNumPublicConnections)
			{
				ATaskGameState* GameStateRef = Cast<ATaskGameState>(UGameplayStatics::GetGameState(this));
				if(GameStateRef)
				{
					for(APlayerState* PlayerState : GameStateRef->PlayerArray)
					{
						ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(PlayerState);

						if(TaskPlayerState)
						{
							TaskPlayerState->Client_CharacterCameraFade(true);							
						}
					}
				}
				
				if (World)
				{
					World->GetTimerManager().ClearTimer(WaitPlayerTimerHandle);					
					FTimerHandle TravelToLobbyTimerHandle;
					World->GetTimerManager().SetTimer(TravelToLobbyTimerHandle,this,&ThisClass::TravelToGame,2.f,false);
				}
			}
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskLobbyGameMode::WaitForPlayer()
{
	WaitTimeForPlayer -= DeltaTime;	

	if(WaitTimeForPlayer <= 0.f)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(WaitPlayerTimerHandle);
			World->ServerTravel(PathToMainMenu);
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskLobbyGameMode::TravelToGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		bUseSeamlessTravel = true;
		World->ServerTravel(PathToGame);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskLobbyGameMode::Logout(AController* Exiting)
{
	const UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		UTaskOnlineSessionsSubsystem* OnlineSessionsSubsystem = GameInstance->GetSubsystem<UTaskOnlineSessionsSubsystem>();
		if(OnlineSessionsSubsystem)
		{
			OnlineSessionsSubsystem->DestroySession();
		}
	}
	
	Super::Logout(Exiting);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskLobbyGameMode::SetTeam(AController* PlayerController)
{
	ATaskGameState* GameStateRef = Cast<ATaskGameState>(UGameplayStatics::GetGameState(this));

	if(GameStateRef)
	{
		for(APlayerState* PlayerState : GameStateRef->PlayerArray)
		{
			ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(PlayerState);

			if(TaskPlayerState && TaskPlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (GameStateRef->RedTeam.Num() >= GameStateRef->BlueTeam.Num())
				{
					GameStateRef->BlueTeam.AddUnique(TaskPlayerState);
					TaskPlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
				else
				{
					GameStateRef->RedTeam.AddUnique(TaskPlayerState);
					TaskPlayerState->SetTeam(ETeam::ET_RedTeam);					
				}
			}
		}		
	}
}
