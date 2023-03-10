// This is a task for gamescan made by Daniel Corredor.


#include "GameFramework/PlayerState/TaskPlayerState.h"
#include "Character/TaskCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

/** ------------------------------------------------------------------------------------------------------------------------------------ */
ATaskPlayerState::ATaskPlayerState()
{
	Team = ETeam::ET_NoTeam;
	bIsInputEnabled = true;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATaskPlayerState,Team);
	DOREPLIFETIME(ATaskPlayerState,bIsInputEnabled);	
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ATaskPlayerState* NewPlayerState = Cast<ATaskPlayerState>(PlayerState);

	if(NewPlayerState)
	{
		NewPlayerState->Team = Team;		
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerState::Client_CharacterCameraFade_Implementation(const bool bIsFadeIn)
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (IsValid(CameraManager))
	{
		if(bIsFadeIn)
		{
			CameraManager->StartCameraFade(0.f, 1.f, 2.f, FColor::Black, true, true);
		}
		else
		{
			CameraManager->StartCameraFade(1.f, 0.f, 2.f, FColor::Black, true, false);
		}
		
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerState::SetTeam(const ETeam NewTeam)
{
	Team = NewTeam;
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerState::EnableCharacterInput(const bool IsEnable)
{
	bIsInputEnabled = IsEnable;

	CharacterRef = Cast<ATaskCharacter>(GetPawn());
	
	if(CharacterRef)
	{
		CharacterRef->EnableCharacterInput(IsEnable);		
	}
}

/** ------------------------------------------------------------------------------------------------------------------------------------ */
void ATaskPlayerState::OnRep_IsInputEnabled()
{
	CharacterRef = Cast<ATaskCharacter>(GetPawn());
	
	if(CharacterRef)
	{
		CharacterRef->EnableCharacterInput(bIsInputEnabled);		
	}
}