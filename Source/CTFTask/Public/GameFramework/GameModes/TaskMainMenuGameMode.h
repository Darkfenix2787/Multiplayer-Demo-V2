// This is a task for gamescan made by Daniel Corredor.

#pragma once

#include "CoreMinimal.h"
#include "TaskGameMode.h"
#include "TaskMainMenuGameMode.generated.h"

/**
 * GameMode Class for the MainMenu Level
 */
UCLASS()
class CTFTASK_API ATaskMainMenuGameMode : public ATaskGameMode
{
	GENERATED_BODY()

public:

	virtual void PostLogin(APlayerController* NewPlayer) override;	
};
