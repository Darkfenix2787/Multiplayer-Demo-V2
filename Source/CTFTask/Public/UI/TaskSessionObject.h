// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TaskSessionObject.generated.h"

class UTaskOnlineSessionsSubsystem;
class UTextBlock;
class UButton;

/**
 * This class will handle the Session Object behaviour
 */
UCLASS()
class CTFTASK_API UTaskSessionObject : public UUserWidget
{
	GENERATED_BODY()

public:	

	/** Function to Setup the session */
	void Setup(FOnlineSessionSearchResult Session);

protected:

	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* HostTextBlock;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ServerNameTextBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayersTextBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* SessionPlayersTextBlock;	

	/** Current session variable to be handled by the widget */
	FOnlineSessionSearchResult CurrentSession;

	virtual bool Initialize() override;	

	UFUNCTION()
	void JoinButtonClicked();

	//** Function to be called by the OnlineSessionSubsytem delegate when join a session
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	

private:

	/** TaskOnlineSessionsSubsystem reference*/
	UTaskOnlineSessionsSubsystem* OnlineSessionsSubsystem;
	
};
