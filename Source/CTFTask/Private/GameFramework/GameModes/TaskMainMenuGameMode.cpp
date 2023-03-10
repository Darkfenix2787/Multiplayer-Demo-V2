// This is a task for gamescan made by Daniel Corredor.

#include "Engine/TaskOnlineSessionsSubsystem.h"
#include "GameFramework/GameModes/TaskMainMenuGameMode.h"

#include "GameFramework/PlayerController/TaskPlayerController.h"
#include "Kismet/GameplayStatics.h"

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskMainMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	// To be sure that there is not a session opened
	const UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		UTaskOnlineSessionsSubsystem* OnlineSessionsSubsystem = GameInstance->GetSubsystem<UTaskOnlineSessionsSubsystem>();
		if(OnlineSessionsSubsystem)
		{
			OnlineSessionsSubsystem->DestroySession();
		}
	}
	
	Super::PostLogin(NewPlayer);
}