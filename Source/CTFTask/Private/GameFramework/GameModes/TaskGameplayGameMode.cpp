// This is a task for gamescan made by Daniel Corredor.


#include "GameFramework/GameModes/TaskGameplayGameMode.h"

#include "DrawDebugHelpers.h"
#include "Character/TaskCharacter.h"
#include "GameFramework/GameState/TaskGameState.h"
#include "GameFramework/PlayerController/TaskPlayerController.h"
#include "GameFramework/PlayerState/TaskPlayerState.h"
#include "Kismet/GameplayStatics.h"

/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskGameplayGameMode::ATaskGameplayGameMode() 
{
	WaitTimeToStarGame = 5.f;	
	GameTime = 60.f;
	Minutes = 0;
	Seconds = 0;
	DeltaTime = 1.f;
	HasGameStarted = false;
	PlayerStartQuantity = 5;
	RedTeamPlayerStartTag = "RedTeamPlayerStart";
	BlueTeamPlayerStartTag = "BlueTeamPlayerStart";
	PathToMainMenu = TEXT("/Game/Maps/MainMenu");
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::Logout(AController* Exiting)
{
	ATaskGameState* TaskGameState = Cast<ATaskGameState>(UGameplayStatics::GetGameState(this));
	ATaskPlayerState* TaskPlayerState = Exiting->GetPlayerState<ATaskPlayerState>();
	if (TaskGameState && TaskPlayerState)
	{
		if (TaskGameState->RedTeam.Contains(TaskPlayerState))
		{
			TaskGameState->RedTeam.Remove(TaskPlayerState);
		}
		if (TaskGameState->BlueTeam.Contains(TaskPlayerState))
		{
			TaskGameState->BlueTeam.Remove(TaskPlayerState);
		}
	}
	
	Super::Logout(Exiting);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::HandleMatchHasStarted()
{	
	Super::HandleMatchHasStarted();

	GameStateRef = Cast<ATaskGameState>(UGameplayStatics::GetGameState(this));

	UWorld* World = GetWorld();
	if(World)
	{
		// Timer to ensure that the pawn is already possess and can change the team color
		FTimerHandle SetTeam_TimerHandle;
		World->GetTimerManager().SetTimer(SetTeam_TimerHandle,this,&ATaskGameplayGameMode::InitCharacter,1.f,false);

		// Game Time	
		World->GetTimerManager().SetTimer(CountDown_TimerHandle,this,&ATaskGameplayGameMode::CountDownTimer, DeltaTime,true);		
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	NewController->StartSpot = GetRandomPlayerStart(NewController);
	Super::InitSeamlessTravelPlayer(NewController);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);

	const ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(NewPlayer->PlayerState);
	ATaskCharacter* TaskCharacter = Cast<ATaskCharacter>(NewPlayer->GetPawn());

	if(TaskPlayerState && TaskCharacter)
	{
		TaskCharacter->Multicast_SetMeshTeamColor(TaskPlayerState->GetTeam());		
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if(EliminatedCharacter)
	{
		EliminatedCharacter->Reset();
		EliminatedCharacter->Destroy();
	}

	if(EliminatedController)
	{
		APlayerController* PlayerController = Cast<APlayerController>(EliminatedController);
		if(PlayerController)
		{
			AActor* PlayerStart = GetRandomPlayerStart(PlayerController);
			RestartPlayerAtPlayerStart(PlayerController,PlayerStart);						
		}
	}		
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
AActor* ATaskGameplayGameMode::GetRandomPlayerStart( AController* NewController)
{
	AActor* StartSpot = nullptr;
	
	const ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(NewController->PlayerState);

	if(TaskPlayerState)
	{
		const ETeam PlayerTeam = TaskPlayerState->GetTeam();		

		const int32 RandomPlayerStart = FMath::RandRange(1,PlayerStartQuantity);
		const FString PlayerStartTagNumber = FString::Printf(TEXT("%d"),RandomPlayerStart);

		if(PlayerTeam == ETeam::ET_BlueTeam)
		{
			StartSpot = FindPlayerStart(NewController,BlueTeamPlayerStartTag + PlayerStartTagNumber);			
		}
		else
		{
			StartSpot = FindPlayerStart(NewController,RedTeamPlayerStartTag + PlayerStartTagNumber);
		}		
	}

	return StartSpot;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::PlayerCapturedFlag(ETeam ZoneTeam) const
{
	if (GameStateRef)
	{
		if (ZoneTeam == ETeam::ET_BlueTeam)
		{
			GameStateRef->BlueTeamScores();
		}
		if (ZoneTeam == ETeam::ET_RedTeam)
		{
			GameStateRef->RedTeamScores();
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::InitCharacter()
{
	if(GameStateRef)
	{
		EnablePlayerInput(false);
		for(APlayerState* PlayerState : GameStateRef->PlayerArray)
		{
			const ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(PlayerState);

			if(TaskPlayerState)
			{
				ATaskCharacter* TaskCharacter = Cast<ATaskCharacter>(TaskPlayerState->GetPawn());

				if(TaskCharacter)
				{
					TaskCharacter->Multicast_SetMeshTeamColor(TaskPlayerState->GetTeam());
				}
			}
		}
	}
}



/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::EnablePlayerInput(const bool bIsEnable) const
{
	GameStateRef == nullptr ? Cast<ATaskGameState>(UGameplayStatics::GetGameState(this)) : GameStateRef;
	for(APlayerState* PlayerState : GameStateRef->PlayerArray)
	{
		ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(PlayerState);

		if(TaskPlayerState)
		{
			TaskPlayerState->EnableCharacterInput(bIsEnable);
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::CountDownTimer()
{
	if(!GameStateRef)
	{
		return;;
	}
	
	//Logic when the game hasn't started
	if(!HasGameStarted)
	{
		WaitTimeToStarGame -= DeltaTime;		
		Seconds = FMath::FloorToInt(WaitTimeToStarGame);
		
		TimeText = FString::Printf(TEXT("%02d"), Seconds);
		GameStateRef->SetMatchAnnouncement(TimeText);

		if(WaitTimeToStarGame == 2.f)
		{
			for(APlayerState* PlayerState : GameStateRef->PlayerArray)
			{
				ATaskPlayerState* TaskPlayerState = Cast<ATaskPlayerState>(PlayerState);

				if(TaskPlayerState)
				{
					TaskPlayerState->Client_CharacterCameraFade(false);
				}
			}			
		}

		if(WaitTimeToStarGame <= 0.f)
		{
			EnablePlayerInput(true);
			HasGameStarted = true;
			GameStateRef->SetMatchAnnouncement("");
		}
		
		return;
	}

	//Logic when the Game starts
	GameTime -= DeltaTime;	

	if(GameTime <= 0)
	{
		const UWorld* World = GetWorld();
		if(World)
		{
			GetWorld()->GetTimerManager().ClearTimer(CountDown_TimerHandle);
		}
		
		EnablePlayerInput(false);

		if (GameStateRef)
		{
			GameStateRef->EndGame();
		}
		
		// Timer to return to the main menu
		FTimerHandle ReturnToMainMenu_TimerHandle;
		World->GetTimerManager().SetTimer(ReturnToMainMenu_TimerHandle,this,&ATaskGameplayGameMode::ReturnToMainMenu,3.f,false);		
	}

	GameStateRef->SetMatchTime(FMath::FloorToInt(GameTime));
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskGameplayGameMode::ReturnToMainMenu()
{	
	UWorld* World = GetWorld();
	if(World)
	{
		// this will disconnect the clients		
		UGameplayStatics::OpenLevel(World,PathToMainMenu);
	}
}