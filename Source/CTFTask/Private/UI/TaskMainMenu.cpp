// This is a task for gamescan made by Daniel Corredor.


#include "UI/TaskMainMenu.h"

#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/TaskOnlineSessionsSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/TaskSessionObject.h"


/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::MenuSetup(FString LobbyPath, int32 NumOfPlayers)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumOfPlayers;
	MatchType = FString::Printf(TEXT("TaskCaptureTheFlag"));
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	const UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	const UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UTaskOnlineSessionsSubsystem>();
	}

	if(OnlineSessionsSubsystem)
	{
		OnlineSessionsSubsystem->MultiplayerOnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);
		OnlineSessionsSubsystem->MultiplayerOnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessions);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
bool UTaskMainMenu::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(CreateGameButton)
	{
		CreateGameButton->OnClicked.AddDynamic(this, &ThisClass::CreateGameButtonClicked);
	}

	if(BrowseGamesButton)
	{
		BrowseGamesButton->OnClicked.AddDynamic(this, &ThisClass::BrowseGamesButtonClicked);
	}
	
	if(QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::QuitGameButtonClicked);
	}
	
	return true;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			const FInputModeGameOnly InputModeGameOnlyData;
			PlayerController->SetInputMode(InputModeGameOnlyData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::EnableButtons(const bool bIsActive)
{
	CreateGameButton->SetIsEnabled(bIsActive);
	BrowseGamesButton->SetIsEnabled(bIsActive);
	QuitGameButton->SetIsEnabled(bIsActive);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::TravelToLobby()
{
	UWorld* World = GetWorld();
	if(World)
	{
		World->ServerTravel(PathToLobby);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::CreateGameButtonClicked()
{
	EnableButtons(false);
	ContextMenuSwitcher->SetActiveWidgetIndex(0.f);	
	CreateSessionTextBlock->SetText(FText::FromString(TEXT("Creating Session")));	
	
	if(OnlineSessionsSubsystem)
	{
		OnlineSessionsSubsystem->CreateSession(NumPublicConnections,MatchType);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::BrowseGamesButtonClicked()
{
	EnableButtons(false);
	ContextMenuSwitcher->SetActiveWidgetIndex(1.f);
	FindSessionTextBlock->SetText(FText::FromString(TEXT("Searching for Sessions")));
	
	if(OnlineSessionsSubsystem)
	{
		OnlineSessionsSubsystem->FindSessions(10000);
	}	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::QuitGameButtonClicked()
{
	const UWorld* World = GetWorld();

	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			UKismetSystemLibrary::QuitGame(World, PlayerController,EQuitPreference::Quit,true);		
		}		
	}
}


/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::OnCreateSession(bool bWasSuccessful)
{
	if(!bWasSuccessful)
	{
		EnableButtons(true);
		CreateSessionTextBlock->SetText(FText::FromString(TEXT("Create Session Failed")));
		return;
	}
	
	UWorld* World = GetWorld();
	if(World)
	{
		CreateSessionTextBlock->SetText(FText::FromString(TEXT("Traveling To Lobby")));
		FTimerHandle TravelToLobbyTimerHandle;
		World->GetTimerManager().SetTimer(TravelToLobbyTimerHandle,this,&ThisClass::TravelToLobby,2.f,false);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskMainMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	EnableButtons(true);
	
	if(!bWasSuccessful)
	{
		FindSessionTextBlock->SetText(FText::FromString(TEXT("No Session available")));
		return;
	}

	FindSessionTextBlock->SetText(FText::FromString(TEXT("")));
	
	// This logic will fill the server list with the available sessions
	ServerListScrollBox->ClearChildren();
	
	if(OnlineSessionsSubsystem == nullptr)
	{
		return;
	}

	int32 SessionQuantity = 0;
	
	for(FOnlineSessionSearchResult CurrentResult : SessionResults)
	{
		FString SettingsValue;
		CurrentResult.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);		
		
		if(SettingsValue == MatchType)
		{
			SessionQuantity++;
			if(SessionObjectWidget)
			{
				UTaskSessionObject* SessionObjectWidgetRef = CreateWidget<UTaskSessionObject>(this,SessionObjectWidget);
				SessionObjectWidgetRef->Setup(CurrentResult);
				ServerListScrollBox->AddChild(SessionObjectWidgetRef);
			}	
		}
	}

	if(SessionQuantity <= 0)
	{
		FindSessionTextBlock->SetText(FText::FromString(TEXT("No Session available")));
	}
}
