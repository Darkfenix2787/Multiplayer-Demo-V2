// This is a task for gamescan made by Daniel Corredor.


#include "UI/TaskSessionObject.h"
#include "OnlineSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/TaskOnlineSessionsSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"


/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskSessionObject::Setup(FOnlineSessionSearchResult Session)
{
	if(!Session.IsValid())
	{
		return;			
	}

	CurrentSession = Session;
	ServerNameTextBlock->SetText(FText::FromString(CurrentSession.Session.OwningUserName));
	const int32 CurrentPlayers = CurrentSession.Session.SessionSettings.NumPublicConnections - CurrentSession.Session.NumOpenPublicConnections;
	const int32 MaxPlayers = CurrentSession.Session.SessionSettings.NumPublicConnections;
	SessionPlayersTextBlock->SetText(FText::FromString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers)));

	const UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UTaskOnlineSessionsSubsystem>();
	}

	if(OnlineSessionsSubsystem)
	{
		OnlineSessionsSubsystem->MultiplayerOnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSession);
	}
	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
bool UTaskSessionObject::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	return true;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskSessionObject::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);

	PlayersTextBlock->SetText(FText::FromString(TEXT("Joining")));
	if(CurrentSession.IsValid())
	{
		OnlineSessionsSubsystem->JoinSession(CurrentSession);
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskSessionObject::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
		PlayersTextBlock->SetText(FText::FromString(TEXT("Unable to Join")));
		return;
	}
	
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		const IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession,Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if(PlayerController)
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}
}
