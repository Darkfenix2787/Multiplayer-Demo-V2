// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaskStartUp.generated.h"

/**
 * This is just the startup game widget.
 */
UCLASS()
class CTFTASK_API UTaskStartUp : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Function to Setup the session. This will be called from level blueprint*/
	UFUNCTION(BlueprintCallable)
	void MenuSetup(const FName MainMenuMapName = FName(TEXT("MainMenu")));

protected:

	UFUNCTION()
	void GoToMainMenu() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MainMenuLevelName;
	
};
