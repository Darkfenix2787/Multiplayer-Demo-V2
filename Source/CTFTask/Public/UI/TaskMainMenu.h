// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaskMainMenu.generated.h"

class UTextBlock;
class UScrollBox;
class UTaskSessionObject;
class UTaskOnlineSessionsSubsystem;
class UComboBoxString;
class UButton;
class UWidgetSwitcher;

/**
 * This class for handle the main menu and its behaviour
 */
UCLASS()
class CTFTASK_API UTaskMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Function to Setup the session. This will be called from level blueprint*/
	UFUNCTION(BlueprintCallable)
	void MenuSetup(FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")), int32 NumOfPlayers = 2);

protected:

	/** TaskSessionObject widget subclass */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTaskSessionObject> SessionObjectWidget;

	virtual bool Initialize() override;

	/**
	 * Called when a top level widget is in the viewport and the world is potentially coming to and end. When this occurs, 
	 * it's not save to keep widgets on the screen.  We automatically remove them when this happens and mark them for pending kill.
	 */
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	// Functions Callbacks from the subsystem custom delegates
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

private:

	/** Button to Create a server and host a game */
	UPROPERTY(meta=(BindWidget))
	UButton* CreateGameButton;

	/** Button to show the list of available servers */
	UPROPERTY(meta=(BindWidget))
	UButton* BrowseGamesButton;

	/** Button to quit the game */
	UPROPERTY(meta=(BindWidget))
	UButton* QuitGameButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CreateSessionTextBlock;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* FindSessionTextBlock;
	
	/** ScrollBox that contains the server list */
	UPROPERTY(meta=(BindWidget))
	UScrollBox* ServerListScrollBox;	

	/** WidgetSwitcher that handle the presentation between create and join buttons */
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* ContextMenuSwitcher;

	/** Reference to the customized GameInstanceSubsystem */
	UTaskOnlineSessionsSubsystem* OnlineSessionsSubsystem;

	/** Function to reset the inputs behaviour  */
	void MenuTearDown();

	/** Function to Enable/Disable Buttons */
	void EnableButtons(const bool bIsActive);

	/** Function to travel to the lobby when the session is created */
	void TravelToLobby();

	/** Buttons Callback functions */
	
	UFUNCTION()
	void CreateGameButtonClicked();	

	UFUNCTION()
	void BrowseGamesButtonClicked();
	
	UFUNCTION()
	void QuitGameButtonClicked();

	/** Number of connections allowed by the game sessions.
	 *  For the task it is limited to 2 but this can be improved to handle a custom number
	 *  on the screen
	 */
	UPROPERTY(Transient)
	int32 NumPublicConnections;

	/** This is the value for the session setting key and allows to create and
	 *  search session that have this value
	 *  */
	FString MatchType;

	/** Engine Path to the Lobby Level, this should be set on the level blueprint  */
	UPROPERTY(Transient)
	FString PathToLobby{TEXT("")};
};
