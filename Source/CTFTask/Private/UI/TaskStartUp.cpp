// This is a task for gamescan made by Daniel Corredor.


#include "UI/TaskStartUp.h"
#include "Kismet/GameplayStatics.h"

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskStartUp::MenuSetup(const FName MainMenuMapName)
{
	MainMenuLevelName = MainMenuMapName;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	FOnInputAction GoToMainMenuDelegate;
	GoToMainMenuDelegate.BindUFunction(this,FName("GoToMainMenu"));

	ListenForInputAction("PressAnyButton",EInputEvent::IE_Pressed,false, GoToMainMenuDelegate);
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void UTaskStartUp::GoToMainMenu() const
{
	const UWorld* World = GetWorld();
	if(World)
	{
		UGameplayStatics::OpenLevel(World,MainMenuLevelName);
	}
}
